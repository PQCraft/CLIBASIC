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

FILE *f[256];

int err = 0;
int cerr;
char cerrstr[32];

uint32_t varsize[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
uint8_t  *u08var;
int8_t   *s08var;
uint16_t *u16var;
int16_t  *s16var;
uint32_t *u32var;
int32_t  *s32var;
uint64_t *u64var;
int64_t  *s64var;
float    *sflvar;
double   *sdbvar;
char    **strvar;

char cmd[65536] = "";
char *tmpargs[256];
char *arg[256];
uint8_t argt[256];
int argl[256];
int argct = 0;

bool debug = false;

void forceExit() {
    printf("\n");
    exit(255);
}

void cleanExit() {
    signal(SIGINT, forceExit);
    signal(SIGKILL, forceExit);
    signal(SIGTERM, forceExit);
    free(u08var);
    free(s08var);
    free(u16var);
    free(s16var);
    free(u32var);
    free(s32var);
    free(u64var);
    free(s64var);
    free(sflvar);
    free(sdbvar);
    free(strvar);
    fflush(stdout);
    printf("\n");
    exit(err);
}

char VER[] = "0.2";

int runcmd();

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
            exit = true;
        }
        if (!strcmp(argv[i], "--debug") || !strcmp(argv[i], "-d")) {
            debug = true;
        }
    }
    if (exit) cleanExit();
    printf("Command Line Interface BASIC version %s\n", VER);
    if (debug) printf("Running in debug mode\n");
    u08var = (uint8_t *) malloc(0);
    s08var = (int8_t *) malloc(0);
    u16var = (uint16_t *) malloc(0);
    s16var = (int16_t *) malloc(0);
    u32var = (uint32_t *) malloc(0);
    s32var = (int32_t *) malloc(0);
    u64var = (uint64_t *) malloc(0);
    s64var = (int64_t *) malloc(0);
    sflvar = (float *) malloc(0);
    sdbvar = (double *) malloc(0);
    strvar = malloc(0);    
    for (int i = 0; i < 256; i++) {
        tmpargs[256] = (char *) malloc(0);
        arg[256] = (char *) malloc(0);
    }
    while (!exit) {
        for (int i = 0; i < 65536; i++) {cmd[i] = 0;}
        int cp = 0;
        printf("CLIBASIC> ");
        char c = 0;
        while (c != '\n') {
            c = getc(stdin);
            if (c == EOF) {cleanExit();} else
            if (c == '\b' && cp > 0) {cp--; cmd[cp] = 0;} else
            if (c == '\e') {
            
            } else
            if (c == '\n') {} else
            {cmd[cp] = c; cp++;}
        }
        if (debug) printf("cmd: {%s}\n", cmd);
        if (cp > 0) runcmd();
    }
    cleanExit();
}

bool is_spCharS1(char* bfr, int pos) {
    return (bfr[pos] == '+' || bfr[pos] == '-' || bfr[pos] == '/' || bfr[pos] == '*' || bfr[pos] == '^' || bfr[pos] == '%' || bfr[pos] == ',' || bfr[pos] == ' ');
}

uint8_t getType(char* str) {
    if (str[0] == '"') {return 1/*STR*/;}
    bool p = false;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' && str[i] > '9' && str[i] != '.') {return 255 /*VAR*/;} else
        if (str[i] == '.') {if (p) {return 0 /*ERR*/;} p = true;}
    }
    return 2/*NUM*/;
}

bool solveargs() {
    bool inStr = false;
    int pct = 0;
    bool hitnull = false;
    if (debug) printf("argct: %d\n", argct);
    char tmpbuf[65536];
    for (int i = 0; i <= argct; i++) {
        if (debug) printf("$ [%d]: %d\n", i, argl[i]);
        argt[i] = 0; // 0 = Unset (for syntax error detection), 1 = string, 2 = number (int64_t), 3... specific number types
        arg[i] = realloc(arg[i], (argl[i] + 1) * sizeof(char));
        arg[i] = "";
        arg[i] = tmpargs[i];
        argt[i] = getType(arg[i]);
    }
    return true;
}

bool mkargs() {
    bool inStr = false;
    int pct = 0;
    bool hitnull = false;
    int cmdpos = 0;
    for (int i = 0; i < 256 && !hitnull; i++) {
        argl[i] = 0;
        while (cmd[cmdpos] != '\0') {
            if (debug) printf("1![%d], [%d]: %c [%d]\n", i, cmdpos, cmd[cmdpos], argct);
            if (i == 0) {
                while (cmd[cmdpos] == ' ' && argl[0] == 0) {if (debug) {printf("FOUND SPACE\n");} cmdpos++;}
                if (cmd[cmdpos] == ' ') {break;}
                else {argl[0]++;}
            } else {
                if (cmd[cmdpos] == ' ' && !inStr && argl[i] == 0) {cmdpos++;}// else {if (debug) printf("?: %d\n", argl[i]);}
                while (cmd[cmdpos] == ' ' && !inStr/* && (is_spCharS1(cmd, cmdpos - 1) || (is_spCharS1(cmd, cmdpos + 1) && is_spCharS1(cmd, cmdpos - 1)))*/) {cmdpos++;}
                argl[i]++;
                if (cmd[cmdpos] == ',' && !inStr && pct == 0) {goto exitctloop;} else
                if (cmd[cmdpos] == '"') {inStr = !inStr;} else
                if (cmd[cmdpos] == '(' && !inStr) {pct++;} else
                if (cmd[cmdpos] == ')' && !inStr) {pct--;}
            }
            cmdpos++;
            if (debug) printf("2![%d], [%d]: %c (%d)\n", i, cmdpos, cmd[cmdpos], (int)cmd[cmdpos]);
        }
        exitctloop:
        if (inStr || pct != 0) {cerr = 1; return false;}
        cmdpos++;
        argct = i;
        if (cmd[cmdpos] == '\0') hitnull = true;
        if (debug) printf("3![%d], [%d]: %c [%d]\n", i, cmdpos, cmd[cmdpos], argct);
    }
    hitnull = false;
    cmdpos = 0;
    for (int i = 0; i <= argct && !hitnull; i++) {
        tmpargs[i] = (char *) realloc(tmpargs[i], (argl[i] + 1) * sizeof(char));
        for (int j = 0; j < argl[i]; j++) {
            if (cmd[cmdpos] == '\0') {hitnull = true; break;}
            if (debug) printf("1tx args[%d], [%d]: %c (%d)\n", i, cmdpos, cmd[cmdpos], (int)cmd[cmdpos]);
            if (i == 0) {
                while (cmd[cmdpos] == ' ' && j == 0) {if (debug) {printf("FOUND SPACE\n");} cmdpos++;}
                if (cmd[cmdpos] == ' ') {break;}
                else {
                    if(cmd[cmdpos] >= 'a' && cmd[cmdpos] <= 'z') {
                        cmd[cmdpos] = cmd[cmdpos] -32;
                    }
                    tmpargs[0][j] = cmd[cmdpos];
                }
            } else {
                if (cmd[cmdpos] == ' ' && !inStr && argl[i] == 0) {cmdpos++;}// else {if (debug) printf("?: %d\n", argl[i]);}
                while (cmd[cmdpos] == ' ' && !inStr/* && (is_spCharS1(cmd, cmdpos - 1) || (is_spCharS1(cmd, cmdpos + 1) && is_spCharS1(cmd, cmdpos - 1)))*/) {cmdpos++;}
                if (cmd[cmdpos] == ',' && !inStr && pct == 0) {goto exittxloop;} else
                if (cmd[cmdpos] == '"') {inStr = !inStr;} else
                if (cmd[cmdpos] == '(' && !inStr) {pct++;} else
                if (cmd[cmdpos] == ')' && !inStr) {pct--;}
                tmpargs[i][j] = cmd[cmdpos];
            }
            cmdpos++;
            tmpargs[i][j + 1] = 0;
            if (debug) printf("2tx args[%d], [%d]: %c (%d)\n", i, cmdpos, cmd[cmdpos], (int)cmd[cmdpos]);
        }
        exittxloop:
        cmdpos++;
    }
    if (argct == 1 && tmpargs[1][0] == '\0') {argct = 0;}
    
    return solveargs();
}

int runcmd() {
    if (debug) printf("%s\n", cmd);
    if (!mkargs()) goto cmderr;
    if (debug) printf("%s\n", cmd);
    cerr = 255;
    for (int i = 0; i <= argct; i++) {
        if (debug) printf("C [%d]: {%s}\n", i, arg[i]);
    }
    if (!strcmp(arg[0], "EXIT")) {
        err = 0;
        if (argct == 1) err = atoi(arg[1]); 
        if (argct > 1) {cerr = 3; goto cmderr;}
        printf("\x1b[A");
        cleanExit();
    }
    /*if (inProg) {printf("Line %u: ");}*/
    cmderr:
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
            case 255:
                printf("Not a Command: %s", arg[0]);
                break;
        }
        printf("\n");
    }
    return cerr;
}

/*
        if((arg[0][i] >= 'a' && arg[0][i] <= 'z')) {
            arg[0][i] -= 32;
        }
*/