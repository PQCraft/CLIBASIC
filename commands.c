if (!strcmp(arg[0], "FOO") || !strcmp(arg[0], "FOOBAR")) {
    cerr = 0;
    if (debug) printf("CMD[FOO/FOOBAR]: argct: %d\n", argct);
    goto cmderr;
}
if (!strcmp(arg[0], "EXIT")) {
    cerr = 0;
    if (debug) printf("CMD[EXIT]: argct: %d\n", argct);
    if (argct == 1) err = atoi(arg[1]); 
    if (argct > 1) {cerr = 3; goto cmderr;}
    printf("\e[A");
    cleanExit();
}
if (!strcmp(arg[0], "CLS")) {
    cerr = 0;
    if (debug) printf("CMD[CLS]: argct: %d\n", argct);
    if (argct > 1) {cerr = 3; goto cmderr;}
    printf("\e[0;0H\e[2J");
    goto cmderr;
}
if (!strcmp(arg[0], "PRINT")) {
    cerr = 0;
    if (debug) printf("CMD[PRINT]: argct: %d\n", argct);
    for (int i = 1; i <= argct; i++) {getStr(arg[i], arg[i]); printf("%s", arg[i]);}
    goto cmderr;
}
if (!strcmp(arg[0], "EXEC") || !strcmp(arg[0], "SH")) {
    cerr = 0;
    if (debug) printf("CMD[EXEC/SH]: argct: %d\n", argct);
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    err = system(arg[1]);
    goto cmderr;
}
