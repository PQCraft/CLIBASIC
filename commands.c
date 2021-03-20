if (!strcmp(arg[0], "EXIT") || !strcmp(arg[0], "QUIT")) {
    cerr = 0;
    if (argct == 1) err = atoi(arg[1]); 
    if (argct > 1) {cerr = 3; goto cmderr;}
    if (inProg) {cmdint = true; goto cmderr;}
    cleanExit();
}
if (!strcmp(arg[0], "PRINT") || !strcmp(arg[0], "?")) {
    cerr = 0;
    for (int i = 1; i <= argct; i++) {getStr(arg[i], arg[i]); printf("%s", arg[i]);}
    fflush(stdout);
}
if (!strcmp(arg[0], "COLOR")) {
    cerr = 0;
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    if (argt[1] == 0) {} else
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {fgc = (uint8_t)atoi(arg[1]);}
    if (argct > 1) {
        if (argt[2] == 0) {} else
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {bgc = (uint8_t)atoi(arg[2]);}
    }
    if (debug) printf("CMD[COLOR]: fgc: [%u], bgc: [%u]\n", fgc, bgc);
    printf("\e[38;5;%um\e[48;5;%um", fgc, bgc);
    fflush(stdout);
}
if (!strcmp(arg[0], "SET") || !strcmp(arg[0], "LET")) {
    cerr = 0;
    if (argt[1] == 0) {cerr = 3; goto cmderr;}
    if (argct != 2) {cerr = 3; goto cmderr;}
    int v = -1;
    for (int i = 0; i < varmaxct; i++) {
        if (varinuse[i] && !strcmp(arg[1], varname[i])) {v = i; break;}
    }
    if (argt[1] != argt[2] && v != -1) {cerr = 2; goto cmderr;}
    if (getType(tmpargs[1]) != 255) {cerr = 3; goto cmderr;}
    if (!setVar(tmpargs[1], arg[2], argt[2])) goto cmderr;
}
if (!strcmp(arg[0], "LOCATE")) {
    cerr = 0;
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    if (argt[1] == 0) {} else
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {cury = atoi(arg[1]);}
    if (argct > 1) {
        if (argt[2] == 0) {} else
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {curx = atoi(arg[2]);}
    }
    if (curx < 0) {getCurPos(); cerr = 16; goto cmderr;}
    if (cury < 0) {getCurPos(); cerr = 16; goto cmderr;}
    printf("\e[%d;%dH", cury, curx);
    fflush(stdout);
}
if (!strcmp(arg[0], "CLS")) {
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
    uint8_t tbgc = bgc;
    if (argct != 0) {
        if (argt[1] != 2) {cerr = 2; goto cmderr;}
        tbgc = (uint8_t)atoi(arg[1]);
    }
    printf("\e[48;5;%um\e[H\e[2J\e[3J\e[48;5;%um", tbgc, bgc);
    fflush(stdout);
}
if (!strcmp(arg[0], "WAIT")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    uint64_t t = d * 1000000 + time_us();
    while (t > time_us()) {}
}    
if (!strcmp(arg[0], "WAITMS")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    uint64_t t = d * 1000 + time_us();
    while (t > time_us()) {}
}    
if (!strcmp(arg[0], "WAITUS")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    uint64_t t = d + time_us();
    while (t > time_us()) {}
}    
if (!strcmp(arg[0], "EXEC") || !strcmp(arg[0], "SH")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    cerr = system(arg[1]);
    cerr = 0;
}
if (!strcmp(arg[0], "RUN")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    prog = fopen(arg[1], "r");
    if (prog == NULL) {errstr = realloc(errstr, (argl[1] + 1) * sizeof(char)); copyStr(arg[1], errstr); cerr = 15; goto cmderr;}
    progFilename = malloc(argl[1] + 1);
    copyStr(arg[1], progFilename);
    chkinProg = true;
}
if (!strcmp(arg[0], "SRAND") || !strcmp(arg[0], "SRND")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    double rs;
    sscanf(arg[1], "%lf", &rs);
    srand(rs);
}    
if (!strcmp(arg[0], "RESETTIMER")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    resetTimer();
}
if (!strcmp(arg[0], "$PROMPT")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    copyStr(tmpargs[1], prompt);
}
if (!strcmp(arg[0], "$SAVECMDHST")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    write_history(arg[1]);
}
if (!strcmp(arg[0], "$LOADCMDHST")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    read_history(arg[1]);
}
if (!strcmp(arg[0], "$TXTRESET")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[0m\e[38;5;%um\e[48;5;%um", fgc, bgc);
    fflush(stdout);
}
if (!strcmp(arg[0], "$TXTBOLD")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[1m");
}
if (!strcmp(arg[0], "$TXTBOLD")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[1m");
}
if (!strcmp(arg[0], "$TXTDIM")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[2m");
}
if (!strcmp(arg[0], "$TXTITAL")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[3m");
}
if (!strcmp(arg[0], "$TXTUNDRLN")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[4m");
}
if (!strcmp(arg[0], "$TXTULCLR")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    printf("\e[58:5:%um", (uint8_t)atoi(arg[1]));
}
if (!strcmp(arg[0], "$TXTOVERLN")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[53m");
}
if (!strcmp(arg[0], "$TXTDBLUL")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[21m");
}
if (!strcmp(arg[0], "$TXTSQGUL")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[4:3m");
}
if (!strcmp(arg[0], "$TXTBLINK")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[5m");
}
if (!strcmp(arg[0], "$TXTREV")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[7m");
}
if (!strcmp(arg[0], "$TXTHIDE")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[8m");
}
if (!strcmp(arg[0], "$TXTSTRIKE")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    printf("\e[9m");
}
if (!strcmp(arg[0], "$TXTLOCK")) {
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
if (!strcmp(arg[0], "$TXTUNLOCK")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (textlock) tcsetattr(0, TCSANOW, &restore);
    textlock = false;
}
if (!inProg && !strcmp(arg[0], "$DEBUGON")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (!debug) printf("Enabled debug mode.\n");
    debug = true;
}
if (!inProg && !strcmp(arg[0], "$DEBUGOFF")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (debug) printf("Disabled debug mode.\n");
    debug = false;
}
