if (chkCmd(1, farg[0], "CHR$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    outbuf[0] = (char)atoi(farg[1]);
    outbuf[1] = 0;
    goto fexit;
}
if (chkCmd(1, farg[0], "ASC")) {
    cerr = 0;
    ftype = 2;
    int pos = 0;
    if (fargct != 1 && fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    if (fargct == 2) {
        if (fargt[2] != 2) {cerr = 2; goto fexit;}
        pos = (int)atoi(farg[2]);
        if (pos < 0) pos = 0;
        if (pos > (int)strlen(farg[1])) pos = strlen(farg[1]);
    }
    sprintf(outbuf, "%d", farg[1][pos]);
    goto fexit;
}
if (chkCmd(2, farg[0], "RND", "RAND")) {
    cerr = 0;
    ftype = 2;
    double min = 0;
    double max;
    if (fargct == 1) {
        if (fargt[1] != 2) {cerr = 2; goto fexit;}
        sscanf(farg[1], "%lf", &max);
    } else if (fargct == 2) {
        if (fargt[1] + fargt[2] != 4) {cerr = 2; goto fexit;}
        sscanf(farg[1], "%lf", &min);
        sscanf(farg[2], "%lf", &max);
    } else {
        cerr = 3;
        goto fexit;
    }
    sprintf(outbuf, "%lf", randNum(min, max));
    if (debug) printf("1: outbuf: {%s}\n", outbuf);
    goto fexit;
}
if (chkCmd(1, farg[0], "TIMERUS")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)timer());
    goto fexit;
}
if (chkCmd(1, farg[0], "TIMERMS")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)timer() / 1000);
    goto fexit;
}
if (chkCmd(1, farg[0], "TIMER")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)timer() / 1000000);
    goto fexit;
}
if (chkCmd(1, farg[0], "TIMEUS")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)time_us());
    goto fexit;
}
if (chkCmd(1, farg[0], "TIMEMS")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)time_us() / 1000);
    goto fexit;
}
if (chkCmd(1, farg[0], "TIME")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)time_us() / 1000000);
    goto fexit;
}
if (chkCmd(1, farg[0], "CINT")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    if (debug) printf("farg[1]: {%s}\n", farg[1]);
    sscanf(farg[1], "%lf", &dbl);
    dbl = round(dbl);
    sprintf(outbuf, "%d", (int)dbl);
    if (debug) printf("outbuf: {%s}\n", outbuf);
    goto fexit;
}
if (chkCmd(1, farg[0], "INT")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%d", (int)dbl);
    int i;
    for (i = 0; outbuf[i] != '.' && outbuf[i] != 0; i++) {}
    outbuf[i] = 0;
    goto fexit;
}
if (chkCmd(1, farg[0], "VAL")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%lf", dbl);
    //ftype = getVal(outbuf, outbuf);
    //if (ftype == 1) {cerr = 2; goto fexit;}
    goto fexit;
}
if (chkCmd(1, farg[0], "STR$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    copyStr(farg[1], outbuf);
    goto fexit;
}
if (chkCmd(1, farg[0], "UCASE$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    upCase(farg[1]);
    copyStr(farg[1], outbuf);
    goto fexit;
}
if (chkCmd(1, farg[0], "LCASE$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    lowCase(farg[1]);
    copyStr(farg[1], outbuf);
    goto fexit;
}
if (chkCmd(1, farg[0], "LEN")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    sprintf(outbuf, "%lu", (long unsigned)strlen(farg[1]));
    goto fexit;
}
if (chkCmd(1, farg[0], "INKEY$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 0) {cerr = 3; goto fexit;}
    enableRawMode();
    int obp = -1;
    int tmp = -1;
    outbuf[0] = 0;
    while ((outbuf[0] == 27 && tmp != 0) || outbuf[0] == 0) {
        obp++;
        tmp = read(1, &outbuf[obp], 1);
        if (tmp == 0) {outbuf[obp] = 0; break;}
        if (tmp == -1) {outbuf[obp] = 0; break;}
        if (outbuf[obp] == 3) {outbuf[obp] = 0; cmdint = true; break;}
    }
    obp++;
    outbuf[obp] = 0;
    disableRawMode();
    goto fexit;
}
if (chkCmd(1, farg[0], "CURX")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;}
    getCurPos();
    sprintf(outbuf, "%d", curx);
    goto fexit;
}
if (chkCmd(1, farg[0], "CURY")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;}
    getCurPos();
    sprintf(outbuf, "%d", cury);
    goto fexit;
}
if (chkCmd(1, farg[0], "SYSTEM")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    sprintf(outbuf, "%d", system(farg[1]));
    goto fexit;
}
if (chkCmd(1, farg[0], "FGC")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (int)fgc);
    goto fexit;
}
if (chkCmd(1, farg[0], "BGC")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (int)bgc);
    goto fexit;
}
if (chkCmd(1, farg[0], "INPUT$")) {
    cerr = 0;
    ftype = 1;
    if (fargct > 1) {cerr = 3; goto fexit;}
    if (fargct == 1 && fargt[1] != 1) {cerr = 2; goto fexit;}
    outbuf[0] = 0;
    char* tmp = NULL;
    if (fargct == 1) {
        tmp = readline(farg[1]);
    } else {
        tmp = readline("?: ");
    }
    if (tmp != NULL) {
        copyStr(tmp, outbuf);
        free(tmp);
    }
    if (debug) printf("input output: {%s}\n", outbuf);
    goto fexit;
}
if (chkCmd(1, farg[0], "_PROMPT$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 0) {cerr = 3; goto fexit;}
    int tmpt = getVal(prompt, outbuf);
    if (tmpt != 1) strcpy(outbuf, "CLIBASIC> ");
    goto fexit;
}
if (chkCmd(1, farg[0], "_DEBUG")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (int)debug);
    goto fexit;
}
if (chkCmd(1, farg[0], "_VER$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 0) {cerr = 3; goto fexit;}
    copyStr(VER, outbuf);
    goto fexit;
}
if (chkCmd(1, farg[0], "_BITS$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 0) {cerr = 3; goto fexit;}
    copyStr(BVER, outbuf);
    goto fexit;
}
if (chkCmd(1, farg[0], "_OS$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 0) {cerr = 3; goto fexit;}
    copyStr(OSVER, outbuf);
    goto fexit;
}
