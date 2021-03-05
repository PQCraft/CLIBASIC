#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/time.h>
#include <editline.h>

char VER[] = "0.10.2";

FILE *prog;
FILE *f[256];

int      err = 0;
int         cerr;

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

int cmdpos;

int dlstack[256];
bool dldcmd[256];
int dlstackp = -1;
bool didloop = false;

bool itdcmd[256];
int itstackp = -1;
bool didelse = false;
/*
int fnstack[256];
bool fndcmd[256];
int fnstackp = -1;
*/
char *errstr;

char conbuf[32768];
char prompt[32768];
char pstr[32768];

uint8_t fgc = 15;
uint8_t bgc = 0;

int cp = 0;

bool cmdint = false;

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

void cmdIntHndl() {
    signal(SIGINT, cleanExit);
    cmdint = true;
}

void runcmd();
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
    printf("\e[0m\e[38;5;%um\e[48;5;%um", fgc, bgc);
    printf("Command Line Interface BASIC version %s\n", VER);
    if (debug) printf("Running in debug mode\n");
    strcpy(prompt, "\"CLIBASIC> \"");
    errstr = malloc(0);
    cmd = malloc(0);
    srand(time(0));
    while (!exit) {
        for (int i = 0; i < 32768; i++) conbuf[i] = 0;
        char *tmpstr = NULL;
        putc('\r', stdout);
        int tmpt = getVal(prompt, pstr);
        if (tmpt != 1) strcpy(pstr, "CLIBASIC> ");
        printf("\e[38;5;%um\e[48;5;%um", fgc, bgc);
        while (tmpstr == NULL) {tmpstr = readline(pstr);}
        if (tmpstr[0] == '\0') {free(tmpstr); goto brkproccmd;}
        add_history(tmpstr);
        copyStr(tmpstr, conbuf);
        free(tmpstr);
        cp = 0;
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
        signal(SIGINT, cmdIntHndl);
        while (true) {
            if (cmdint) {cmdint = false; goto brkproccmd;}
            if (conbuf[cp] == '"') {inStr = !inStr; cmdl++;} else
            if ((conbuf[cp] == ':' && !inStr) || conbuf[cp] == '\0') {
                while (conbuf[cp - cmdl] == ' ') {cmdl--;}
                cmd = realloc(cmd, (cmdl + 1) * sizeof(char));
                cmdpos = cp - cmdl;
                copyStrSnip(conbuf, cp - cmdl, cp, cmd);
                cmdl = 0;
                runcmd();
                if (conbuf[cp] == '\0') goto brkproccmd;
            } else
            {cmdl++;}
            if (!didloop) {cp++;} else {didloop = false;}
        }
        brkproccmd:
        signal(SIGINT, cleanExit);
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
    char tmp[2][32768];
    char **farg;
    uint8_t *fargt;
    int *flen;
    int fargct;
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
            farg[0] = (char *)malloc((flen[0] + 1) * sizeof(char));
            errstr = realloc(errstr, (flen[0] + 1) * sizeof(char));
            copyStrSnip(inbuf, 0, i, farg[0]);
            copyStrSnip(inbuf, 0, i, errstr);
        } else {
            flen[j] = getArg(j - 1, tmp[0], tmp[1]);
            farg[j] = (char *)malloc((flen[j] + 1) * sizeof(char));
            fargt[j] = getVal(tmp[1], farg[j]);
            if (fargt[j] == 0) goto fexit;
            if (fargt[j] == 255) fargt[j] = 0;
        }
    }
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
    if (cerr != 0) return 0;
    return ftype;
}

uint8_t getVar(char* vn, char* varout) {
    if (vn[0] == '\0') return 0;
    if (vn[strlen(vn) - 1] == ')') {
        return getFunc(vn, varout);
    }
    for (int i = 0; vn[i] != '\0'; i++) {
        if (!isValidVarChar(vn, i)) {
            cerr = 4;
            errstr = realloc(errstr, (strlen(vn) + 1) * sizeof(char));
            copyStr(vn, errstr);
            return 0;
        }
        if (vn[i] >= 'a' && vn[i] <= 'z') vn[i] -= 32;
    }
    int v = -1;
    for (int i = 0; i < varmaxct; i++) {
        if (varinuse[i] && !strcmp(vn, varname[i])) {v = i; break;}
    }
    if (v == -1) {
        if (vn[strlen(vn) - 1] == '$') {varout[0] = 0; return 1;}
        else {varout[0] = '0'; varout[1] = 0; return 2;}
    } else {
        copyStr(varstr[v], varout);
        return vartype[v];
    }
    return 0;
}

void setVar(char* vn, char* val, uint8_t t) {
    for (int i = 0; vn[i] != '\0'; i++) {if (vn[i] >= 'a' && vn[i] <= 'z') vn[i] = vn[i] - 32;}
    int v = -1;
    for (int i = 0; i < varmaxct; i++) {
        if (!strcmp(vn, varname[i])) {v = i; break;}
    }
    if (v == -1) {
        v = varmaxct;
        varstr = realloc(varstr, (v + 1) * sizeof(char*));
        varname = realloc(varname, (v + 1) * sizeof(char*));
        varlen = (int*)realloc(varlen, (v + 1) * sizeof(int));
        varinuse = (bool*)realloc(varinuse, (v + 1) * sizeof(bool));
        varinuse[v] = true;
        vartype = (uint8_t*)realloc(vartype, (v + 1) * sizeof(uint8_t));
        varlen[v] = strlen(val);
        varstr[v] = malloc(0);
        varstr[v] = (char *)realloc(varstr[v], (varlen[v] + 1) * sizeof(char));
        varname[v] = malloc(0);
        varname[v] = (char *)realloc(varname[v], (strlen(vn) + 1) * sizeof(char));
        copyStr(vn, varname[v]);
        vartype[v] = t;
        varmaxct++;
    }
    copyStr(val, varstr[v]);
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
    int ip = 0, jp = 0;
    char tmp[4][32768];
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
    ip = 0; jp = 0;
    tmp[0][0] = 0; tmp[1][0] = 0; tmp[2][0] = 0; tmp[3][0] = 0;
    while (true) {
        bool seenStr = false;
        int pct = 0;
        while (true) {
            if (inbuf[jp] == '"') {inStr = !inStr; if (seenStr && inStr) {cerr = 1; return 0;} seenStr = true;}
            if (inbuf[jp] == '(') {pct++;}
            if (inbuf[jp] == ')') {pct--;}
            if ((isSpChar(inbuf, jp) && !inStr && pct == 0) || inbuf[jp] == '\0') goto gvbrk;
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
        if ((dt == 1 && inbuf[jp] != '+') && inbuf[jp] != '\0') {cerr = 1; return 0;}
        if (t == 1) {copyStrSnip(tmp[0], 1, strlen(tmp[0]) - 1, tmp[2]); copyStrApnd(tmp[2], tmp[1]);} else
        if (t == 2) {
            copyStr(inbuf, tmp[0]);
            int p1, p2, p3;
            bool inStr = false;
            pct = 0;
            while (true) {
                numAct = 0;
                p1 = 0, p2 = 0, p3 = 0;
                for (int i = 0; tmp[0][i] != '\0' && p2 == 0; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (tmp[0][i] == '(') {pct++;}
                    if (tmp[0][i] == ')') {pct--;}
                  if (tmp[0][i] == '^' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 4;}
                }
                for (int i = 0; tmp[0][i] != '\0' && p2 == 0; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (tmp[0][i] == '(') {pct++;}
                    if (tmp[0][i] == ')') {pct--;}
                    if (tmp[0][i] == '*' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 2;}
                    if (tmp[0][i] == '/' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 3;}
                }
                for (int i = 0; tmp[0][i] != '\0' && p2 == 0; i++) {
                    if (tmp[0][i] == '"') inStr = !inStr;
                    if (tmp[0][i] == '(') {pct++;}
                    if (tmp[0][i] == ')') {pct--;}
                    if (tmp[0][i] == '+' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 0;}
                    if (tmp[0][i] == '-' && !inStr && pct == 0) {if (!gvchkchar(tmp[0], i)) {return 0;} p2 = i; numAct = 1;}
                }
                inStr = false;
                if (p2 == 0) {
                    if (p3 == 0) {
                        t = getType(tmp[0]);
                        if (t == 0) {cerr = 1; return 0;} else
                        if (t == 255) {t = getVar(tmp[0], tmp[0]);
                        if (t == 0) {return 0;}
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
                sscanf(tmp[2], "%lf", &num1);
                sscanf(tmp[3], "%lf", &num2);
                switch (numAct) {
                    case 0: num3 = num1 + num2; break;
                    case 1: num3 = num1 - num2; break;
                    case 2: num3 = num1 * num2; break;
                    case 3: if (num2 == 0) {cerr = 5; return 0;} num3 = num1 / num2; break;
                    case 4: num3 = pow(num1, num2); break;
                }
                tmp[1][0] = 0;
                sprintf(tmp[2], "%lf", num3);
                while (tmp[2][strlen(tmp[2]) - 1] == '0') {tmp[2][strlen(tmp[2]) - 1] = 0;}
                if (tmp[2][strlen(tmp[2]) - 1] == '.') {tmp[2][strlen(tmp[2]) - 1] = 0;}
                copyStrSnip(tmp[0], p3, strlen(tmp[0]), tmp[3]);
                if (p1 != 0) copyStrSnip(tmp[0], 0, p1, tmp[1]);
                copyStrApnd(tmp[2], tmp[1]);
                copyStrApnd(tmp[3], tmp[1]);
                copyStr(tmp[1], tmp[0]);
            }
        }
        if (inbuf[jp] == '\0') {break;}
        jp++;
        ip = jp;
    }
    gvfexit:
    copyStr(tmp[1], outbuf);
    if (outbuf[0] == '\0' && dt != 1) {outbuf[0] = '0'; outbuf[1] = '\0'; return 2;}
    return dt;
}

bool solveargs() {
    argt = malloc(argct + 1);
    arg = malloc((argct + 1) * sizeof(char*));
    char tmpbuf[32768];
    argt[0] = 0;
    arg[0] = tmpargs[0];
    for (int i = 1; i <= argct; i++) {arg[i] = malloc(1);}
    for (int i = 1; i <= argct; i++) {
        argt[i] = 0;
        for (int p = 0; p < 32767; p++) {tmpbuf[p] = 0;}
        argt[i] = getVal(tmpargs[i], tmpbuf);
        free(arg[i]);
        arg[i] = malloc((argl[i] + 1) * sizeof(char));
        copyStr(tmpbuf, arg[i]);
        argl[i] = strlen(arg[i]);
        if (argt[i] == 0) return false;
        if (argt[i] == 255) {argt[i] = 0;}
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
    for (int i = 0; inbuf[i] != '\0' && ct <= num; i++) {
        if (inbuf[i] == '(' && !inStr) {pct++;}
        if (inbuf[i] == ')' && !inStr) {pct--;}
        if (inbuf[i] == '"') {inStr = !inStr;}        
        if (inbuf[i] == ' ' && !inStr) {} else
        if (inbuf[i] == ',' && !inStr && pct == 0) {ct++;} else
        if (ct == num) {outbuf[len] = inbuf[i]; len++;}
    }
    outbuf[len] = 0;
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
    tmpargs = malloc((argct + 1) * sizeof(char*));
    argl = malloc((argct + 1) * sizeof(int));
    for (int i = 0; i <= argct; i++) {
        argl[i] = 0;
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
    return solveargs();
}

uint8_t logictest(char* inbuf) {
    char tmp[3][32768];
    int tmpp = 0;
    uint8_t t1 = 0;
    uint8_t t2 = 0;
    int p = 0; /*   VAR   =   0   */
    bool inStr = false;
    int pct = 0;
    while (inbuf[p] == ' ') {p++;}
    if (p >= (int)strlen(inbuf)) {cerr = 10; return -1;}
    for (int i = p; true; i++) {
        if (inbuf[i] == '(' && !inStr) {pct++;}
        if (inbuf[i] == ')' && !inStr) {pct--;}
        if (inbuf[i] == '"') {inStr = !inStr;}  
        if (inbuf[i] == '\0') {cerr = 1; return -1;}
        if ((inbuf[i] == '<' || inbuf[i] == '=' || inbuf[i] == '>') && !inStr && pct == 0) {p = i; break;}
        if (inbuf[i] == ' ' && !inStr) {} else
        {tmp[0][tmpp] = inbuf[i]; tmpp++;}
    }
    tmp[0][tmpp] = '\0';
    tmpp = 0;
    for (int i = p; true; i++) {
        if (tmpp > 2) {cerr = 1; return -1;}
        if (inbuf[i] != '<' && inbuf[i] != '=' && inbuf[i] != '>') {p = i; break;} else
        {tmp[1][tmpp] = inbuf[i]; tmpp++;}
    }
    tmp[1][tmpp] = '\0';
    tmpp = 0;
    for (int i = p; true; i++) {
        if (inbuf[i] == '\0') {break;}
        if (inbuf[i] == ' ' && !inStr) {} else
        {tmp[2][tmpp] = inbuf[i]; tmpp++;}
    }
    tmp[2][tmpp] = '\0';
    t1 = getVal(tmp[0], tmp[0]);
    if (t1 == 0) return -1;
    t2 = getVal(tmp[2], tmp[2]);
    if (t2 == 0) return -1;
    if (t1 != t2) {cerr = 2; return -1;}
    if (!strcmp(tmp[1], "=")) {
        return !strcmp(tmp[0], tmp[2]);
    } else if (!strcmp(tmp[1], ">")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 > num2;
    } else if (!strcmp(tmp[1], "<")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 < num2;
    } else if (!strcmp(tmp[1], ">=")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 >= num2;
    } else if (!strcmp(tmp[1], "<=")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 <= num2;
    } else if (!strcmp(tmp[1], "=>")) {
        if (t1 == 1) {cerr = 2; return -1;}
        double num1, num2;
        sscanf(tmp[0], "%lf", &num1);
        sscanf(tmp[2], "%lf", &num2);
        return num1 >= num2;
    } else if (!strcmp(tmp[1], "=<")) {
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
    char tmp[2][32768];
    tmp[0][0] = 0; tmp[1][0] = 0;
    int i = 0;
    while (cmd[i] == ' ') {i++;}
    int j = i;
    while (cmd[j] != ' ' && cmd[j] != '\0') {j++;}
    copyStrSnip(cmd, i, j, tmp[0]);
    for (int h = 0; tmp[0][h] != '\0'; h++) {
        if (tmp[0][h] >= 'a' && tmp[0][h] <= 'z') {
            tmp[0][h] -= 32;
        }
    }
    cerr = 0;
    if (!strcmp(tmp[0], "DO")) {
        if (dlstackp >= 255) {cerr = 12; goto lexit;}
        dlstackp++;
        if (dlstackp > 0) {
            if (dldcmd[dlstackp - 1]) {dldcmd[dlstackp] = true; return true;}
        }
        if (itstackp > 0) {
            if (itdcmd[itstackp - 1]) {itdcmd[itstackp] = true; return true;}
        }
        dldcmd[dlstackp] = false;
        dlstack[dlstackp] = cmdpos;
        return true;
    }
    if (!strcmp(tmp[0], "DOWHILE")) {
        if (dlstackp >= 255) {cerr = 12; goto lexit;}
        dlstackp++;
        if (dlstackp > 0) {
            if (dldcmd[dlstackp - 1]) {dldcmd[dlstackp] = true; return true;}
        }
        if (itstackp > -1) {
            if (itdcmd[itstackp - 1]) {dldcmd[dlstackp] = true; return true;}
        }
        copyStrSnip(cmd, j + 1, strlen(cmd), tmp[1]);
        uint8_t testval = logictest(tmp[1]);
        if (testval != 1 && testval != 0) return true;
        dldcmd[dlstackp] = (bool)testval;
        dldcmd[dlstackp] = !dldcmd[dlstackp];
        return true;
    }
    if (!strcmp(tmp[0], "LOOP")) {
        if (dlstackp <= -1) {cerr = 6; return true;}
        if (itstackp > -1) {
            if (itdcmd[itstackp]) return true;
        }
        cp = dlstack[dlstackp];
        dlstackp--;
        didloop = true;
        return true;
    }
    if (!strcmp(tmp[0], "LOOPWHILE")) {
        if (dlstackp <= -1) {cerr = 6; return true;}
        if (itstackp > -1) {
            if (itdcmd[itstackp]) return true;
        }
        copyStrSnip(cmd, j + 1, strlen(cmd), tmp[1]);
        uint8_t testval = logictest(tmp[1]);
        if (testval != 1 && testval != 0) return true;
        if (testval == 1) cp = dlstack[dlstackp];
        dlstackp--;
        didloop = true;
        return true;
    }
    if (!strcmp(tmp[0], "IF")) {
        if (itstackp >= 255) {cerr = 13; return true;}
        itstackp++;
        if (itstackp > 0) {
            if (itdcmd[itstackp - 1]) {itdcmd[itstackp] = true; return true;}
        }
        if (dlstackp > 0) {
            if (dldcmd[dlstackp - 1]) {dldcmd[dlstackp] = true; return true;}
        }
        copyStrSnip(cmd, j + 1, strlen(cmd), tmp[1]);
        uint8_t testval = logictest(tmp[1]);
        if (testval != 1 && testval != 0) return true;
        itdcmd[itstackp] = (bool)testval;
        itdcmd[itstackp] = !itdcmd[itstackp];
        return true;
    }
    if (!strcmp(tmp[0], "ELSE")) {
        if (itstackp <= -1) {cerr = 8; goto lexit;}
        if (itstackp > 0) {
            if (itdcmd[itstackp - 1]) return true;
        }
        if (dlstackp > 0) {
            if (dldcmd[dlstackp - 1]) return true;
        }
        if (didelse) {cerr = 11; return true;}
        didelse = true;
        itdcmd[itstackp] = !itdcmd[itstackp];
        return true;
    }
    if (!strcmp(tmp[0], "ENDIF")) {
        if (itstackp <= -1) {cerr = 7; goto lexit;}
        itstackp--;
        return true;
    }
    return false;
    lexit:
    return true;
}

void runcmd() {
    if (cmd[0] == '\0' || cmd[0] == '\'') return;
    bool lgc = runlogic();
    if (lgc) goto cmderr;
    cerr = 255;
    if (dlstackp > -1) {if (dldcmd[dlstackp] == 1) return;}
    if (itstackp > -1) {if (itdcmd[itstackp] == 1) return;}
    if (!mkargs()) goto cmderr;
    for (int i = 0; i < argl[0]; i++) {
        if (arg[0][i] >= 'a' && arg[0][i] <= 'z') {
            arg[0][i] -= 32;
        }
    }
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
                printf("Invalid Variable Name: %s", errstr);
                break;
            case 5:
                printf("Divide by zero");
                break;
            case 6:
                printf("LOOP without DO");
                break;
            case 7:
                printf("ENDIF without IF");
                break;
            case 8:
                printf("ELSE without IF");
                break;
            case 9:
                printf("NEXT without FOR");
                break;
            case 10:
                printf("Expected expression");
                break;
            case 11:
                printf("Unexpected ELSE");
                break;
            case 12:
                printf("Reached DO limit");
                break;
            case 13:
                printf("Reached IF limit");
                break;
            case 14:
                printf("Reached FOR limit");
                break;
            case 127:
                printf("Not a Function: %s", errstr);
                break;
            case 255:
                printf("Not a Command: %s", arg[0]);
                break;
        }
        putc('\n', stdout);
        cp = strlen(conbuf);
    }
    if (lgc) return;
    for (int i = 0; i <= argct; i++) {
        if (tmpargs[i] != NULL) free(tmpargs[i]);
    }
    for (int i = 1; i <= argct; i++) {
        if (arg[i] != NULL) free(arg[i]);
    }
    if (tmpargs != NULL) free(tmpargs);
    if (argl != NULL) free(argl);
    if (argt != NULL) free(argt);
    if (arg != NULL) free(arg);
    return;
}
