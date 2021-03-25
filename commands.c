if (!qstrcmp(arg[0], "EXIT") || !qstrcmp(arg[0], "QUIT")) {
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
    if (argct == 1) {if (!solvearg(1)) goto cmderr; err = atoi(arg[1]);}
    if (inProg) {inProg = false; goto cmderr;}
    cleanExit();
}
if (!qstrcmp(arg[0], "PRINT") || !qstrcmp(arg[0], "?")) {
    cerr = 0;
    for (int i = 1; i <= argct; i++) {if (!solvearg(i)) goto cmderr; printf("%s", arg[i]);}
    fflush(stdout);
}
if (!qstrcmp(arg[0], "COLOR")) {
    cerr = 0;
    int tmp = 0;
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {} else
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {
        tmp = atoi(arg[1]);
        if (tmp < 0 || tmp > 255) {cerr = 16; goto cmderr;}
        fgc = (uint8_t)tmp;
        printf("\e[38;5;%um", fgc);
    }
    if (argct > 1) {
        if (!solvearg(2)) goto cmderr;
        if (argt[2] == 0) {} else
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {
            tmp = atoi(arg[2]);
            if (tmp < 0 || tmp > 255) {cerr = 16; goto cmderr;}
            fgc = (uint8_t)tmp;
            printf("\e[48;5;%um", bgc);
        }
    }
    fflush(stdout);
}
if (!qstrcmp(arg[0], "SET") || !qstrcmp(arg[0], "LET")) {
    cerr = 0;
    if (argct != 2) {cerr = 3; goto cmderr;}
    solvearg(1);
    if (!solvearg(2)) goto cmderr;
    if (argt[1] == 0 || argt[2] == 0) {cerr = 1; goto cmderr;}
    int v = -1;
    for (int i = 0; i < varmaxct; i++) {
        if (varinuse[i] && !qstrcmp(arg[1], varname[i])) {v = i; break;}
    }
    if (argt[1] != argt[2] && v != -1) {cerr = 2; goto cmderr;}
    if (getType(tmpargs[1]) != 255) {
        cerr = 4;
        errstr = realloc(errstr, (strlen(tmpargs[1]) + 1) * sizeof(char));
        copyStr(tmpargs[1], errstr);
        goto cmderr;
    }
    if (!setVar(tmpargs[1], arg[2], argt[2])) goto cmderr;
}
if (!qstrcmp(arg[0], "LOCATE")) {
    cerr = 0;
    int tmp = 0;
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {} else
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {
        tmp = atoi(arg[1]);
        if (tmp < 0) {cerr = 16; goto cmderr;}
        curx = tmp;
    }
    if (argct > 1) {
        if (!solvearg(2)) goto cmderr;
        if (argt[2] == 0) {} else
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {
            tmp = atoi(arg[2]);
            if (tmp < 0) {cerr = 16; goto cmderr;}
            cury = tmp;
        }
    }
    printf("\e[%d;%dH", cury, curx);
    fflush(stdout);
}
if (!qstrcmp(arg[0], "CLS")) {
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
    uint8_t tbgc = bgc;
    if (argct != 0) {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 2) {cerr = 2; goto cmderr;}
        tbgc = (uint8_t)atoi(arg[1]);
    }
    printf("\e[48;5;%um\e[H\e[2J\e[3J\e[48;5;%um", tbgc, bgc);
    fflush(stdout);
}
if (!qstrcmp(arg[0], "WAIT")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    uint64_t t = d * 1000000 + time_us();
    while (t > time_us()) {}
}    
if (!qstrcmp(arg[0], "WAITMS")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    uint64_t t = d * 1000 + time_us();
    while (t > time_us()) {}
}    
if (!qstrcmp(arg[0], "WAITUS")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    uint64_t t = d + time_us();
    while (t > time_us()) {}
}    
if (!qstrcmp(arg[0], "EXEC") || !qstrcmp(arg[0], "SH")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    cerr = system(arg[1]);
    cerr = 0;
}
if (!qstrcmp(arg[0], "RUN")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    prog = fopen(arg[1], "r");
    if (prog == NULL) {errstr = realloc(errstr, (argl[1] + 1) * sizeof(char)); copyStr(arg[1], errstr); cerr = 15; goto cmderr;}
    progFilename = malloc(argl[1] + 1);
    copyStr(arg[1], progFilename);
    printf("Loading...");
    fflush(stdout);
    loadProg();
    for (uint8_t i = 0; i < 10; i++) printf("\b");
    for (uint8_t i = 0; i < 10; i++) printf(" ");
    for (uint8_t i = 0; i < 10; i++) printf("\b");
    fflush(stdout);
    chkinProg = true;
}
if (!qstrcmp(arg[0], "SRAND") || !qstrcmp(arg[0], "SRND")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    double rs;
    sscanf(arg[1], "%lf", &rs);
    srand(rs);
}    
if (!qstrcmp(arg[0], "RESETTIMER")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    resetTimer();
}
if (!qstrcmp(arg[0], "$PROMPT")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    copyStr(tmpargs[1], prompt);
}
if (!qstrcmp(arg[0], "$SAVECMDHST")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    write_history(arg[1]);
}
if (!qstrcmp(arg[0], "$LOADCMDHST")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    read_history(arg[1]);
}
if (!qstrcmp(arg[0], "$TXTRESET")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[0m\e[38;5;%um\e[48;5;%um", fgc, bgc);
    fflush(stdout);
}
if (!qstrcmp(arg[0], "$TXTBOLD")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[1m");
}
if (!qstrcmp(arg[0], "$TXTBOLD")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[1m");
}
if (!qstrcmp(arg[0], "$TXTDIM")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[2m");
}
if (!qstrcmp(arg[0], "$TXTITAL")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[3m");
}
if (!qstrcmp(arg[0], "$TXTUNDRLN")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[4m");
}
if (!qstrcmp(arg[0], "$TXTULCLR")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    printf("\e[58:5:%um", (uint8_t)atoi(arg[1]));
}
if (!qstrcmp(arg[0], "$TXTOVERLN")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[53m");
}
if (!qstrcmp(arg[0], "$TXTDBLUL")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[21m");
}
if (!qstrcmp(arg[0], "$TXTSQGUL")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[4:3m");
}
if (!qstrcmp(arg[0], "$TXTBLINK")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[5m");
}
if (!qstrcmp(arg[0], "$TXTREV")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[7m");
}
if (!qstrcmp(arg[0], "$TXTHIDE")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[8m");
}
if (!qstrcmp(arg[0], "$TXTSTRIKE")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[9m");
}
if (!qstrcmp(arg[0], "$TXTLOCK")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (!textlock) {
        tcgetattr(0, &term);
        tcgetattr(0, &restore);
        term.c_lflag &= ~(ICANON|ECHO);
        tcsetattr(0, TCSANOW, &term);
    }
    textlock = true;
}
if (!qstrcmp(arg[0], "$TXTUNLOCK")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (textlock) tcsetattr(0, TCSANOW, &restore);
    textlock = false;
}
if (!qstrcmp(arg[0], "$DEBUGON")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (!debug) printf("Enabled debug mode.\n");
    debug = true;
}
if (!qstrcmp(arg[0], "$DEBUGOFF")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (debug) printf("Disabled debug mode.\n");
    debug = false;
}
