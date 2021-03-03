#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/time.h>
#include <editline.h>

char VER[] = "0.8.4";

FILE *prog;
FILE *f[256];

int      err = 0;
int         cerr;
char cerrstr[32];

char            *mem;
uint32_t memsize = 0;

int      *varlen;
char    **varstr;
char   **varname;
bool   *varinuse;
uint8_t *vartype;
int     varmaxct;

char      *cmd;
int       cmdl;
char **tmpargs;
char     **arg;
uint8_t  *argt;
int      *argl;
int  argct = 0;

char *fstr;

char prompt[32768];

uint8_t fgc = 15;
uint8_t bgc = 0;

bool debug = false;

void forceExit() {
    //printf("\n");
    exit(0);
}

void cleanExit() {
    signal(SIGINT, forceExit);
    signal(SIGKILL, forceExit);
    signal(SIGTERM, forceExit);
    printf("\e[0m\r\n");
    exit(err);
}

int runcmd();
int copyStr(char* str1, char* str2);
void copyStrSnip(char* str1, int i, int j, char* str2);
uint8_t getVal(char* inbuf, char* outbuf);
int read_history (const char *filename);
int write_history (const char *filename);

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanExit); 
    signal(SIGKILL, cleanExit); 
    signal(SIGTERM, cleanExit); 
    bool exit = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")) {
            printf("Command Line Interface BASIC version %s\n", VER);
            printf("Copyright (C) 2021 PQCraft\n");
            exit = true;
        }
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            printf("Usage: clibasic [options]\n");
            printf("\n");
            printf("--help      Shows the help screen.\n");
            printf("--version   Shows the version info.\n");
            exit = true;
        }
        if (!strcmp(argv[i], "--debug") || !strcmp(argv[i], "-d")) {
            debug = true;
        }
    }
    if (exit) cleanExit();
    printf("\e[0m");
    printf("\e[38;5;%um", fgc);
    printf("\e[48;5;%um", bgc);
    printf("Command Line Interface BASIC version %s\n", VER);
    if (debug) printf("Running in debug mode\n");
    char conbuf[32768];
    char pstr[32768];
    strcpy(prompt, "\"CLIBASIC> \"");
    fstr = malloc(0);
    cmd = malloc(0);
    srand(time(0));
    while (!exit) {
        for (int i = 0; i < 32768; i++) conbuf[i] = 0;
        int cp = 0;
        char *tmpstr = NULL;
        putc('\r', stdout);
        int tmpt = getVal(prompt, pstr);
        if (tmpt != 1) strcpy(pstr, "CLIBASIC> ");
        while (tmpstr == NULL) {tmpstr = readline(pstr);}
        if (debug) printf("check for null:\n");
        if (tmpstr[0] == '\0') {free(tmpstr); goto brkproccmd;}
        add_history(tmpstr);
        copyStr(tmpstr, conbuf);
        free(tmpstr);
        cp = 0;
        bool inStr = false;
        if (debug) printf("conbuf: {%s}\n", conbuf);
        while (true) {
            if (debug) printf("cmdpoc(1): cp:[%d], cmdl:[%d], char:[%c](%d)\n", cp, cmdl, conbuf[cp], (int)conbuf[cp]);
            if (conbuf[cp] == '"') {inStr = !inStr; cmdl++;} else
            if ((conbuf[cp] == ':' && !inStr) || conbuf[cp] == '\0') {
                while(conbuf[cp - cmdl] == ' ') {cmdl--;}
                cmd = realloc(cmd, (cmdl + 1) * sizeof(char));
                copyStrSnip(conbuf, cp - cmdl, cp, cmd);
                if (debug) printf("cmd: {%s} [%d] [%d]\n", cmd, cp - cmdl, cp);
                runcmd();
                cmdl = 0;
                if (conbuf[cp] == '\0') goto brkproccmd;
            } else
            {cmdl++;}
            cp++;
            if (debug) printf("cmdpoc(2): cp:[%d], cmdl:[%d], char:[%c](%d)\n", cp, cmdl, conbuf[cp], (int)conbuf[cp]);
        }
        brkproccmd:
        if (0) {}
    }
    cleanExit();
    return 0;
}

double randNum(double num1, double num2) 
{
    double range = num2 - num1;
    double div = RAND_MAX / range;
    return num1 + (rand() / div);
}

bool isSpChar(char* bfr, int pos) {
    return (bfr[pos] == '+' || bfr[pos] == '-' || bfr[pos] == '*' || bfr[pos] == '/' || bfr[pos] == '^');
}

bool isValidVarChar(char* bfr, int pos) {
    return ((bfr[pos] >= 'A' && bfr[pos] <= 'Z') || (bfr[pos] >= 'a' && bfr[pos] <= 'z') || bfr[pos] == '_' || bfr[pos] == '$' || bfr[pos] == '%' || bfr[pos] == '&' || bfr[pos] == '!' || bfr[pos] == '~');
}

int copyStr(char* str1, char* str2) {
    int i;
    for (i = 0; str1[i] != '\0'; i++) {str2[i] = str1[i];}
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
    for (i3 = i; str1[i] != '\0'; i++) {str2[i3] = str1[i2]; i2++; i3++;}
    str2[i3] = 0;
}

void copyStrApnd(char* str1, char* str2) {
    int j = 0, i = 0;
    for (i = strlen(str2); str1[j] != '\0'; i++) {str2[i] = str1[j]; j++;}
    str2[i] = 0;
}

void getStr(char* str1, char* str2) {
    char buf[32768];
    int j = 0, i;
    for (i = 0; str1[i] != '\0'; i++) {
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
                    if (debug) printf("getStr: hexstr: {%s}, c: [%d]\n", h, tc);
                    c = (char)tc;
                    break;
                case '\\': c = '\\'; break;
                default: i--; break;
            }
        }
        if (debug) printf("getStr: c: [%d], j: [%d]\n", (int)c, j);
        buf[j] = c;
        j++;
    }
    buf[j] = 0;
    copyStr(buf, str2);
}

uint8_t getType(char* str) {
    if (str[0] == '"') {if (str[strlen(str) - 1] != '"') {return 0;} return 1/*STR*/;}
    bool p = false;
    for (int i = 0; str[i] != '\0'; i++) {
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
    if (debug) printf("getFunc: inbuf: {%s}\n", inbuf);
    char tmp[2][32768];
    char **farg;
    uint8_t *fargt;
    int *flen;
    int fargct;
    int ftype = 0;
    int i;
    for (i = 0; inbuf[i] != '('; i++) {if (inbuf[i] >= 'a' && inbuf[i] <= 'z') inbuf[i] = inbuf[i] - 32;}
    if (debug) printf("getFunc: i: [%d]\n", i);
    copyStrSnip(inbuf, i + 1, strlen(inbuf) - 1, tmp[0]);
    fargct = getArgCt(tmp[0]);
    if (debug) printf("getFunc: malloc farg:\n");
    farg = malloc((fargct + 1) * sizeof(char*));
    if (debug) printf("getFunc: malloc flen:\n");
    flen = malloc((fargct + 1) * sizeof(int));
    if (debug) printf("getFunc: malloc fargt:\n");
    fargt = malloc((fargct + 1) * sizeof(uint8_t));
    if (debug) printf("getFunc: fargct: [%d]\n", fargct);
    for (int j = 0; j <= fargct; j++) {
        if (debug) printf("getFunc: tmp[0]: {%s}\n", tmp[0]);
        if (debug) printf("getFunc: tmp[1]: {%s}\n", tmp[1]);
        if (j == 0) {
            flen[0] = i;
            farg[0] = (char *)malloc((flen[0] + 1) * sizeof(char));
            fstr = realloc(fstr, (flen[0] + 1) * sizeof(char));
            copyStrSnip(inbuf, 0, i, farg[0]);
            copyStrSnip(inbuf, 0, i, fstr);
        } else {
            flen[j] = getArg(j - 1, tmp[0], tmp[1]);
            if (debug) printf("getFunc: tmp[0]: {%s}\n", tmp[0]);
            if (debug) printf("getFunc: tmp[1]: {%s}\n", tmp[1]);
            farg[j] = (char *)malloc((flen[j] + 1) * sizeof(char));
            fargt[j] = getVal(tmp[1], farg[j]);
            if (fargt[j] == 0) goto fexit;
            if (fargt[j] == 255) fargt[j] = 0;
        }
        if (debug) printf("getFunc: inbuf: {%s}\n", inbuf);
        if (debug) printf("getFunc: flen[%d]: [%d]\n", j, flen[j]);
        if (debug) printf("getFunc: farg[%d]: {%s}\n", j, farg[j]);
    }
    if (debug) printf("getFunc: fargct: [%d]\n", fargct);
    outbuf[0] = 0;
    cerr = 127;
    #include "functions.c"
    fexit:
    for (int j = 0; j <= fargct; j++) {
        free(farg[j]);
    }
    free(farg);
    free(flen);
    free(fargt);
    if (debug) printf("getFunc: outbuf: {%s}\n", outbuf);
    if (debug) printf("getFunc: fstr: {%s}\n", fstr);
    if (cerr != 0) return 0;
    return ftype;
}

uint8_t getVar(char* vn, char* varout) {
    if (debug) printf("getVar: vn: {%s}\n", vn);
    if (vn[0] == '\0') return 0;
    if (vn[strlen(vn) - 1] == ')') {
        return getFunc(vn, varout);
    }
    for (int i = 0; vn[i] != '\0'; i++) {if (!isValidVarChar(vn, i)) {cerr = 4; return 0;} if (vn[i] >= 'a' && vn[i] <= 'z') vn[i] = vn[i] - 32;}
    int v = -1;
    if (debug) printf("getVar: v: [%d]\n", v);
    for (int i = 0; i < varmaxct; i++) {
        if (debug) printf("getVar: i: [%d]\n", i);
        if (varinuse[i] && !strcmp(vn, varname[i])) {v = i; break;}
    }
    if (debug) printf("getVar: v: [%d]\n", v);
    if (v == -1) {
        if (vn[strlen(vn) - 1] == '$') {varout[0] = 0; return 1;}
        else {varout[0] = '0'; varout[1] = 0; return 2;}
    } else {
        if (debug) printf("getVar: varstr[%d]: {%s}\n", v, varstr[v]);
        copyStr(varstr[v], varout);
        return vartype[v];
    }
    return 0;
}

void setVar(char* vn, char* val, uint8_t t) {
    for (int i = 0; vn[i] != '\0'; i++) {if (vn[i] >= 'a' && vn[i] <= 'z') vn[i] = vn[i] - 32;}
    if (debug) printf("setVar: vn: {%s}\n", vn);
    if (debug) printf("setVar: val: {%s}\n", val);
    int v = -1;
    for (int i = 0; i < varmaxct; i++) {
        if (!strcmp(vn, varname[i])) {v = i; break;}
    }
    if (v == -1) {
        v = varmaxct;
        if (debug) printf("setVar: realloc varstr\n");
        varstr = realloc(varstr, (v + 1) * sizeof(char*));
        if (debug) printf("setVar: realloc varname\n");
        varname = realloc(varname, (v + 1) * sizeof(char*));
        if (debug) printf("setVar: realloc varlen\n");
        varlen = (int*)realloc(varlen, (v + 1) * sizeof(int));
        if (debug) printf("setVar: realloc varinuse\n");
        varinuse = (bool*)realloc(varinuse, (v + 1) * sizeof(bool));
        varinuse[v] = true;
        if (debug) printf("setVar: realloc vartype\n");
        vartype = (uint8_t*)realloc(vartype, (v + 1) * sizeof(uint8_t));
        varlen[v] = strlen(val);
        if (debug) printf("setVar: realloc varstr[v]\n");
        varstr[v] = malloc(0);
        varstr[v] = (char *)realloc(varstr[v], (varlen[v] + 1) * sizeof(char));
        if (debug) printf("setVar: realloc varname[v]\n");
        varname[v] = malloc(0);
        varname[v] = (char *)realloc(varname[v], (strlen(vn) + 1) * sizeof(char));
        copyStr(vn, varname[v]);
        vartype[v] = t;
        varmaxct++;
    }
    if (debug) printf("setVar: val: {%s}\n", val);
    copyStr(val, varstr[v]);
    if (debug) printf("setVar: varstr[v]: {%s}\n", varstr[v]);
    return;
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
    if (tmpinbuf[0] == '\0') {return 255;}
    char inbuf[32768];
    copyStr(tmpinbuf, inbuf);
    if (debug) printf("getVal: inbuf: {%s}\n", inbuf);
    int ip = 0, jp = 0;
    char tmp[4][32768];
    uint8_t t = 0;
    uint8_t dt = 0;
    bool inStr = false;
    double num1 = 0;
    double num2 = 0;
    double num3 = 0;
    int numAct;
    if (debug) printf("checking for syntax error\n");
    if ((isSpChar(inbuf, 0) && inbuf[0] != '-') || isSpChar(inbuf, strlen(inbuf) - 1)) {cerr = 1; return 0;}
    if (debug) printf("no syntax error detected\n");
    int tmpct = 0;
    tmp[0][0] = 0; tmp[1][0] = 0; tmp[2][0] = 0; tmp[3][0] = '"'; tmp[3][1] = 0;
    for (int i = 0; inbuf[i] != '\0'; i++) {
        if (inbuf[i] == '(') {if (tmpct == 0) {ip = i;} tmpct++;}
        if (inbuf[i] == ')') {
            tmpct--;
            if (tmpct == 0 && (ip == 0 || isSpChar(inbuf, ip - 1))) {
                jp = i;
                copyStrSnip(inbuf, ip + 1, jp, tmp[0]);
                t = getVal(tmp[0], tmp[0]);
                if (t == 0) return 0;
                if (dt == 0) dt = t;
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
    if (debug) printf("getVal: inbuf: {%s}\n", inbuf);
    ip = 0; jp = 0;
    tmp[0][0] = 0; tmp[1][0] = 0; tmp[2][0] = 0; tmp[3][0] = 0;
    while (true) {
        if (debug) printf("getVal (1): ip: [%d], jp: [%d]\n", ip, jp);
        bool seenStr = false;
        while (true) {
            if (debug) printf("getVal (2): ip: [%d], jp: [%d]\n", ip, jp);
            if (inbuf[jp] == '"') {inStr = !inStr; if (seenStr && inStr) {cerr = 1; return 0;} seenStr = true;}
            if ((isSpChar(inbuf, jp) && !inStr) || inbuf[jp] == '\0') goto gvbrk;
            jp++;
        }
        gvbrk:
        copyStrSnip(inbuf, ip, jp, tmp[0]);
        if (debug) printf("getVal (3): tmp[0]: {%s}\n", tmp[0]);
        t = getType(tmp[0]);
        if (debug) printf("getType: tmp[0]: [%u]\n", t);
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
        if (debug) printf("getType: tmp[0]: [%u]\n", t);
        if (debug) printf("getVal (4): tmp[0]: {%s}\n", tmp[0]);
        if (t != 0 && dt == 0) {dt = t;} else
        if ((t != 0 && t != dt)) {cerr = 2; return 0;} else
        if (t == 0) {cerr = 1; return false;}
        if ((dt == 1 && inbuf[jp] != '+') && inbuf[jp] != '\0') {cerr = 1; return 0;}
        if (debug) printf("getVal (5): tmp[1]: {%s}\n", tmp[1]);
        if (t == 1) {copyStrSnip(tmp[0], 1, strlen(tmp[0]) - 1, tmp[2]); copyStrApnd(tmp[2], tmp[1]);} else
        if (t == 2) {
            copyStr(inbuf, tmp[0]);
            int p1, p2, p3;
            bool inStr = false;
            while (true) {
                numAct = 0;
                p1 = 0, p2 = 0, p3 = 0;
                if (debug) printf("checking for exp\n");
                for (int i = 0; tmp[0][i] != '\0' && p2 == 0; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (tmp[0][i] == '^' && !inStr) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 4;}
                }
                if (debug) printf("checking for mlt/dvd\n");
                for (int i = 0; tmp[0][i] != '\0' && p2 == 0; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (tmp[0][i] == '*' && !inStr) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 2;}
                    if (tmp[0][i] == '/' && !inStr) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 3;}
                }
                if (debug) printf("checking for add/sub\n");
                for (int i = 0; tmp[0][i] != '\0' && p2 == 0; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (tmp[0][i] == '+' && !inStr) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 0;}
                    if (tmp[0][i] == '-' && !inStr) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 1;}
                }
                inStr = false;
                if (debug) printf("getVal: p1: [%d], p2: [%d], p3: [%d]\n", p1, p2, p3);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                if (p2 == 0) {
                    if (p3 == 0) {
                        t = getType(tmp[0]);
                        if (t == 0) {cerr = 1; return 0;} else
                        if (t == 255) {t = getVar(tmp[0], tmp[0]);
                        if (t == 0) {return 0;}
                        if (t != 2) {cerr = 2; return 0;}}
                    }
                    if (debug) printf("no operations found\n");
                    copyStr(tmp[0], tmp[1]); goto gvfexit;
                }
                tmp[1][0] = 0; tmp[2][0] = 0; tmp[3][0] = 0;
                for (int i = p2 - 1; i > 0; i--) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (isSpChar(tmp[0], i) && !inStr) {p1 = i; break;}
                }
                for (int i = p2 + 1; true; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if ((isSpChar(tmp[0], i) && i != p2 + 1 && !inStr) || tmp[0][i] == '\0') {p3 = i; break;}
                }
                copyStrSnip(tmp[0], p1, p2, tmp[2]);
                t = getType(tmp[2]);
                if (t == 0) {cerr = 1; return 0;} else
                if (t == 255) {t = getVar(tmp[2], tmp[2]); if (t == 0) {return 0;} if (t != 2) {cerr = 2; return 0;}}
                copyStrSnip(tmp[0], p2 + 1, p3, tmp[3]);
                t = getType(tmp[3]);
                if (t == 0) {cerr = 1; return 0;} else
                if (t == 255) {t = getVar(tmp[3], tmp[3]); if (t == 0) {return 0;} if (t != 2) {cerr = 2; return 0;}}
                if (debug) printf("getVal: p1: [%d], p2: [%d], p3: [%d]\n", p1, p2, p3);
                sscanf(tmp[2], "%lf", &num1);
                sscanf(tmp[3], "%lf", &num2);
                if (debug) printf("getVal: num1: [%lf], num2: [%lf]\n", num1, num2);
                if (debug) printf("getVal: numAct: [%d]\n", numAct);
                switch (numAct) {
                    case 0: num3 = num1 + num2; break;
                    case 1: num3 = num1 - num2; break;
                    case 2: if (num2 == 0) {cerr = 5; return 0;} num3 = num1 * num2; break;
                    case 3: num3 = num1 / num2; break;
                    case 4: num3 = pow(num1, num2); break;
                }
                tmp[1][0] = 0;
                if (debug) printf("getVal: num3: [%lf]\n", num3);                
                sprintf(tmp[2], "%lf", num3);
                while (tmp[2][strlen(tmp[2]) - 1] == '0') {tmp[2][strlen(tmp[2]) - 1] = 0;}
                if (tmp[2][strlen(tmp[2]) - 1] == '.') {tmp[2][strlen(tmp[2]) - 1] = 0;}
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                copyStrSnip(tmp[0], p3, strlen(tmp[0]), tmp[3]);
                if (p1 != 0) copyStrSnip(tmp[0], 0, p1, tmp[1]);
                copyStrApnd(tmp[2], tmp[1]);
                copyStrApnd(tmp[3], tmp[1]);
                copyStr(tmp[1], tmp[0]);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
            }
        }
        if (debug) printf("getVal (6): tmp[1]: {%s}\n", tmp[1]);
        // 
        if (inbuf[jp] == '\0') {break;}
        jp++;
        ip = jp;
    }
    gvfexit:
    //if (dt == 0) {dt = 2; tmp[1][0] = '0'; tmp[1][1] = '\0';}
    copyStr(tmp[1], outbuf);
    if (outbuf[0] == '\0' && dt != 1) {outbuf[0] = '0'; outbuf[1] = '\0'; return 2;}
    return dt;
}

bool solveargs() {
    if (debug) printf("solveargs: argct: %d\n", argct);
    argt = malloc(argct + 1);
    arg = malloc((argct + 1) * sizeof(char*));
    char tmpbuf[32768];
    argt[0] = 0;
    arg[0] = tmpargs[0];
    for (int i = 1; i <= argct; i++) {
        if (debug) printf("$ [%d]: %d\n", i, argl[i]);
        argt[i] = 0; // 0 = Unset (for error detection), 1 = string, 2 = number
        for (int p = 0; p < 32767; p++) {tmpbuf[p] = 0;}
        argt[i] = getVal(tmpargs[i], tmpbuf);
        if (debug) printf("solveargs: argt[%d]: %d\n", i, argt[i]);
        if (debug) printf("solveargs: argl[%d]: %d\n", i, argl[i]);
        arg[i] = malloc((argl[i] + 1) * sizeof(char));
        if (argt[i] == 0) return false;
        if (argt[i] == 255) {argt[i] = 0;}
        copyStr(tmpbuf, arg[i]);
        argl[i] = strlen(arg[i]);
    }
    return true;
}

int getArgCt(char* inbuf) {
    int ct = 0;
    bool inStr = false;
    int pct = 0;
    int j = 0;
    while (inbuf[j] == ' ') {j++;}
    for (int i = j; inbuf[i] != '\0'; i++) {
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
    if (debug) printf("getArg: inbuf: {%s}\n", inbuf);
    for (int i = 0; inbuf[i] != '\0' && ct <= num; i++) {
        if (inbuf[i] == '(' && !inStr) {pct++;}
        if (inbuf[i] == ')' && !inStr) {pct--;}
        if (inbuf[i] == '"') {inStr = !inStr;}        
        if (inbuf[i] == ' ' && !inStr) {} else
        if (inbuf[i] == ',' && !inStr && pct == 0) {ct++;} else
        if (ct == num) {outbuf[len] = inbuf[i]; len++;}
    }
    outbuf[len] = 0;
    if (debug) printf("getArg [%d]: outbuf: {%s}\n", num, outbuf);
    if (pct || inStr) return -1;
    return len;
}

bool mkargs() {
    int tmplen = 0;
    char tmpbuf[2][32768];
    int j = 0;
    while (cmd[j] == ' ') {j++;}
    int h = j;
    while (cmd[h] != ' ' && cmd[h] != '\0') {h++;}
    copyStrSnip(cmd, h + 1, strlen(cmd), tmpbuf[0]);
    argct = getArgCt(tmpbuf[0]);
    if (debug) printf("mkargs: argct: [%d]\n", argct);
    tmpargs = malloc((argct + 1) * sizeof(char*));
    argl = malloc((argct + 1) * sizeof(int));
    for (int i = 0; i <= argct; i++) {
        argl[i] = 0;
        if (debug) printf("realloc size [%d]: %d\n", i, argl[i] + 1);
        if (i == 0) {
            copyStrSnip(cmd, j, h, tmpbuf[0]);
            argl[i] = strlen(tmpbuf[0]);
            tmpargs[i] = malloc((argl[i] + 1) * sizeof(char));
            copyStr(tmpbuf[0], tmpargs[i]);            
            copyStrSnip(cmd, h + 1, strlen(cmd), tmpbuf[0]);
        } else {
            tmplen = getArg(i - 1, tmpbuf[0], tmpbuf[1]);
            if (tmplen == -1) {cerr = 1; return false;}
            argl[i] = tmplen;
            tmpargs[i] = malloc((argl[i] + 1) * sizeof(char));
            copyStr(tmpbuf[1], tmpargs[i]);
        }
        tmpargs[i][argl[i]] = '\0';
    }
    if (argct == 1 && tmpargs[1][0] == '\0') {argct = 0;}
    for (int i = 0; i <= argct; i++) {tmpargs[i][argl[i]] = '\0';}
    if (debug) printf("solve args:\n");
    return solveargs();
}

int runcmd() {
    if (debug) printf("%s\n", cmd);
    if (debug) printf("check for null:\n");
    if (cmd[0] == '\0') return 0;
    if (debug) printf("mkargs:\n");
    if (!mkargs()) goto cmderr;
    if (debug) printf("run commands:\n");
    if (debug) printf("C [%d]: {%s}\n", 0, arg[0]);
    for (int i = 1; i <= argct; i++) {
        if (debug) printf("A [%d]: {%s}\n", i, arg[i]);
    }
    for (int i = 0; i < argl[0]; i++) {
        if (arg[0][i] >= 'a' && arg[0][i] <= 'z') {
            arg[0][i] = arg[0][i] - 32;
        }
    }
    if (debug) printf("argct: %d\n", argct);
    cerr = 255;
    if (debug) printf("cerr: %d\n", cerr);
    // COMMANDS START
    #include "commands.c"
    // COMMANDS END
    cmderr:
    /*if (inProg) {printf("Line %u: ");}*/
    if (cerr != 0) {
        printf("ERROR %d: ", cerr);
        switch (cerr) {
            default:
                printf("\b\b  \b\b");
                break;
            case 1:
                printf("Syntax");
                break;
            case 2:
                printf("Type Mismatch");
                break;
            case 3:
                printf("Agument Count Mismatch");
                break;
            case 4:
                printf("Invalid Variable Name: %s", cerrstr);
                break;
            case 5:
                printf("Divide by zero");
                break;
            case 127:
                printf("Not a Function: %s", fstr);
                break;
            case 255:
                printf("Not a Command: %s", arg[0]);
                break;
        }
        putc('\n', stdout);
    }
    for (int i = 0; i <= argct; i++) {
        free(tmpargs[i]);
    }
    for (int i = 1; i <= argct; i++) {
        free(arg[i]);
    }
    free(tmpargs);    
    free(argl);    
    free(argt);    
    free(arg);    
    return cerr;
}
