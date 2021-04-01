#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/time.h>

#if !defined(_WIN32)
    #include <termios.h>
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

char VER[] = "0.12.7.1";

#ifndef BUFSIZE
    #define BUFSIZE 32768
#endif

#if defined(__linux__)
    char OSVER[] = "Linux";
#elif defined(BSD)
    char OSVER[] = "BSD";
#elif defined(__unix__)
    char OSVER[] = "UNIX";
#elif _WIN32
    char OSVER[] = "Windows";
    //(https://pbs.twimg.com/media/CRcU7BKWwAEQZIE.jpg)
    #include "termios_win.h"
    #include <windows.h>
    #define SIGKILL 9
    char* rlptr;
    void cleanExit();
    void setcsr() {
        COORD coord;
        coord.X = 0;
        coord.Y = 0;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }
    char* readline(char* prompt) {
        //(https://theenglishfarm.com/sites/default/files/styles/featured_image/public/harold_2.jpg?itok=uo6h4hz4)
        printf(prompt);
        fflush(stdout);
        char buf[BUFSIZE];
        buf[0] = 0;
        int inct = scanf("%[^\n]s", &buf);
        if (inct != 1 && inct != 0) raise(SIGINT);
        int tmpc = 0;
        read(1, &tmpc, 1);
        if (tmpc == 3) raise(SIGINT);
        while (getchar() != '\n') {}
        rlptr = malloc(strlen(buf) + 1);
        strcpy(rlptr, buf);
        return rlptr;
    }
    void add_history(char* str) {}
    void read_history(char* str) {}
    void write_history(char* str) {}
    //(https://i.kym-cdn.com/entries/icons/original/000/027/746/crying.jpg)
#elif __APPLE__
    char OSVER[] = "MacOS";
#else
    char OSVER[] = "?";
#endif

#ifdef B32
    char BVER[] = "32";
#elif B64
    char BVER[] = "64";
#else
    char BVER[] = "?";
#endif

FILE *prog;
char* progbuf;
FILE *f[256];

int err = 0;
int cerr;

bool inProg = false;
bool chkinProg = false;
char* progFilename;
int progLine = 1;

int* varlen;
char** varstr;
char** varname;
bool *varinuse;
uint8_t *vartype;
int varmaxct = 0;

char* cmd;
int cmdl;
char** tmpargs;
char** arg;
uint8_t *argt;
int* argl;
int argct = 0;

int cmdpos;

int dlstack[256];
int dlpline[256];
bool dldcmd[256];
int dlstackp = -1;
bool didloop = false;
bool lockpl = false;

bool itdcmd[256];
int itstackp = -1;
bool didelse = false;
/*
int fnstack[256];
bool fndcmd[256];
int fnstackp = -1;
*/
char* errstr;

char conbuf[BUFSIZE];
char lastcb[BUFSIZE];
char prompt[BUFSIZE];
char pstr[BUFSIZE];

uint8_t fgc = 15;
uint8_t bgc = 0;

int curx;
int cury;

int64_t cp = 0;

bool cmdint = false;

bool debug = false;
bool runfile = false;

bool sh_silent = false;
bool sh_clearAttrib = true;
bool sh_restoreAttrib = true;

bool txt_bold = false;
bool txt_italic = false;
bool txt_underln = false;
bool txt_underlndbl = false;
bool txt_underlnsqg = false;
bool txt_strike = false;
bool txt_overln = false;
bool txt_dim = false;
bool txt_blink = false;
bool txt_hidden = false;
bool txt_reverse = false;
int  txt_underlncolor = 0;

struct termios term, restore;
static struct termios orig_termios;
bool textlock = false;

struct timeval time1, time2;
uint64_t t_start;

#define qstrcmp(a, b)  (*(a) != *(b) ? \
    (int) ((unsigned char) *(a) - \
    (unsigned char) *(b)) : \
    strcmp((a), (b)))

void forceExit() {
    if (textlock) {tcsetattr(0, TCSANOW, &restore); textlock = false;}
    exit(0);
}

void getCurPos();

void cleanExit() {
    if (textlock) {tcsetattr(0, TCSANOW, &restore); textlock = false;}
    signal(SIGINT, forceExit);
    signal(SIGKILL, forceExit);
    signal(SIGTERM, forceExit);
    printf("\e[0m");
    fflush(stdout);
    getCurPos();
    if (curx != 1) printf("\n");
    exit(err);
}

void cmdIntHndl() {
    if (cmdint) signal(SIGINT, cleanExit);
    cmdint = true;
}

void runcmd();
int copyStr(char* str1, char* str2);
void copyStrSnip(char* str1, int i, int j, char* str2);
uint8_t getVal(char* inbuf, char* outbuf);
void resetTimer();
void loadProg();
void updateTxtAttrib();

int main(int argc, char* argv[]) {
    signal(SIGINT, cleanExit); 
    signal(SIGKILL, cleanExit); 
    signal(SIGTERM, cleanExit); 
    getCurPos();
    if (curx != 1) printf("\n");
    bool exit = false;
    for (int i = 1; i < argc; i++) {
        if (!qstrcmp(argv[i], "--version") || !qstrcmp(argv[i], "-v")) {
            if (argc > 2) {printf("Incorrent number of options passed.\n"); cleanExit();}
            printf("Command Line Interface BASIC version %s (%s %s-bit)\n", VER, OSVER, BVER);
            printf("Copyright (C) 2021 PQCraft\n");
            exit = true;
        } else
        if (!qstrcmp(argv[i], "--help") || !qstrcmp(argv[i], "-h")) {
            if (argc > 2) {printf("Incorrent number of options passed.\n"); cleanExit();}
            printf("Usage: clibasic [options] [file]\n");
            printf("\n");
            printf("  -h, --help      Shows the help screen.\n");
            printf("  -v, --version   Shows the version info.\n");
            printf("  -d, --debug     Quick way to create a text wall.\n");
            exit = true;
        } else
        if (!qstrcmp(argv[i], "--debug") || !qstrcmp(argv[i], "-d")) {
            if (debug || argc > 3) {printf("Incorrent number of options passed.\n"); cleanExit();}
            debug = true;
        } else {
            if (runfile || argc > 3) {free(progFilename); printf("Incorrent number of options passed.\n"); cleanExit();}
            inProg = true;
            runfile = true;
            prog = fopen(argv[i], "r");
            progFilename = malloc(strlen(argv[i]));
            copyStr(argv[i], progFilename);
            if (prog == NULL) {printf("File not found: '%s'\n", progFilename); free(progFilename); cleanExit();}
        }
    }
    if (exit) cleanExit();
    updateTxtAttrib();
    if (!runfile) {
        printf("Command Line Interface BASIC version %s (%s %s-bit)\n", VER, OSVER, BVER);
        strcpy(prompt, "\"CLIBASIC> \"");
    }
    if (debug) printf("Running in debug mode\n");
    progbuf = malloc(0);
    errstr = malloc(0);
    cmd = malloc(0);
    argt = malloc(0);
    arg = malloc(0);
    srand(time(0));
    if (!runfile) {
        prog = fopen("AUTORUN.BAS", "r"); progFilename = malloc(12); strcpy(progFilename, "AUTORUN.BAS");
        if (prog == NULL) {prog = fopen("autorun.bas", "r"); strcpy(progFilename, "autorun.bas");}
        if (prog == NULL) {free(progFilename);}
        else {inProg = true;}
    } else {
        loadProg();
    }
    resetTimer();
    while (!exit) {
        fchkint:
        conbuf[0] = 0;
        if (chkinProg) {inProg = true; chkinProg = false;}
        if (!inProg) {
            if (runfile) {if (textlock) {tcsetattr(0, TCSANOW, &restore); textlock = false;} err = cerr; cleanExit();}
            char* tmpstr = NULL;
            int tmpt = getVal(prompt, pstr);
            if (tmpt != 1) strcpy(pstr, "CLIBASIC> ");
            updateTxtAttrib();
            getCurPos();
            if (curx != 1) printf("\n");
            while (tmpstr == NULL) {tmpstr = readline(pstr);}
            if (tmpstr[0] == 0) {free(tmpstr); goto brkproccmd;}
            if (qstrcmp(tmpstr, lastcb)) add_history(tmpstr);
            copyStr(tmpstr, conbuf);
            copyStr(tmpstr, lastcb);
            free(tmpstr);
        }
        cp = 0;
        cmdl = 0;
        dlstackp = -1;
        itstackp = -1;
        for (int i = 0; i < 256; i++) {
            dlstack[i] = 0;
            dldcmd[i] = false;
            itdcmd[i] = false;
        }
        didloop = false;
        didelse = false;
        bool inStr = false;
        if (!runfile) signal(SIGINT, cmdIntHndl);
        progLine = 1;
        while (1) {
            if (!inProg) {
                if (conbuf[cp] == '"') {inStr = !inStr; cmdl++;} else
                if ((conbuf[cp] == ':' && !inStr) || conbuf[cp] == 0) {
                    while (conbuf[cp - cmdl] == ' ' && cmdl > 0) {cmdl--;}
                    cmd = realloc(cmd, (cmdl + 1) * sizeof(char));
                    cmdpos = cp - cmdl;
                    copyStrSnip(conbuf, cp - cmdl, cp, cmd);
                    cmdl = 0;
                    if (debug) printf("calling runcmd()\n");
                    runcmd();
                    if (cmdint) {if (textlock) {tcsetattr(0, TCSANOW, &restore); textlock = false;} cmdint = false; goto brkproccmd;}
                    if (cp == -1) goto brkproccmd;
                    if (conbuf[cp] == 0) goto brkproccmd;
                    if (chkinProg) goto fchkint;
                } else
                {cmdl++;}
                if (!didloop) {cp++;} else {didloop = false;}
            } else {
                //if (cmdint) {inProg = false; fclose(prog); free(progFilename); cmdint = false; goto brkproccmd;}
                if (progbuf[cp] == '"') {inStr = !inStr; cmdl++;} else
                if ((progbuf[cp] == ':' && !inStr) || progbuf[cp] == '\n' || progbuf[cp] == 0) {
                    if (progbuf[cp - cmdl - 1] == '\n' && !lockpl) {progLine++; if (debug) printf("found nl: [%lld]\n", (long long int)cp);}
                    if (lockpl) lockpl = false;
                    while (progbuf[cp - cmdl] == ' ' && cmdl > 0) {cmdl--;}
                    cmd = realloc(cmd, (cmdl + 1) * sizeof(char));
                    cmdpos = cp - cmdl;
                    copyStrSnip(progbuf, cp - cmdl, cp, cmd);
                    cmdl = 0;
                    if (debug) printf("conbuf: {%s}\n", conbuf);
                    runcmd();
                    if (cmdint) {inProg = false; fclose(prog); free(progFilename); cmdint = false; goto brkproccmd;}
                    if (cp == -1) {inProg = false; fclose(prog); free(progFilename); goto brkproccmd;}
                    if (progbuf[cp] == 0) {inProg = false; fclose(prog); free(progFilename); goto brkproccmd;}
                } else
                {cmdl++;}
                if (!didloop) {cp++;} else {didloop = false;}
            }
        }
        brkproccmd:
        signal(SIGINT, cleanExit);
        if (textlock) {tcsetattr(0, TCSANOW, &restore); textlock = false;}
    }
    if (textlock) {tcsetattr(0, TCSANOW, &restore); textlock = false;}
    cleanExit();
    return 0;
}

uint64_t time_us() {
    gettimeofday(&time1, NULL);
    return time1.tv_sec * 1000000 + time1.tv_usec;
}

uint64_t timer() {
    return time_us() - t_start;
}

void resetTimer() {
    t_start = time_us();
}

void getCurPos() {
    fflush(stdout);
    cury = 0; curx = 0;
    #ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO con;
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hcon != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hcon, &con)) {
        curx = con.dwCursorPosition.X + 1;
        cury = con.dwCursorPosition.Y + 1;
    }
    #else
    char buf[16]={0};
    int ret, i, pow;
    char ch;
    if (!textlock) {
        tcgetattr(0, &term);
        tcgetattr(0, &restore);
        term.c_lflag &= ~(ICANON|ECHO);
        tcsetattr(0, TCSANOW, &term);
    }
    while (write(1, "\e[6n", 4) == -1) {}
    for (i = 0, ch = 0; ch != 'R'; i++) {
        ret = read(1, &ch, 1);
        if (!ret) {
            if (!textlock) tcsetattr(0, TCSANOW, &restore);
            return;
        }
        buf[i] = ch;
    }
    if (i < 2) {
        if (!textlock) tcsetattr(0, TCSANOW, &restore);
        return;
    }
    for (i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10) {
        curx += (buf[i] - '0') * pow;
    }
    for(i--, pow = 1; buf[i] != '['; i--, pow *= 10) {
        cury += (buf[i] - '0') * pow;
    }
    if (!textlock) tcsetattr(0, TCSANOW, &restore);
    return;
    #endif
}

void enableRawMode() {
    struct termios raw;
    if (!isatty(STDIN_FILENO)) exit(EXIT_FAILURE);
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) exit(EXIT_FAILURE);
    raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) exit(EXIT_FAILURE);
    return;
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void loadProg() {
    fseek(prog, 0, SEEK_END);
    progbuf = realloc(progbuf, ftell(prog) + 1);
    fseek(prog, 0, SEEK_SET);
    int64_t j = 0;
    bool comment = false;
    bool inStr = false;
    while (!feof(prog)) {
        int tmpc = fgetc(prog);
        if (tmpc == '"') inStr = !inStr;
        if (tmpc == '\'' && !inStr) comment = true;
        if (tmpc == '\n') comment = false;
        if (tmpc == '\r' || tmpc == '\t') tmpc = ' ';
        if (!comment) {progbuf[j] = (char)tmpc; j++;}
    }
    if (j == 0) j = 1;
    progbuf[j - 1] = 0;
}

double randNum(double num1, double num2) {
    double range = num2 - num1;
    double div = RAND_MAX / range;
    return num1 + (rand() / div);
}

bool chkCmd(int ct, ...) {
    va_list args;
    va_start(args, ct);
    char* str0 = va_arg(args, char*);
    bool match = false;
    for (int i = 0; i < ct; i++) {
        char* str1 = str0;
        char* str2 = va_arg(args, char*);
        while (1) {
            if (!*str1 && !*str2) break;
            if (*str1 != *str2) goto nmatch;
            str1++;
            str2++;
        }
        match = true;
        nmatch:
        if (match) return true;
    }
    return false;
}

bool isSpChar(char* bfr, int pos) {
    return (bfr[pos] == '+' || bfr[pos] == '-' || bfr[pos] == '*' || bfr[pos] == '/' || bfr[pos] == '^');
}

bool isValidVarChar(char* bfr, int pos) {
    return ((bfr[pos] >= 'A' && bfr[pos] <= 'Z') || (bfr[pos] >= 'a' && bfr[pos] <= 'z') || (bfr[pos] >= '0' && bfr[pos] <= '9')
    || bfr[pos] == '_' || bfr[pos] == '$' || bfr[pos] == '%' || bfr[pos] == '&' || bfr[pos] == '!' || bfr[pos] == '~');
}

int copyStr(char* str1, char* str2) {
    int i;
    for (i = 0; str1[i] != 0; i++) {str2[i] = str1[i];}
    str2[i] = 0;
    return i;
}

void copyStrSnip(char* str1, int i, int j, char* str2) {
    int i2 = 0;
    for (int i3 = i; i3 < j; i3++) {str2[i2] = str1[i3]; i2++;}
    str2[i2] = 0;
}

void copyStrTo(char* str1, int i, char* str2) {
    int i2 = 0;
    int i3;
    for (i3 = i; str1[i] != 0; i++) {str2[i3] = str1[i2]; i2++; i3++;}
    str2[i3] = 0;
}

void copyStrApnd(char* str1, char* str2) {
    int j = 0, i = 0;
    for (i = strlen(str2); str1[j] != 0; i++) {str2[i] = str1[j]; j++;}
    str2[i] = 0;
}

void upCase(char* str) {
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') str[i] -= 32;
    }
}

void lowCase(char* str) {
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') str[i] += 32;
    }
}

void updateTxtAttrib() {
    fputs("\e[0m", stdout);
    printf("\e[0m\e[38;5;%um\e[48;5;%um", fgc, bgc);
    if (txt_bold) fputs("\e[1m", stdout);
    if (txt_italic) fputs("\e[3m", stdout);
    if (txt_underln) fputs("\e[4m", stdout);
    if (txt_underlndbl) fputs("\e[21m", stdout);
    if (txt_underlnsqg) fputs("\e[4:3m", stdout);
    if (txt_strike) fputs("\e[9m", stdout);
    if (txt_overln) fputs("\e[53m", stdout);
    if (txt_dim) fputs("\e[2m", stdout);
    if (txt_blink) fputs("\e[5m", stdout);
    if (txt_hidden) fputs("\e[8m", stdout);
    if (txt_reverse) fputs("\e[7m", stdout);
    if (txt_underlncolor) printf("\e[58:5:%um", txt_underlncolor);
    fflush(stdout);
}

void getStr(char* str1, char* str2) {
    char buf[BUFSIZE];
    int j = 0, i;
    for (i = 0; str1[i] != 0; i++) {
        char c = str1[i];
        if (c == '\\') {
            i++;
            char h[5];
            unsigned int tc = 0;
            switch (str1[i]) {
                case 'n': c = '\n'; break;
                case 'r': c = '\r'; break;
                case 'f': c = '\f'; break;
                case 't': c = '\t'; break;
                case 'b': c = '\b'; break;
                case 'e': c = '\e'; break;
                case 'x':
                    h[0] = '0';
                    h[1] = str1[i]; i++;
                    h[2] = str1[i]; i++;
                    h[3] = str1[i];
                    h[4] = 0;
                    sscanf(h, "%x", &tc);
                    c = (char)tc;
                    break;
                case '\\': c = '\\'; break;
                default: i--; break;
            }
        }
        buf[j] = c;
        j++;
    }
    buf[j] = 0;
    copyStr(buf, str2);
}

uint8_t getType(char* str) {
    if (str[0] == '"') {if (str[strlen(str) - 1] != '"') {return 0;} return 1/*STR*/;}
    bool p = false;
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] == '-') {} else
        if ((str[i] < '0' || str[i] > '9') && str[i] != '.') {return 255 /*VAR*/;} else
        if (str[i] == '.') {if (p) {return 0 /*ERR*/;} p = true;}
    }
    return 2/*NUM*/;
}

uint8_t getVal(char* inbuf, char* outbuf);
int getArg(int num, char* inbuf, char* outbuf);
int getArgCt(char* inbuf);

uint8_t getFunc(char* inbuf, char* outbuf) {
    if (debug) printf("getFunc(\"%s\", \"%s\");\n", inbuf, outbuf);
    char tmp[2][BUFSIZE];
    char** farg;
    uint8_t *fargt;
    int* flen;
    int fargct;
    int ftmpct = 0;
    int ftype = 0;
    int i;
    for (i = 0; inbuf[i] != '('; i++) {if (inbuf[i] >= 'a' && inbuf[i] <= 'z') inbuf[i] = inbuf[i] - 32;}
    copyStrSnip(inbuf, i + 1, strlen(inbuf) - 1, tmp[0]);
    fargct = getArgCt(tmp[0]);
    farg = malloc((fargct + 1) * sizeof(char*));
    flen = malloc((fargct + 1) * sizeof(int));
    fargt = malloc((fargct + 1) * sizeof(uint8_t));
    for (int j = 0; j <= fargct; j++) {
        if (j == 0) {
            flen[0] = i;
            farg[0] = (char*)malloc((flen[0] + 1) * sizeof(char));
            errstr = realloc(errstr, (flen[0] + 1) * sizeof(char));
            copyStrSnip(inbuf, 0, i, farg[0]);
            copyStrSnip(inbuf, 0, i, errstr);
        } else {
            getArg(j - 1, tmp[0], tmp[1]);
            fargt[j] = getVal(tmp[1], tmp[1]);
            if (fargt[j] == 0) goto fexit;
            if (fargt[j] == 255) fargt[j] = 0;
            flen[j] = strlen(tmp[1]);
            farg[j] = (char*)malloc((flen[j] + 1) * sizeof(char));
            copyStr(tmp[1], farg[j]);
            if (debug) printf("farg[%d]: {%s}\n", j, farg[j]);
            ftmpct++;
        }
    }
    outbuf[0] = 0;
    cerr = 127;
    #include "functions.c"
    fexit:
    for (int j = 0; j <= ftmpct; j++) {
        free(farg[j]);
    }
    free(farg);
    free(flen);
    free(fargt);
    if (debug) printf("output: getFunc(\"%s\", \"%s\");\n", inbuf, outbuf);
    if (cerr != 0) return 0;
    return ftype;
}

uint8_t getVar(char* vn, char* varout) {
    if (debug) printf("getVar(\"%s\", \"%s\");\n", vn, varout);
    if (vn[0] == 0) return 0;
    if (vn[strlen(vn) - 1] == ')') {
        return getFunc(vn, varout);
    }
    upCase(vn);
    for (int i = 0; vn[i] != 0; i++) {
        if (!isValidVarChar(vn, i)) {
            cerr = 4;
            errstr = realloc(errstr, (strlen(vn) + 1) * sizeof(char));
            copyStr(vn, errstr);
            return 0;
        }
    }
    int v = -1;
    if (debug) printf("varmaxct: [%d]\n", varmaxct);
    for (int i = 0; i < varmaxct; i++) {
        if (debug) printf("varname[%d]: {%s}\n", i, varname[i]);
        if (varinuse[i]) {if (!qstrcmp(vn, varname[i])) {v = i; break;}}
    }
    if (debug) printf("getVar: v: [%d]\n", v);
    if (v == -1) {
        if (vn[strlen(vn) - 1] == '$') {varout[0] = 0; return 1;}
        else {varout[0] = '0'; varout[1] = 0; return 2;}
    } else {
        copyStr(varstr[v], varout);
        if (debug) printf("varout: {%s}\n", varout);
        return vartype[v];
    }
    return 0;
}

bool setVar(char* vn, char* val, uint8_t t) {
    if (debug) printf("setVar(\"%s\", \"%s\", %d);\n", vn, val, (int)t);
    upCase(vn);
    for (int i = 0; vn[i] != 0; i++) {
        if (!isValidVarChar(vn, i)) {
            cerr = 4;
            errstr = realloc(errstr, (strlen(vn) + 1) * sizeof(char));
            copyStr(vn, errstr);
            return false;
        }
    }
    if (getType(vn) != 255) {
        cerr = 4;
        errstr = realloc(errstr, (strlen(vn) + 1) * sizeof(char));
        copyStr(vn, errstr);
        return false;
    }
    int v = -1;
    for (int i = 0; i < varmaxct; i++) {
        if (!qstrcmp(vn, varname[i])) {v = i; break;}
    }
    if (v == -1) {
        v = varmaxct;
        varstr = realloc(varstr, (v + 1) * sizeof(char*));
        varname = realloc(varname, (v + 1) * sizeof(char*));
        varlen = realloc(varlen, (v + 1) * sizeof(int));
        varinuse = (bool*)realloc(varinuse, (v + 1) * sizeof(bool));
        varinuse[v] = true;
        vartype = (uint8_t*)realloc(vartype, (v + 1) * sizeof(uint8_t));
        varlen[v] = strlen(val);
        varstr[v] = (char*)malloc((varlen[v] + 1) * sizeof(char));
        varname[v] = (char*)malloc((strlen(vn) + 1) * sizeof(char));
        copyStr(vn, varname[v]);
        copyStr(val, varstr[v]);
        vartype[v] = t;
        varmaxct++;
    } else {
        if (t != vartype[v]) {cerr = 2; return false;}
        varlen[v] = strlen(val);
        varstr[v] = (char*)realloc(varstr[v], (varlen[v] + 1) * sizeof(char));
        copyStr(val, varstr[v]);
    }
    return true;
}

bool gvchkchar(char* tmp, int i) {
    if (isSpChar(tmp, i + 1)) {
        if (tmp[i + 1] == '-') {
            if (isSpChar(tmp, i + 2)) {
                cerr = 1; return false;
            }
        } else {
            cerr = 1; return false;
        }
    } else {
        if (isSpChar(tmp, i - 1)) {
            cerr = 1; return false;
        }
    }
    return true;
}

uint8_t getVal(char* tmpinbuf, char* outbuf) {
    if (debug) printf("getVal(\"%s\", \"%s\");\n", tmpinbuf, outbuf);
    if (tmpinbuf[0] == 0) {return 255;}
    char inbuf[BUFSIZE];
    copyStr(tmpinbuf, inbuf);
    outbuf[0] = 0;
    int ip = 0, jp = 0;
    char tmp[4][BUFSIZE];
    uint8_t t = 0;
    uint8_t dt = 0;
    bool inStr = false;
    double num1 = 0;
    double num2 = 0;
    double num3 = 0;
    int numAct;
    if ((isSpChar(inbuf, 0) && inbuf[0] != '-') || isSpChar(inbuf, strlen(inbuf) - 1)) {cerr = 1; return 0;}
    int tmpct = 0;
    tmp[0][0] = 0; tmp[1][0] = 0; tmp[2][0] = 0; tmp[3][0] = '"'; tmp[3][1] = 0;
    for (int i = 0; inbuf[i] != 0; i++) {
        if (inbuf[i] == '(') {if (tmpct == 0) {ip = i;} tmpct++;}
        if (inbuf[i] == ')') {
            tmpct--;
            if (tmpct == 0 && (ip == 0 || isSpChar(inbuf, ip - 1))) {
                jp = i;
                copyStrSnip(inbuf, ip + 1, jp, tmp[0]);
                t = getVal(tmp[0], tmp[0]);
                if (t == 0) return 0;
                if (dt == 0) dt = t;
                if (t == 255) {t = 1; dt = 1;}
                if (t != dt) {cerr = 2; return 0;}
                copyStrSnip(inbuf, 0, ip, tmp[1]);
                copyStrApnd(tmp[1], tmp[2]);
                if (t == 1) copyStrApnd(tmp[3], tmp[2]);
                copyStrApnd(tmp[0], tmp[2]);
                if (t == 1) copyStrApnd(tmp[3], tmp[2]);
                copyStrSnip(inbuf, jp + 1, strlen(inbuf), tmp[1]);
                copyStrApnd(tmp[1], tmp[2]);
                copyStr(tmp[2], inbuf);
                tmp[0][0] = 0; tmp[1][0] = 0; tmp[2][0] = 0; tmp[3][0] = 0;
            }
        }
    }
    if (tmpct != 0) {cerr = 1; return 0;}
    ip = 0; jp = 0;
    tmp[0][0] = 0; tmp[1][0] = 0; tmp[2][0] = 0; tmp[3][0] = 0;
    while (true) {
        bool seenStr = false;
        int pct = 0;
        while (true) {
            if (inbuf[jp] == '"') {inStr = !inStr; if (seenStr && inStr) {cerr = 1; return 0;} seenStr = true;}
            if (inbuf[jp] == '(') {pct++;}
            if (inbuf[jp] == ')') {pct--;}
            if ((isSpChar(inbuf, jp) && !inStr && pct == 0) || inbuf[jp] == 0) goto gvbrk;
            jp++;
        }
        gvbrk:
        copyStrSnip(inbuf, ip, jp, tmp[0]);
        t = getType(tmp[0]);
        if (t == 255) {
            t = getVar(tmp[0], tmp[0]);
            if (t == 0) {return 0;}
            if (t == 1) {
                tmp[2][0] = '"'; tmp[2][1] = 0;
                copyStr(tmp[0], tmp[3]);
                copyStr(tmp[2], tmp[0]);
                copyStrApnd(tmp[3], tmp[0]);
                copyStrApnd(tmp[2], tmp[0]);
            }
        }
        if (t != 0 && dt == 0) {dt = t;} else
        if ((t != 0 && t != dt)) {cerr = 2; return 0;} else
        if (t == 0) {cerr = 1; return false;}
        if ((dt == 1 && inbuf[jp] != '+') && inbuf[jp] != 0) {cerr = 1; return 0;}
        if (t == 1) {copyStrSnip(tmp[0], 1, strlen(tmp[0]) - 1, tmp[2]); getStr(tmp[2], tmp[2]); copyStrApnd(tmp[2], tmp[1]);} else
        if (t == 2) {
            copyStr(inbuf, tmp[0]);
            int p1, p2, p3;
            bool inStr = false;
            pct = 0;
            while (true) {
                numAct = 0;
                p1 = 0, p2 = 0, p3 = 0;
                for (int i = 0; tmp[0][i] != 0 && p2 == 0; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (tmp[0][i] == '(') {pct++;}
                    if (tmp[0][i] == ')') {pct--;}
                    if (tmp[0][i] == '^' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 4;}
                }
                for (int i = 0; tmp[0][i] != 0 && p2 == 0; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (tmp[0][i] == '(') {pct++;}
                    if (tmp[0][i] == ')') {pct--;}
                    if (tmp[0][i] == '*' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 2;}
                    if (tmp[0][i] == '/' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 3;}
                }
                for (int i = 0; tmp[0][i] != 0 && p2 == 0; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (tmp[0][i] == '(') {pct++;}
                    if (tmp[0][i] == ')') {pct--;}
                    if (tmp[0][i] == '+' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 0;}
                    if (tmp[0][i] == '-' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 1;}
                }
                inStr = false;
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                if (p2 == 0) {
                    if (p3 == 0) {
                        t = getType(tmp[0]);
                        if (t == 0) {cerr = 1; return 0;} else
                        if (t == 255) {t = getVar(tmp[0], tmp[0]);
                        if (t == 0) {return 0;}
                        if (t == 255) {t = 2; tmp[0][0] = '0'; tmp[0][1] = 0;}
                        if (t != 2) {cerr = 2; return 0;}}
                    }
                    copyStr(tmp[0], tmp[1]); goto gvfexit;
                }
                tmp[1][0] = 0; tmp[2][0] = 0; tmp[3][0] = 0;
                for (int i = p2 - 1; i > 0; i--) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (isSpChar(tmp[0], i) && !inStr) {p1 = i; break;}
                }
                for (int i = p2 + 1; true; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if ((isSpChar(tmp[0], i) && i != p2 + 1 && !inStr) || tmp[0][i] == 0) {p3 = i; break;}
                }
                if (tmp[0][p1] == '+') p1++;
                copyStrSnip(tmp[0], p1, p2, tmp[2]);
                t = getType(tmp[2]);
                if (t == 0) {cerr = 1; return 0;} else
                if (t == 255) {t = getVar(tmp[2], tmp[2]); if (t == 0) {return 0;} if (t != 2) {cerr = 2; return 0;}}
                copyStrSnip(tmp[0], p2 + 1, p3, tmp[3]);
                t = getType(tmp[3]);
                if (t == 0) {cerr = 1; return 0;} else
                if (t == 255) {t = getVar(tmp[3], tmp[3]); if (t == 0) {return 0;} if (t != 2) {cerr = 2; return 0;}}
                if (debug) printf("getVal: p1: [%d], p2: [%d], p3: [%d]\n", p1, p2, p3);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                sscanf(tmp[2], "%lf", &num1);
                sscanf(tmp[3], "%lf", &num2);
                if (debug) printf("num1: [%lf], num2: [%lf], num3: [%lf]\n", num1, num2, num3);
                switch (numAct) {
                    case 0: num3 = num1 + num2; break;
                    case 1: num3 = num1 - num2; break;
                    case 2: num3 = num1 * num2; break;
                    case 3: if (num2 == 0) {cerr = 5; return 0;} num3 = num1 / num2; break;
                    case 4:
                        if (num1 == 0) {if (num2 == 0) {cerr = 5; return 0;} num3 = 0; break;}
                        if (num2 == 0) {num3 = 1; break;}
                        num3 = pow(num1, num2);
                        if (num1 < 0 && num3 > 0) {num3 *= -1;}
                        break;
                }
                if (debug) printf("num1: [%lf], num2: [%lf], num3: [%lf]\n", num1, num2, num3);
                tmp[1][0] = 0;
                sprintf(tmp[2], "%lf", num3);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                if (dt == 2) {
                    bool dp = false;
                    int i = 0;
                    while (tmp[2][i] != 0) {if (tmp[2][i] == '.') {dp = true;} i++;}
                    while (dp && tmp[2][strlen(tmp[2]) - 1] == '0') {tmp[2][strlen(tmp[2]) - 1] = 0;}
                    if (dp && tmp[2][strlen(tmp[2]) - 1] == '.') {tmp[2][strlen(tmp[2]) - 1] = 0;}
                }
                copyStrSnip(tmp[0], p3, strlen(tmp[0]), tmp[3]);
                if (p1 != 0) copyStrSnip(tmp[0], 0, p1, tmp[1]);
                copyStrApnd(tmp[2], tmp[1]);
                copyStrApnd(tmp[3], tmp[1]);
                copyStr(tmp[1], tmp[0]);
            }
        }
        if (inbuf[jp] == 0) {break;}
        jp++;
        ip = jp;
    }
    gvfexit:
    if (dt == 2) {
        bool dp = false;
        int i = 0;
        while (tmp[1][i] != 0) {if (tmp[1][i] == '.') {dp = true;} i++;}
        while (dp && tmp[1][strlen(tmp[1]) - 1] == '0') {tmp[1][strlen(tmp[1]) - 1] = 0;}
        if (dp && tmp[1][strlen(tmp[1]) - 1] == '.') {tmp[1][strlen(tmp[1]) - 1] = 0;}
        i = 0;
        while (tmp[1][i] == '0' && tmp[1][i + 1] != 0) {i++;}
        copyStrSnip(tmp[1], i, strlen(tmp[1]), outbuf);
    } else {
        copyStr(tmp[1], outbuf);
    }
    if (outbuf[0] == 0 && dt != 1) {outbuf[0] = '0'; outbuf[1] = 0; return 2;}
    if (debug) printf("output: getVal(\"%s\", \"%s\");\n", inbuf, outbuf);
    return (uint8_t)dt;
}

bool solvearg(int i) {
    char tmpbuf[BUFSIZE];
    if (i == 0) {
        argt[0] = 0;
        arg[0] = tmpargs[0];
        argl[0] = strlen(arg[0]);
        return true;
    }
    argt[i] = 0;
    tmpbuf[0] = 0;
    argt[i] = getVal(tmpargs[i], tmpbuf);
    arg[i] = realloc(arg[i], (strlen(tmpbuf) + 1) * sizeof(char));
    copyStr(tmpbuf, arg[i]);
    argl[i] = strlen(arg[i]);
    if (argt[i] == 0) return false;
    if (argt[i] == 255) {argt[i] = 0;}
    return true;
}

int getArgCt(char* inbuf) {
    int ct = 0;
    bool inStr = false;
    int pct = 0;
    int j = 0;
    while (inbuf[j] == ' ') {j++;}
    for (int i = j; inbuf[i] != 0; i++) {
        if (ct == 0) ct = 1;
        if (inbuf[i] == '(' && !inStr) {pct++;}
        if (inbuf[i] == ')' && !inStr) {pct--;}
        if (inbuf[i] == '"') inStr = !inStr;
        if (inbuf[i] == ',' && !inStr && pct == 0) ct++;
    }
    return ct;
}

int getArg(int num, char* inbuf, char* outbuf) {
    bool inStr = false;
    int pct = 0;
    int ct = 0;
    int len = 0;
    for (int i = 0; inbuf[i] != 0 && ct <= num; i++) {
        if (inbuf[i] == '(' && !inStr) {pct++;}
        if (inbuf[i] == ')' && !inStr) {pct--;}
        if (inbuf[i] == '"') {inStr = !inStr;}        
        if (inbuf[i] == ' ' && !inStr) {} else
        if (inbuf[i] == ',' && !inStr && pct == 0) {ct++;} else
        if (ct == num) {outbuf[len] = inbuf[i]; len++;}
    }
    outbuf[len] = 0;
    return len;
}

void mkargs() {
    char tmpbuf[2][BUFSIZE];
    int j = 0;
    while (cmd[j] == ' ') {j++;}
    int h = j;
    while (cmd[h] != ' ' && cmd[h] != '=' && cmd[h] != 0) {h++;}
    copyStrSnip(cmd, h + 1, strlen(cmd), tmpbuf[0]);
    int tmph = h;
    while (cmd[tmph] == ' ' && cmd[tmph] != 0) {tmph++;}
    if (cmd[tmph] == '=') {
        strcpy(tmpbuf[1], "SET ");
        cmd[tmph] = ',';
        copyStrApnd(cmd, tmpbuf[1]);
        cmd = realloc(cmd, (strlen(tmpbuf[1]) + 1) * sizeof(char));
        copyStr(tmpbuf[1], cmd);
        copyStr(tmpbuf[1], tmpbuf[0]);
        tmpbuf[1][0] = 0;
        h = 3;
        j = 0;
    }
    if (debug) printf("cmd: {%s}\n", cmd);
    if (debug) printf("tmpbuf[0]: {%s}\n", tmpbuf[0]);
    if (debug) printf("tmpbuf[1]: {%s}\n", tmpbuf[1]);
    argct = getArgCt(tmpbuf[0]);
    tmpargs = realloc(tmpargs, (argct + 1) * sizeof(char*));
    argl = realloc(argl, (argct + 1) * sizeof(int));
    for (int i = 0; i <= argct; i++) {
        argl[i] = 0;
        if (i == 0) {
            copyStrSnip(cmd, j, h, tmpbuf[0]);
            argl[i] = strlen(tmpbuf[0]);
            tmpargs[i] = malloc((argl[i] + 1) * sizeof(char));
            copyStr(tmpbuf[0], tmpargs[i]);            
            copyStrSnip(cmd, h + 1, strlen(cmd), tmpbuf[0]);
        } else {
            argl[i] = getArg(i - 1, tmpbuf[0], tmpbuf[1]);
            tmpargs[i] = malloc((argl[i] + 1) * sizeof(char));
            copyStr(tmpbuf[1], tmpargs[i]);
        }
        if (debug) printf("length of arg[%d]: %d\n", i, argl[i]);
        tmpargs[i][argl[i]] = 0;
    }
    if (argct == 1 && tmpargs[1][0] == 0) {argct = 0;}
    for (int i = 0; i <= argct; i++) {tmpargs[i][argl[i]] = 0;}
}

uint8_t logictest(char* inbuf) {
    if (debug) printf("logictest(\"%s\");\n", inbuf);
    char tmp[3][BUFSIZE];
    int tmpp = 0;
    uint8_t t1 = 0;
    uint8_t t2 = 0;
    int p = 0;
    bool inStr = false;
    int pct = 0;
    while (inbuf[p] == ' ') {p++;}
    if (p >= (int)strlen(inbuf)) {cerr = 10; return -1;}
    for (int i = p; true; i++) {
        if (inbuf[i] == '(' && !inStr) {pct++;}
        if (inbuf[i] == ')' && !inStr) {pct--;}
        if (inbuf[i] == '"') {inStr = !inStr;}  
        if (inbuf[i] == 0) {cerr = 1; return -1;}
        if ((inbuf[i] == '<' || inbuf[i] == '=' || inbuf[i] == '>') && !inStr && pct == 0) {p = i; break;}
        if (inbuf[i] == ' ' && !inStr) {} else
        {tmp[0][tmpp] = inbuf[i]; tmpp++;}
    }
    tmp[0][tmpp] = 0;
    tmpp = 0;
    for (int i = p; true; i++) {
        if (tmpp > 2) {cerr = 1; return -1;}
        if (inbuf[i] != '<' && inbuf[i] != '=' && inbuf[i] != '>') {p = i; break;} else
        {tmp[1][tmpp] = inbuf[i]; tmpp++;}
    }
    tmp[1][tmpp] = 0;
    tmpp = 0;
    for (int i = p; true; i++) {
        if (inbuf[i] == 0) {break;}
        if (inbuf[i] == ' ' && !inStr) {} else
        {tmp[2][tmpp] = inbuf[i]; tmpp++;}
    }
    tmp[2][tmpp] = 0;
    if (debug) printf("getting tmp[0]... current: {%s}\n", tmp[0]);
    t1 = getVal(tmp[0], tmp[0]);
    if (debug) printf("DONE: {%s}\n", tmp[0]);
    if (t1 == 0) return -1;
    if (debug) printf("getting tmp[2]... current: {%s}\n", tmp[2]);
    t2 = getVal(tmp[2], tmp[2]);
    if (debug) printf("DONE: {%s}\n", tmp[2]);
    if (t2 == 0) return -1;
    if (t1 != t2) {cerr = 2; return -1;}
    if (!qstrcmp(tmp[1], "=")) {
        return (uint8_t)(bool)!qstrcmp(tmp[0], tmp[2]);
    } else if (!qstrcmp(tmp[1], "<>")) {
        return (uint8_t)(bool)qstrcmp(tmp[0], tmp[2]);
    } else if (!qstrcmp(tmp[1], ">")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 > num2;
    } else if (!qstrcmp(tmp[1], "<")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 < num2;
    } else if (!qstrcmp(tmp[1], ">=")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 >= num2;
    } else if (!qstrcmp(tmp[1], "<=")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 <= num2;
    } else if (!qstrcmp(tmp[1], "=>")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 >= num2;
    } else if (!qstrcmp(tmp[1], "=<")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 <= num2;
    }
    cerr = 1;
    return -1;
}

bool runlogic() {
    char tmp[2][BUFSIZE];
    tmp[0][0] = 0; tmp[1][0] = 0;
    int i = 0;
    while (cmd[i] == ' ') {i++;}
    int j = i;
    while (cmd[j] != ' ' && cmd[j] != 0) {j++;}
    copyStrSnip(cmd, i, j, tmp[0]);
    upCase(tmp[0]);
    cerr = 0;
    #include "logic.c"
    return false;
    lexit:
    return true;
}

void runcmd() {
    if (cmd[0] == 0) return;
    cerr = 0;
    bool lgc = runlogic();
    if (lgc) goto cmderr;
    if (debug) printf("testing logic...\n");
    if (dlstackp > -1) {if (dldcmd[dlstackp] == 1) return;}
    if (itstackp > -1) {if (itdcmd[itstackp] == 1) return;}
    if (debug) printf("passed\n");
    if (debug) printf("making args...\n");
    mkargs();
    argt = realloc(argt, argct + 1);
    arg = realloc(arg, (argct + 1) * sizeof(char*));
    for (int i = 1; i <= argct; i++) {arg[i] = malloc(0);}
    if (debug) printf("running command...\n");
    solvearg(0);
    upCase(arg[0]);
    if (debug) printf("C [%d]: {%s}\n", 0, arg[0]);
    cerr = 255;
    #include "commands.c"
    if (debug) printf("cerr: [%d]\n", cerr);
    cmderr:
    if (cerr) {
        getCurPos();
        if (curx != 1) printf("\n");
        if (inProg) {printf("Error %d on line %d of %s: '%s': ", cerr, progLine, progFilename, cmd);}
        else {printf("Error %d: ", cerr);}
        switch (cerr) {
            default:
                fputs("\b\b \b", stdout);
                break;
            case 1:
                fputs("Syntax", stdout);
                break;
            case 2:
                fputs("Type mismatch", stdout);
                break;
            case 3:
                fputs("Agument count mismatch", stdout);
                break;
            case 4:
                printf("Invalid variable name: '%s'", errstr);
                break;
            case 5:
                fputs("Operation results in undefined", stdout);
                break;
            case 6:
                fputs("LOOP without DO", stdout);
                break;
            case 7:
                fputs("ENDIF without IF", stdout);
                break;
            case 8:
                fputs("ELSE without IF", stdout);
                break;
            case 9:
                fputs("NEXT without FOR", stdout);
                break;
            case 10:
                fputs("Expected expression", stdout);
                break;
            case 11:
                fputs("Unexpected ELSE", stdout);
                break;
            case 12:
                fputs("Reached DO limit", stdout);
                break;
            case 13:
                fputs("Reached IF limit", stdout);
                break;
            case 14:
                fputs("Reached FOR limit", stdout);
                break;
            case 15:
                printf("File not found: '%s'", errstr);
                break;
            case 16:
                fputs("Invalid data or data range exceded", stdout);
                break;
            case 127:
                printf("Not a function: '%s'", errstr);
                break;
            case 253:
                printf("Command not valid in command-line: '%s'", arg[0]);
                break;
            case 254:
                printf("Command not valid in program: '%s'", arg[0]);
                break;
            case 255:
                printf("Not a command: '%s'", arg[0]);
                break;
        }
        putc('\n', stdout);
        cp = -1;
        chkinProg = inProg = false;
    }
    if (lgc) return;
    if (debug) printf("freeing stuff...\n");
    for (int i = 0; i <= argct; i++) {
        free(tmpargs[i]);
    }
    for (int i = 1; i <= argct; i++) {
        free(arg[i]);
    }
    //free(tmpargs);
    //free(argl);
    //free(argt);
    //free(arg);
    return;
}
