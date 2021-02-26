#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/time.h>
#include <quadmath.h>

char VER[] = "0.4.1";

FILE **f;

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

char            *cmd;
int             cmdl;
char       **tmpargs;
char           **arg;
uint8_t        *argt;
int            *argl;
int        argct = 0;

bool debug = false;

void forceExit() {
    printf("\n");
    exit(255);
}

void cleanExit() {
    signal(SIGINT, forceExit);
    signal(SIGKILL, forceExit);
    signal(SIGTERM, forceExit);
    fflush(stdout);
    printf("\n");
    exit(err);
}

//    if (debug) printf(":\n");

int runcmd();
void copyStrSnip(char* str1, int i, int j, char* str2);

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanExit); 
    signal(SIGKILL, cleanExit); 
    signal(SIGTERM, cleanExit); 
    bool exit = false;
    for (int i = 1; i < argc; i++) {
        //printf("%d: [%s]\n", i, argv[i]);
        if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")) {
            printf("Command Line Interface BASIC version %s\n", VER);
            printf("Copyright (C) PQCraft 2021\n");
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
    printf("Command Line Interface BASIC version %s\n", VER);
    if (debug) printf("Running in debug mode\n");
    char conbuf[32768];
    //cmd = malloc(0);
    //argl = malloc(0);
    //arg = malloc(0);
    //tmpargs = malloc(0);
    //argt = malloc(0);
    while (!exit) {
        for (int i = 0; i < 32768; i++) conbuf[i] = 0;
        int cp = 0;
        printf("CLIBASIC> ");
        char c = 0;
        while (c != '\n') {
            c = getchar();
            if (c == '\0') {} else
            if (c == EOF) {cleanExit();} else
            if (c == '\b' && cp > 0) {cp--; conbuf[cp] = 0;} else
            if (c == '\e') {
                // Escape code handling code
            } else
            if (c == '\n') {conbuf[cp] = 0; goto proccmd;} else
            {conbuf[cp] = c; cp++;}
            if (conbuf[0] == '\0') c = 0;
        }
        proccmd:
        cp = 0;
        //int cmdp = 0;
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
        if (0) {};
    }
    cleanExit();
    return 0;
}

// reserved for mkvar and rmvar define lines

bool isSpChar(char* bfr, int pos) {
    return (bfr[pos] == '+' || bfr[pos] == '-' || bfr[pos] == '*' || bfr[pos] == '/' || bfr[pos] == '^' || bfr[pos] == ',');
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
    //if (debug) printf("copyStrApnd: strlen(str2): [%d]\n", (int)strlen(str2));
    for (i = strlen(str2); str1[j] != '\0'; i++) {str2[i] = str1[j]; j++;}
    str2[i] = 0;
}

uint8_t getType(char* str) {
    if (str[0] == '"') {if (str[strlen(str) - 1] != '"') {return 0;} return 1/*STR*/;}
    bool p = false;
    for (int i = 0; str[i] != '\0'; i++) {
        if ((str[i] < '0' || str[i] > '9') && str[i] != '.') {return 255 /*VAR*/;} else
        if (str[i] == '.') {if (p) {return 0 /*ERR*/;} p = true;}
    }
    return 2/*NUM*/;
}

uint8_t getVar(char* vn, char* varout) {
    if (vn[0] == '\0') return 0;
    int v = -1;
    for (int i = 0; i < varmaxct; i++) {
        if (varinuse[i] && !strcmp(vn, varname[i])) {v = i; break;}
    }
    if (v == -1) {
        if (vn[strlen(vn) - 1] == '$') {varout = ""; return 1;}
        else {varout = "0"; return 2;}
    } else {
        copyStr(varstr[v], &varout[v]);
        return vartype[v];
    }
    return 0;
}

uint8_t getVal(char* inbuf, char* outbuf) {
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
    if (isSpChar(inbuf, 0) || isSpChar(inbuf, strlen(inbuf) - 1)) {cerr = 1; return 0;}
    if (debug) printf("no syntax error detected\n");
    outbuf[0] = 0; //REM (427689): REMOVE LATER
    tmp[0][0] = 0; tmp[1][0] = 0;
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
        if (t == 255) {t = getVar(tmp[0], tmp[0]);}
        if (debug) printf("getType: tmp[0]: [%u]\n", t);
        if (debug) printf("getVal (4): tmp[0]: {%s}\n", tmp[0]);
        if (t != 0 && dt == 0) {dt = t;} else
        if ((t != 0 && t != dt)) {cerr = 2; return 0;} else
        if (t == 0) {cerr = 1; return false;}
        if ((dt == 1 && inbuf[jp] != '+') && inbuf[jp] != '\0') {cerr = 1; return 0;}
        // Solver
        if (debug) printf("getVal (4): tmp[1]: {%s}\n", tmp[1]);
        if (t == 1) {copyStrSnip(tmp[0], 1, strlen(tmp[0]) - 1, tmp[1]);} else
        if (t == 2) {
            copyStr(inbuf, tmp[0]);
            int p1, p2, p3;
            while (true) {
                numAct = 0;
                p1 = 0, p2 = 0, p3 = 0;
                if (debug) printf("checking for exp\n");
                for (int i = 0; tmp[0][i] != '\0' && p2 == 0; i++) {
                    if (tmp[0][i] == '^') {p2 = i; numAct = 4;}
                }
                if (debug) printf("checking for mlt/dvd\n");
                for (int i = 0; tmp[0][i] != '\0' && p2 == 0; i++) {
                    if (tmp[0][i] == '*') {p2 = i; numAct = 2;}
                    if (tmp[0][i] == '/') {p2 = i; numAct = 3;}
                }
                if (debug) printf("checking for add/sub\n");
                for (int i = 0; tmp[0][i] != '\0' && p2 == 0; i++) {
                    if (tmp[0][i] == '+') {p2 = i; numAct = 0;}
                    if (tmp[0][i] == '-') {p2 = i; numAct = 1;}
                }
                if (debug) printf("getVal: p1: [%d], p2: [%d], p3: [%d]\n", p1, p2, p3);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                if (p2 == 0) {
                    if (debug) printf("no operations found\n");
                    copyStr(tmp[0], tmp[1]); goto gvfexit;
                }
                for (int i = p2 - 1; i > 0; i--) {
                    if (isSpChar(tmp[0], i)) {p1 = i; break;}
                }
                for (int i = p2 + 1; true; i++) {
                    if (isSpChar(tmp[0], i) || tmp[0][i] == '\0') {p3 = i; break;}
                }
                copyStrSnip(tmp[0], p1, p2, tmp[2]);
                t = getType(tmp[2]);
                if (t == 0) {cerr = 1; return 0;} else
                if (t == 255) {t = getVar(tmp[2], tmp[2]); if (t != 2) {cerr = 2; return 0;}}
                copyStrSnip(tmp[0], p2 + 1, p3, tmp[3]);
                t = getType(tmp[3]);
                if (t == 0) {cerr = 1; return 0;} else
                if (t == 255) {t = getVar(tmp[3], tmp[3]); if (t != 2) {cerr = 2; return 0;}}
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
                sprintf(tmp[2],"%lf", num3);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                copyStrSnip(tmp[0], p3, strlen(tmp[0]), tmp[3]);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                if (p1 != 0) copyStrSnip(tmp[0], 0, p1 + 1, tmp[1]);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                copyStrApnd(tmp[2], tmp[1]);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                copyStrApnd(tmp[3], tmp[1]);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
                copyStr(tmp[1], tmp[0]);
                if (debug) printf("getVal: tmp[0]: {%s}, tmp[1]: {%s}, tmp[2]: {%s}, tmp[3]: {%s}\n", tmp[0], tmp[1], tmp[2], tmp[3]);
            }
        }
        if (debug) printf("getVal (4): tmp[1]: {%s}\n", tmp[1]);
        // 
        if (inbuf[jp] == '\0') {break;}
        jp++;
        ip = jp;
    }
    gvfexit:
    copyStr(tmp[1], outbuf);
    return t;
}

bool solveargs() {
    //bool inStr = false;
    //int pct = 0;
    //bool hitnull = false;
    if (debug) printf("solveargs: argct: %d\n", argct);
    argt = realloc(argt, argct + 1);
    arg = realloc(arg, (argct + 1) * sizeof(char*));
    char tmpbuf[32768];
    argt[0] = 0;
    arg[0] = tmpargs[0];
    /*for (int i = 1; i <= argct; i++) {
        int j = 0;
        while (tmpargs[i][j] != '\0') {j++;}
        arg[i] = realloc(arg[i], (j + 1) * sizeof(char));
        argl[i] = copyStr(tmpargs[i], arg[i]);
    }*/
    for (int i = 1; i <= argct; i++) {
        if (debug) printf("$ [%d]: %d\n", i, argl[i]);
        argt[i] = 0; // 0 = Unset (for error detection), 1 = string, 2 = number
        for (int p = 0; p < 32767; p++) {tmpbuf[p] = 0;}
        argt[i] = getVal(tmpargs[i], tmpbuf);
        if (debug) printf("solveargs: argt[%d]: %d\n", i, argt[i]);
        if (argt[i] == 0) return false;
        arg[i] = (char*)realloc(arg[i], (strlen(tmpbuf) + 1) * sizeof(char));
        copyStr(tmpbuf, arg[i]);
        argl[i] = strlen(arg[i]);
    }
    return true;
}

bool mkargs() {
    bool inStr = false;
    int pct = 0;
    bool hitnull = false;
    int cmdpos = 0;
    argct = 0;
    if (debug) printf("count args and detect syntax errors:\n");
    for (int i = 0; !hitnull; i++) {
        argl = realloc(argl, (i + 1) * sizeof(int));
        argl[i] = 0;
        while (cmd[cmdpos] != '\0') {
            if (debug) printf("1![%d], [%d]: %c [%d]\n", i, cmdpos, cmd[cmdpos], argct);
            if (i == 0) {
                while (cmd[cmdpos] == ' ' && argl[0] == 0) {if (debug) {printf("FOUND SPACE\n");} cmdpos++;}
                if (cmd[cmdpos] == ' ') {break;}
                else {argl[0]++;}
            } else {
                if (argct == 0) argct = 1;
                //while (cmd[cmdpos] == ' ' && !inStr/* && (is_spCharS1(cmd, cmdpos - 1) || (is_spCharS1(cmd, cmdpos + 1) && is_spCharS1(cmd, cmdpos - 1)))*/) {cmdpos++;}
                if (cmd[cmdpos] == ' ' && !inStr) {argl[i]--;}// else {if (debug) printf("?: %d\n", argl[i]);}
                if (cmd[cmdpos] == ',' && !inStr && pct == 0) {argct++; argl = realloc(argl, (argct + 1) * sizeof(int)); goto exitctloop;} else
                if (cmd[cmdpos] == '"') {inStr = !inStr;} else
                if (cmd[cmdpos] == '(' && !inStr) {pct++;} else
                if (cmd[cmdpos] == ')' && !inStr) {pct--;}
                argl[i]++;
            }
            if (cmd[cmdpos] == '\0') goto fexitctloop;
            cmdpos++;
            if (debug) printf("2![%d], [%d]: %c (%d)\n", i, cmdpos, cmd[cmdpos], (int)cmd[cmdpos]);
           if (cmd[cmdpos] == '\0') goto fexitctloop;
        }
        exitctloop:
        if (inStr || pct != 0) {cerr = 1; return false;}
        cmdpos++;
        if (cmd[cmdpos] == '\0') hitnull = true;
        if (debug) printf("3![%d], [%d]: %c [%d]\n", i, cmdpos, cmd[cmdpos], argct);
    }
    fexitctloop:
    hitnull = false;
    cmdpos = 0;
    if (debug) printf("argct: %d\n", argct);
    tmpargs = realloc(tmpargs, (argct + 1) * sizeof(char*));
    if (debug) printf("allocate tmpargs size:\n");
    for (int i = 0; i <= argct; i++) {if (debug) {printf("realloc size [%d]: %d\n", i, argl[i] + 1);}}
    if (debug) printf("allocating tmpargs:\n");
    for (int i = 0; i <= argct; i++) {
        if (debug) printf("realloc size [%d]: %d\n", i, argl[i] + 1);
        tmpargs[i] = (char*)realloc(tmpargs[i], (argl[i] + 1) * sizeof(char));
        if (debug) printf("done\n");
    }
    if (debug) printf("put args in arg containers:\n");
    for (int i = 0; i <= argct; i++) {
        int argpos = 0;
        while (argpos <= argl[i]) {
            if (debug) printf("1t[%d], [%d]: %c [%d]\n", i, cmdpos, cmd[cmdpos], argct);
            if (i == 0) {
                while (cmd[cmdpos] == ' ' && argl[0] == 0) {if (debug) {printf("FOUND SPACE\n");} cmdpos++;}
                if (cmd[cmdpos] == ' ') {break;}
                else {tmpargs[0][argpos] = cmd[cmdpos]; argpos++;}
            } else {
                //if (cmd[cmdpos] == ' ' && !inStr && argl[i] == 0) {cmdpos++;}
                while (cmd[cmdpos] == ' ' && !inStr) {cmdpos++;}
                if (cmd[cmdpos] == ',' && !inStr && pct == 0) {goto exittxloop;} else
                if (cmd[cmdpos] == '"') {inStr = !inStr;} else
                if (cmd[cmdpos] == '(' && !inStr) {pct++;} else
                if (cmd[cmdpos] == ')' && !inStr) {pct--;}
                tmpargs[i][argpos] = cmd[cmdpos];
                argpos++;
            }
            //if (cmd[cmdpos] == '\0') goto fexittxloop;
            cmdpos++;
            if (debug) printf("2t[%d], [%d]: %c (%d)\n", i, cmdpos, cmd[cmdpos], (int)cmd[cmdpos]);
            //if (cmd[cmdpos] == '\0') goto fexittxloop;
        }
        tmpargs[i][argl[i]] = '\0';
        exittxloop:
        if (debug) printf("tmpargs[i]: {%s}\n", tmpargs[i]);
        if (inStr || pct != 0) {cerr = 1; return false;}
        cmdpos++;
        if (cmd[cmdpos + 1] == '\0') hitnull = true;
        if (debug) printf("3t[%d], [%d]: %c [%d]\n", i, cmdpos, cmd[cmdpos], argct);
        if (debug) printf("tmpargs[%d]: {%s}\n", i, tmpargs[i]);
    }
    //fexittxloop:
    if (debug) printf("argct: %d\n", argct);    
    if (debug) printf("check for blank arg:\n");
    if (argct == 1 && tmpargs[1][0] == '\0') {argct = 0;}
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
    //cerr = 255;
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
    /*if (inProg) {printf("Line %u: ");}*/
    if (debug) printf("cerr: %d\n", cerr);
    cmderr:
    if (debug) printf("cerr: %d\n", cerr);
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
            case 255:
                printf("Not a Command: %s", arg[0]);
                break;
        }
        printf("\n");
    }
    return cerr;
}
