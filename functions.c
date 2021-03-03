if (!strcmp(farg[0], "CHR$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    outbuf[0] = (char)atoi(farg[1]);
    outbuf[1] = 0;
    goto fexit;
}
if (!strcmp(farg[0], "ASC")) {
    cerr = 0;
    ftype = 2;
    int pos = 0;
    if (fargct != 1 && fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    if (fargct == 2) {
        if (fargt[2] != 2) {cerr = 2; goto fexit;}
        pos = (int)atoi(farg[2]);
        if (pos < 0) pos = 0;
        if (pos > flen[2] + 1) pos = flen[2] + 1;
    }
    sprintf(outbuf, "%d", farg[1][pos]);
    goto fexit;
}
if (!strcmp(farg[0], "RND") || !strcmp(farg[0], "RAND")) {
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
    goto fexit;
}
if (!strcmp(farg[0], "INT")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%d", (int)dbl);
    int i;
    for (i = 0; outbuf[i] != '.' && outbuf[i] != '\0'; i++) {}
    outbuf[i] = 0;
    goto fexit;
}
if (!strcmp(farg[0], "CINT")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    dbl = round(dbl);
    sprintf(outbuf, "%d", (int)dbl);
    goto fexit;
}
if (!strcmp(farg[0], "STR$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    copyStr(farg[1], outbuf);
    goto fexit;
}
if (!strcmp(farg[0], "VAL")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%lf", dbl);
    ftype = getVal(outbuf, outbuf);
    if (ftype == 1) {cerr = 2; goto fexit;}
    goto fexit;
}
if (!strcmp(farg[0], "FGC")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (int)fgc);
    goto fexit;
}
if (!strcmp(farg[0], "BGC")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 0) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (int)fgc);
    goto fexit;
}