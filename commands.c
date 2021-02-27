if (!strcmp(arg[0], "FOO") || !strcmp(arg[0], "FOOBAR")) {
    cerr = 0;
    if (debug) printf("CMD[FOO/FOOBAR]: argct: [%d]\n", argct);
    goto cmderr;
}
if (!strcmp(arg[0], "EXIT")) {
    cerr = 0;
    if (argct == 1) err = atoi(arg[1]); 
    if (argct > 1) {cerr = 3; goto cmderr;}
    printf("\e[A");
    cleanExit();
}
if (!strcmp(arg[0], "EXEC") || !strcmp(arg[0], "SH")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    err = system(arg[1]);
    goto cmderr;
}
if (!strcmp(arg[0], "PRINT")) {
    cerr = 0;
    for (int i = 1; i <= argct; i++) {getStr(arg[i], arg[i]); printf("%s", arg[i]);}
    goto cmderr;
}
if (!strcmp(arg[0], "COLOR")) {
    cerr = 0;
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    fgc = (uint8_t)atoi(arg[1]);
    if (argct > 1) {
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
        bgc = (uint8_t)atoi(arg[2]);
    }
    if (debug) printf("CMD[COLOR]: fgc: [%u], bgc: [%u]\n", fgc, bgc);
    printf("\e[38;5;%um", fgc);
    printf("\e[48;5;%um", bgc);
    goto cmderr;
}
if (!strcmp(arg[0], "CLS")) {
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
    uint8_t tbgc = bgc;
    if (argct != 0) {
        if (argt[1] != 2) {cerr = 2; goto cmderr;}
        tbgc = (uint8_t)atoi(arg[1]);
    }
    printf("\e[48;5;%um\e[0;0H\e[2J\e[48;5;%um", tbgc, bgc);
    goto cmderr;
}
