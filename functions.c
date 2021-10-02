if (chkCmdPtr[0] == '_') goto _func;
if (chkCmd(1, "CHR$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    outbuf[0] = (char)atoi(farg[1]);
    outbuf[1] = 0;
    goto fexit;
}
if (chkCmd(1, "ASC")) {
    cerr = 0;
    ftype = 2;
    int32_t pos = 0;
    if (fargct != 1 && fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    if (fargct == 2) {
        if (fargt[2] != 2) {cerr = 2; goto fexit;}
        pos = atoi(farg[2]);
        if (pos < 0) {cerr = 16; goto fexit;}
        if (pos > (int32_t)strlen(farg[1])) pos = (int32_t)strlen(farg[1]);
    }
    sprintf(outbuf, "%u", farg[1][pos]);
    goto fexit;
}
if (chkCmd(1, "CHRAT$")) {
    cerr = 0;
    ftype = 1;
    int32_t pos = 0;
    if (fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[2] != 2) {cerr = 2; goto fexit;}
    pos = atoi(farg[2]);
    if (pos < 0) {cerr = 16; goto fexit;}
    if (pos > (int32_t)strlen(farg[1])) pos = strlen(farg[1]);
    sprintf(outbuf, "%c", farg[1][pos]);
    goto fexit;
}
if (chkCmd(2, "RND", "RAND")) {
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
if (chkCmd(1, "TIMERUS")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)timer());
    goto fexit;
}
if (chkCmd(1, "TIMERMS")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)timer() / 1000);
    goto fexit;
}
if (chkCmd(1, "TIMER")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)timer() / 1000000);
    goto fexit;
}
if (chkCmd(1, "TIMEUS")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)usTime());
    goto fexit;
}
if (chkCmd(1, "TIMEMS")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)usTime() / 1000);
    goto fexit;
}
if (chkCmd(1, "TIME")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;} 
    sprintf(outbuf, "%llu", (long long unsigned)usTime() / 1000000);
    goto fexit;
}
if (chkCmd(1, "SH")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;} 
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    #ifndef _WIN_NO_VT
    if (sh_clearAttrib) fputs("\e[0m", stdout);
    #else
    if (sh_clearAttrib) SetConsoleTextAttribute(hConsole, ocAttrib);
    #endif
    farg[1] = realloc(farg[1], strlen(farg[1]) + 6); copyStrApnd(" 2>&1", farg[1]);
    #ifndef _WIN32
    if (sh_silent) {farg[1] = realloc(farg[1], strlen(farg[1]) + 13); copyStrApnd(" &>/dev/null", farg[1]);}
    #else
    if (sh_silent) {farg[1] = realloc(farg[1], strlen(farg[1]) + 13); copyStrApnd(" 1>nul 2>nul", farg[1]);}
    #endif
    int duperr;
    duperr = dup(2);
    close(2);
    sprintf(outbuf, "%d", (retval = system(farg[1])));
    dup2(duperr, 2);
    close(duperr);
    if (sh_restoreAttrib) updateTxtAttrib();
    goto fexit;
}
if (chkCmd(1, "EXEC")) {
    cerr = 0;
    ftype = 2;
    if (fargct < 1) {cerr = 3; goto fexit;} 
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    #ifndef _WIN_NO_VT
    if (sh_clearAttrib) fputs("\e[0m", stdout);
    #else
    if (sh_clearAttrib) SetConsoleTextAttribute(hConsole, ocAttrib);
    #endif
    #ifndef _WIN32
    char** runargs = (char**)malloc((fargct + 1) * sizeof(char*));
    runargs[0] = farg[1];
    int argno = 1;
    for (; argno < fargct; argno++) {
        runargs[argno] = farg[argno + 1];
    }
    runargs[argno] = NULL;
    int status;
    int stdout_dup = 0, stderr_dup = 0, fd = 0;
    if (sh_silent) {
        stdout_dup = dup(1);
        stderr_dup = dup(2);
        fd = open("/dev/null", O_WRONLY | O_CREAT);
        dup2(fd, 1);
        dup2(fd, 2);
    }
    pid_t pid = fork();
    if (pid < 0) cerr = -1;
    if (pid == 0) {
        execvp(runargs[0], runargs);
        exit(127);
    }
    else if (pid > 0) {
        while (wait(&status) != pid) {}
        sprintf(outbuf, "%d", (retval = WEXITSTATUS(status)));
    }
    else if (sh_silent) {
        dup2(stdout_dup, 1);
        dup2(stderr_dup, 2);
        close(fd);
    }
    free(runargs);
    #else
    char* tmpcmd = malloc(CB_BUF_SIZE);
    tmpcmd[0] = 0;
    bool winecho = false;
    for (int argno = 1; argno <= fargct; argno++) {
        bool nq = winArgNeedsQuotes(farg[argno]);
        if (argno == 1) {
            upCase(farg[argno]);
            winecho = !strcmp(farg[argno], "ECHO");
        }
        if (nq && !winecho) copyStrApnd(" \"", tmpcmd);
        copyStrApnd(farg[argno], tmpcmd);
        if (nq && !winecho) strApndChar(tmpcmd, '"');
    }
    int stdout_dup = 0, stderr_dup = 0;
    if (sh_silent) {
        stdout_dup = dup(1);
        stderr_dup = dup(2);
        int fd = open("NUL", _O_WRONLY | _O_CREAT);
        dup2(fd, 1);
        dup2(fd, 2);
    }
    retval = WEXITSTATUS(system(tmpcmd));
    if (sh_silent) {
        dup2(stdout_dup, 1);
        dup2(stderr_dup, 2);
    }
    free(tmpcmd);
    #endif
    if (sh_restoreAttrib) updateTxtAttrib();
    goto fexit;
}
if (chkCmd(1, "SH$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;} 
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    farg[1] = realloc(farg[1], strlen(farg[1]) + 6); copyStrApnd(" 2>&1", farg[1]);
    int duperr;
    duperr = dup(2);
    close(2);
    outbuf[0] = 0;
    FILE* p = popen(farg[1], "r");
    if (p) {
        outbuf[fread(outbuf, 1, CB_BUF_SIZE - 1, p)] = 0;
        retval = WEXITSTATUS(pclose(p));
    }
    dup2(duperr, 2);
    close(duperr);
    goto fexit;
}
if (chkCmd(1, "EXEC$")) {
    cerr = 0;
    ftype = 1;
    if (fargct < 1) {cerr = 3; goto fexit;} 
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    #ifndef _WIN32
    char** runargs = (char**)malloc((fargct + 1) * sizeof(char*));
    runargs[0] = farg[1];
    int argno = 1;
    for (; argno < fargct; argno++) {
        runargs[argno] = farg[argno + 1];
    }
    runargs[argno] = NULL;
    int stdout_dup = 0, stderr_dup = 0, fd[2];
    if (pipe(fd) == -1) {cerr = -1; goto fexit;}
    stdout_dup = dup(1);
    stderr_dup = dup(2);
    dup2(fd[1], 1);
    dup2(fd[1], 2);
    pid_t pid = fork();
    if (pid < 0) cerr = -1;
    else if (pid == 0) {
        execvp(runargs[0], runargs);
        putchar(0);
        exit(127);
    }
    else if (pid > 0) {
        while (wait(&retval) != pid) {}
        retval = WEXITSTATUS(retval);
        outbuf[read(fd[0], outbuf, CB_BUF_SIZE - 1)] = 0;
    }
    dup2(stdout_dup, 1);
    dup2(stderr_dup, 2);
    close(fd[0]);
    close(fd[1]);
    free(runargs);
    #else
    char* tmpcmd = malloc(CB_BUF_SIZE);
    tmpcmd[0] = 0;
    bool winecho = false;
    for (int argno = 1; argno <= fargct; argno++) {
        bool nq = winArgNeedsQuotes(farg[argno]);
        if (argno == 1) {
            upCase(farg[argno]);
            winecho = !strcmp(farg[argno], "ECHO");
        }
        if (nq && !winecho) copyStrApnd(" \"", tmpcmd);
        copyStrApnd(farg[argno], tmpcmd);
        if (nq && !winecho) strApndChar(tmpcmd, '"');
    }
    int duperr;
    duperr = dup(2);
    close(2);
    outbuf[0] = 0;
    FILE* p = popen(tmpcmd, "r");
    if (p) {
        outbuf[fread(outbuf, 1, CB_BUF_SIZE, p)] = 0;
        retval = WEXITSTATUS(pclose(p));
    }
    dup2(duperr, 2);
    close(duperr);
    free(tmpcmd);
    #endif
    goto fexit;
}
if (chkCmd(1, "CINT")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    sprintf(outbuf, "%d", (int)round(atof(farg[1])));
    goto fexit;
}
if (chkCmd(1, "INT")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%d", (int)dbl);
    int32_t i;
    for (i = 0; outbuf[i] != '.' && outbuf[i]; i++) {}
    outbuf[i] = 0;
    goto fexit;
}
if (chkCmd(1, "VAL")) {
    cerr = 0;
    ftype = 2;
    if (fargct < 1 || fargct > 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    if (fargct == 2 && fargt[2] != 2) {cerr = 2; goto fexit;}
    double dbl;
    if (fargct == 1) {
        sscanf(farg[1], "%lf", &dbl);
        sprintf(outbuf, "%lf", dbl);
        goto fexit;
    }
    int act = 0;
    uint64_t num;
    char* tmpstr = NULL;
    int32_t tmplen = 0;
    int32_t tmppos = 0;
    if (fargct == 2) act = atoi(farg[2]);
    switch (act) {
        case 0:
            tmplen = strlen(farg[1]);
            for (int32_t i = 0; farg[1][i] == '0' && i < tmplen; i++) {
                tmppos++;
            }
            tmpstr = (char*)malloc(tmplen - tmppos + 1);
            copyStrSnip(farg[1], tmppos, tmplen, tmpstr);
            if (getType(farg[1]) != 2) {
                outbuf[0] = '0';
                outbuf[1] = 0;
                break;
            }
            sscanf(tmpstr, "%lf", &dbl);
            free(tmpstr);
            sprintf(outbuf, "%lf", dbl);
            break;
        case 1:
            sscanf(farg[1], "%llx", (long long unsigned int*)&num);
            sprintf(outbuf, "%llu", (long long unsigned int)num);
            break;
        case 2:
            sscanf(farg[1], "%llo", (long long unsigned int*)&num);
            sprintf(outbuf, "%llu", (long long unsigned int)num);
            break;
        default:
            sscanf(farg[1], "%lf", &dbl);
            sprintf(outbuf, "%lf", dbl);
            break;
    }
    goto fexit;
}
if (chkCmd(1, "STR$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    copyStr(farg[1], outbuf);
    goto fexit;
}
if (chkCmd(1, "MOD")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 2 || fargt[2] != 2) {cerr = 2; goto fexit;}
    double dbl1, dbl2;
    sscanf(farg[1], "%lf", &dbl1);
    sscanf(farg[2], "%lf", &dbl2);
    sprintf(outbuf, "%lf", fmod(dbl1, dbl2));
    goto fexit;
}
if (chkCmd(1, "PI")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    strcpy(outbuf, "3.141593");
    goto fexit;
}
if (chkCmd(1, "SIN")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%lf", sin(dbl));
    goto fexit;
}
if (chkCmd(1, "COS")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%lf", cos(dbl));
    goto fexit;
}
if (chkCmd(1, "TAN")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%lf", tan(dbl));
    goto fexit;
}
if (chkCmd(1, "SINH")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    dbl = sinh(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%lf", dbl);
    goto fexit;
}
if (chkCmd(1, "COSH")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    dbl = cosh(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%lf", dbl);
    goto fexit;
}
if (chkCmd(1, "TANH")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%lf", tanh(dbl));
    goto fexit;
}
if (chkCmd(1, "LOG")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    dbl = log(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%lf", dbl);
    goto fexit;
}
if (chkCmd(1, "LOG10")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    dbl = log10(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%lf", dbl);
    goto fexit;
}
if (chkCmd(1, "EXP")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    dbl = exp(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%lf", dbl);
    goto fexit;
}
if (chkCmd(1, "INKEY$")) {
    cerr = 0;
    ftype = 1;
    if (fargct) {cerr = 3; goto fexit;}
    #ifndef _WIN32
    int tmp;
    if (!(tmp = kbhit())) goto fexit;
    int obp = 0;
    while (obp < tmp) {
        outbuf[obp] = 0;
        outbuf[obp] = getchar();
        if (outbuf[obp] == 0) {break;}
        obp++;
    }
    outbuf[obp] = 0;
    #else
    updatechars();
    copyStr(kbinbuf, outbuf);
    kbinbuf[0] = 0;
    #endif
    goto fexit;
}
if (chkCmd(1, "UCASE$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    upCase(farg[1]);
    copyStr(farg[1], outbuf);
    goto fexit;
}
if (chkCmd(1, "LCASE$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    lowCase(farg[1]);
    copyStr(farg[1], outbuf);
    goto fexit;
}
if (chkCmd(1, "LEN")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    sprintf(outbuf, "%lu", (long unsigned)strlen(farg[1]));
    goto fexit;
}
if (chkCmd(1, "SNIP$")) {
    cerr = 0;
    ftype = 1;
    if (fargct < 2 || fargct > 3) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    int32_t start, end;
    if (fargct == 2) {
        if (fargt[2] == 1) {cerr = 2; goto fexit;}
        if (fargt[2] == 0) {cerr = 3; goto fexit;}
        start = 0;
        end = atoi(farg[2]);
        if (end < 0) {cerr = 16; goto fexit;}
    } else {
        if (fargt[2] == 1 || fargt[3] == 1) {cerr = 2; goto fexit;}
        if (fargt[2] + fargt[3]) {
            if (!fargt[2]) {start = 0;}
            else {start = atoi(farg[2]); if (start < 0) {cerr = 16; goto fexit;}}
            if (!fargt[3]) {end = 32;}
            else {end = atoi(farg[3]); if (end < 0 || end < start) {cerr = 16; goto fexit;}}
        } else {
            cerr = 3;
            goto fexit;
        }
    }
    copyStrSnip(farg[1], start, end, outbuf);
}
if (chkCmd(1, "CURX")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    getCurPos();
    sprintf(outbuf, "%d", curx);
    goto fexit;
}
if (chkCmd(1, "CURY")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    getCurPos();
    sprintf(outbuf, "%d", cury);
    goto fexit;
}
if (chkCmd(1, "HEX$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%llx", (long long unsigned int)dbl);
    upCase(outbuf);
    goto fexit;
}
if (chkCmd(1, "OCT$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    double dbl;
    sscanf(farg[1], "%lf", &dbl);
    sprintf(outbuf, "%llo", (long long unsigned int)dbl);
    upCase(outbuf);
    goto fexit;
}
if (chkCmd(1, "RGB")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 3) {cerr = 3; goto fexit;}
    if (fargt[1] != 2 || fargt[2] != 2 || fargt[3] != 2) {cerr = 2; goto fexit;}
    uint8_t r, g, b;
    int tmpv;
    if ((tmpv = atoi(farg[1])) < 0 || tmpv > 255) {cerr = 16; goto fexit;}
    r = tmpv;
    if ((tmpv = atoi(farg[2])) < 0 || tmpv > 255) {cerr = 16; goto fexit;}
    g = tmpv;
    if ((tmpv = atoi(farg[3])) < 0 || tmpv > 255) {cerr = 16; goto fexit;}
    b = tmpv;
    sprintf(outbuf, "%u", (r << 16) | (g << 8) | b);
    goto fexit;
}
if (chkCmd(1, "LIMIT")) {
    cerr = 0;
    ftype = 2;
    double num = 0;
    if (fargct < 2 || fargct > 3) {cerr = 3; goto fexit;}
    if (fargt[1] != 2 || fargt[2] != 2) {cerr = 2; goto fexit;}
    if (fargct == 3 && fargt[3] == 1) {cerr = 2; goto fexit;}
    num = atof(farg[1]);
    if (fargct == 2) {
        double max = atof(farg[2]);
        if (num > max) {num = max;}
    } else if (fargct == 3 && fargt[3] == 0) {
        double min = atof(farg[2]);
        if (num < min) {num = min;}
    } else if (fargct == 3) {
        double min = atof(farg[2]);
        double max = atof(farg[3]);
        if (num > max) {num = max;}
        else if (num < min) {num = min;}
    } else {
        cerr = 3;
        goto fexit;
    }
    sprintf(outbuf, "%lf", num);
    goto fexit;
}
if (chkCmd(1, "PAD$")) {
    cerr = 0;
    ftype = 1;
    if (fargct < 2 || fargct > 3) {cerr = 3; goto fexit;}
    if (fargt[1] == 0) {cerr = 3; goto fexit;}
    if (fargt[2] != 2) {cerr = 2; goto fexit;}
    char tmpc;
    if (fargct == 3) {
        if (fargt[3] != 1) {cerr = 2; goto fexit;}
        if (flen[3] != 1) {cerr = 16; goto fexit;}
        tmpc = farg[3][0];
    } else {
        if (fargt[1] == 1) tmpc = ' ';
        else tmpc = '0';
    }
    int32_t pad = atoi(farg[2]) - flen[1];
    int32_t i;
    for (i = 0; i < pad; ++i) {
        outbuf[i] = tmpc;
    }
    outbuf[i] = 0;
    copyStrApnd(farg[1], outbuf);
    goto fexit;
}
if (chkCmd(1, "BASENAME$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    copyStr(basefilename(farg[1]), outbuf);
    goto fexit;
}
if (chkCmd(1, "DIRNAME$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    copyStr(pathfilename(farg[1]), outbuf);
    goto fexit;
}
if (chkCmd(1, "WIDTH")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    #ifdef __unix__
    struct winsize max;
    ioctl(0, TIOCGWINSZ , &max);
    sprintf(outbuf, "%d", max.ws_col);
    #else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int tmpret;
    tmpret = GetConsoleScreenBufferInfo(hConsole, &csbi);
    (void)tmpret;
    sprintf(outbuf, "%d", csbi.srWindow.Right - csbi.srWindow.Left + 1);
    #endif
    goto fexit;
}
if (chkCmd(1, "HEIGHT")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    #ifdef __unix__
    struct winsize max;
    ioctl(0, TIOCGWINSZ , &max);
    sprintf(outbuf, "%d", max.ws_row);
    #else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int tmpret;
    tmpret = GetConsoleScreenBufferInfo(hConsole, &csbi);
    (void)tmpret;
    sprintf(outbuf, "%d", csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    #endif
    goto fexit;
}
if (chkCmd(1, "FGC")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    if (txt_truecolor) sprintf(outbuf, "%u", truefgc);
    else sprintf(outbuf, "%u", fgc);
    goto fexit;
}
if (chkCmd(1, "BGC")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    if (txt_truecolor) sprintf(outbuf, "%u", truebgc);
    else sprintf(outbuf, "%u", bgc);
    goto fexit;
}
if (chkCmd(1, "TRUECOLOR")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (int)txt_truecolor);
    goto fexit;
}
if (chkCmd(1, "INPUT$")) {
    cerr = 0;
    ftype = 1;
    if (fargct > 1) {cerr = 3; goto fexit;}
    if (fargct == 1 && fargt[1] != 1) {cerr = 2; goto fexit;}
    if (fargct != 1) {
        farg[1] = malloc(4);
        strcpy(farg[1], "?: ");
    }
    char* tmp = NULL;
    #ifndef _WIN32
    getCurPos();
    curx--;
    farg[1] = realloc(farg[1], strlen(farg[1]) + curx);
    int32_t ptr = strlen(farg[1]);
    while (curx) {farg[1][ptr] = 22; ptr++; curx--;}
    farg[1][ptr] = 0;
    #endif
    #ifndef _WIN32
    __typeof__(rl_getc_function) old_rl_getc_function = rl_getc_function;
    rl_getc_function = getc;
    #endif
    tmp = readline(farg[1]);
    #ifndef _WIN32
    rl_getc_function = old_rl_getc_function;
    #endif
    if (tmp != NULL) {
        copyStr(tmp, outbuf);
        free(tmp);
    #ifdef _WIN32
    } else {
        putchar('\n');
    #endif
    }
    if (fargct != 1) free(farg[1]);
    goto fexit;
}
if (chkCmd(1, "LINES")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    int cl = 1;
    if (!farg[1][0]) {strcpy(outbuf, "0"); goto fexit;}
    for (int32_t i = 0; farg[1][i]; i++) {
        if (farg[1][i] == '\n') {
            cl++;
        }
    }
    sprintf(outbuf, "%d", cl);
    goto fexit;
}
if (chkCmd(1, "LINE$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 2 || fargt[2] != 1) {cerr = 2; goto fexit;}
    int cl = 0, tl = atoi(farg[1]);
    if (tl < 0) {cerr = 16; goto fexit;}
    for (int32_t i = 0; farg[2][i]; i++) {
        if (farg[2][i] == '\n') {
            cl++;
        } else if (cl == tl) {
            strApndChar(outbuf, farg[2][i]);
        }
    }
    if (outbuf[strlen(outbuf) - 1] == '\r') outbuf[strlen(outbuf) - 1] = 0;
    goto fexit;
}
if (chkCmd(1, "DATE")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1 || fargt[1] == 0) {cerr = 3; goto fexit;}
    int element = -1;
    if (fargt[1] == 2) {
        element = atoi(farg[1]);
    } else {
        upCase(farg[1]);
        chkCmdPtr = farg[1];
        if (chkCmd(2, "SEC", "SECOND")) {element = 0;}
        else if (chkCmd(2, "MIN", "MINUTE")) {element = 1;}
        else if (chkCmd(2, "HR", "HOUR")) {element = 2;}
        else if (chkCmd(1, "DAY")) {element = 3;}
        else if (chkCmd(2, "MON", "MONTH")) {element = 4;}
        else if (chkCmd(1, "YEAR")) {element = 5;}
        else if (chkCmd(2, "WDAY", "WEEKDAY")) {element = 6;}
        else if (chkCmd(2, "YDAY", "YEARDAY")) {element = 7;}
        else if (chkCmd(3, "DST", "DAYLIGHT", "DAYLIGHTSAVING")) {element = 8;}
    }
    if (element > 8 || element < 0) {cerr = 16; goto fexit;}
    time_t rawtime;
    time(&rawtime);
    struct tm* timeinfo;
    timeinfo = localtime(&rawtime);
    int edata = -1;
    switch (element) {
        case 0:
            edata = timeinfo->tm_sec;
            break;
        case 1:
            edata = timeinfo->tm_min;
            break;
        case 2:
            edata = timeinfo->tm_hour;
            break;
        case 3:
            edata = timeinfo->tm_mday;
            break;
        case 4:
            edata = timeinfo->tm_mon;
            break;
        case 5:
            edata = 1900 + timeinfo->tm_year;
            break;
        case 6:
            edata = timeinfo->tm_wday;
            break;
        case 7:
            edata = timeinfo->tm_yday;
            break;
        case 8:
            edata = timeinfo->tm_isdst;
            break;
    }
    sprintf(outbuf, "%d", edata);
    goto fexit;
}
if (chkCmd(1, "CWD$")) {
    cerr = 0;
    ftype = 1;
    if (fargct) {cerr = 3; goto fexit;}
    getcwd(outbuf, CB_BUF_SIZE);
    goto fexit;
}
if (chkCmd(1, "FILES$")) {
    cerr = 0;
    fileerror = 0;
    ftype = 1;
    if (fargct > 1) {cerr = 3; goto fexit;}
    char* olddn = NULL;
    if (fargct) {
        if (fargt[1] != 1) {cerr = 2; goto fexit;}
        olddn = malloc(CB_BUF_SIZE);
        getcwd(olddn, CB_BUF_SIZE);
        if (chdir(farg[1])) {
            fileerror = errno;
            free(olddn);
            outbuf[0] = 0;
            goto fexit;
        }
    }
    DIR* cwd = opendir(".");
    if (!cwd) {if (fargct) {chdir(olddn); free(olddn);} outbuf[0] = 0; goto fexit;}
    struct dirent* dir;
    #ifdef _WIN32
        #define FSC '\\'
        strcpy(outbuf, ".\\\n..\\");
    #else
        #define FSC '/'
        strcpy(outbuf, "./\n../");
    #endif
    struct stat pathstat;
    while ((dir = readdir(cwd))) {
        stat(dir->d_name, &pathstat);
        if (S_ISDIR(pathstat.st_mode) && strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
            strApndChar(outbuf, '\n');
            copyStrApnd(dir->d_name, outbuf);
            strApndChar(outbuf, FSC);
        }
    }
    closedir(cwd);
    cwd = opendir(".");
    while ((dir = readdir(cwd))) {
        stat(dir->d_name, &pathstat);
        if (!(S_ISDIR(pathstat.st_mode))) {
            strApndChar(outbuf, '\n');
            copyStrApnd(dir->d_name, outbuf);
        }
    }
    if (fargct) {
        chdir(olddn);
        free(olddn);
    }
    closedir(cwd);
    goto fexit;
}
if (chkCmd(2, "CD", "CHDIR")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    outbuf[0] = '0' + !chdir(farg[1]);
    outbuf[1] = 0;
    goto fexit;
}
if (chkCmd(1, "FOPEN")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 1 || fargt[2] != 1) {cerr = 2; goto fexit;}
    if (!isFile(farg[1])) {
        outbuf[0] = '-';
        outbuf[1] = '1';
        outbuf[2] = 0;
        fileerror = EINVAL;
        goto fexit;
    }
    sprintf(outbuf, "%d", openFile(farg[1], farg[2]));
    goto fexit;
}
if (chkCmd(1, "FCLOSE")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    outbuf[0] = '0' + closeFile(atoi(farg[1]));
    outbuf[1] = 0;
    goto fexit;
}
if (chkCmd(1, "FSIZE")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    int fnum = atoi(farg[1]);
    if (fnum < 0 || fnum >= filemaxct) {
        outbuf[0] = '-';
        outbuf[1] = '1';
        outbuf[2] = 0;
        fileerror = EINVAL;
        goto fexit;
    }
    sprintf(outbuf, "%d", filedata[fnum].size);
    goto fexit;
}
if (chkCmd(1, "EOF")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    int fnum = atoi(farg[1]);
    if (fnum < 0 || fnum >= filemaxct) {
        outbuf[0] = '-';
        outbuf[1] = '1';
        outbuf[2] = 0;
        fileerror = EINVAL;
        goto fexit;
    }
    int32_t prev = ftell(filedata[fnum].fptr);
    fgetc(filedata[fnum].fptr);
    errno = 0;
    outbuf[0] = '0' + (feof(filedata[fnum].fptr) != 0);
    fileerror = errno;
    fseek(filedata[fnum].fptr, prev, SEEK_SET);
    outbuf[1] = 0;
    goto fexit;
}
if (chkCmd(1, "EOFD")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    int fnum = atoi(farg[1]);
    if (fnum < 0 || fnum >= filemaxct) {
        outbuf[0] = '-';
        outbuf[1] = '1';
        outbuf[2] = 0;
        fileerror = EINVAL;
        goto fexit;
    }
    errno = 0;
    outbuf[0] = '0' + (ftell(filedata[fnum].fptr) >= filedata[fnum].size);
    outbuf[1] = 0;
    fileerror = errno;
    goto fexit;
}
if (chkCmd(1, "FREAD$")) {
    cerr = 0;
    fileerror = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    int fnum = atoi(farg[1]);
    outbuf[1] = 0;
    if (fnum < 0 || fnum >= filemaxct) {
        outbuf[0] = 0;
        fileerror = EINVAL;
        goto fexit;
    }
    if (feof(filedata[fnum].fptr)) {
        outbuf[0] = 0;
    } else {
        errno = 0;
        int c = fgetc(filedata[fnum].fptr);
        outbuf[0] = (c < 0) ? 0 : c;
        fileerror = errno;
    }
    goto fexit;
}
if (chkCmd(1, "FREAD")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    int fnum = atoi(farg[1]);
    int fc = -1;
    if (fnum < 0 || fnum >= filemaxct) {
        fileerror = EINVAL;
    } else if (!feof(filedata[fnum].fptr)) {
        errno = 0;
        fc = fgetc(filedata[fnum].fptr);
        if (fc < 0) fc = -1;
        fileerror = errno;
    }
    sprintf(outbuf, "%d", fc);
    goto fexit;
}
if (chkCmd(1, "FWRITE")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 2 || fargt[2] != 1) {cerr = 2; goto fexit;}
    int fnum = atoi(farg[1]);
    int32_t ret = -1;
    if (fnum < 0 || fnum >= filemaxct) {
        fileerror = EINVAL;
    } else {
        errno = 0;
        ret = (fputs(farg[2], filedata[fnum].fptr) != EOF);
        fileerror = errno;
    }
    sprintf(outbuf, "%d", ret);
    goto fexit;
}
if (chkCmd(1, "FSEEK")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 2 || fargt[2] != 2) {cerr = 2; goto fexit;}
    int fnum = atoi(farg[1]);
    int32_t ret = 2;
    if (fnum < 0 || fnum >= filemaxct) {
        fileerror = EINVAL;
    } else {
        errno = 0;
        int32_t pos = atoi(farg[2]);
        if (pos < 0) {
            fileerror = EINVAL;
        } else {
            ret = !fseek(filedata[fnum].fptr, (pos > filedata[fnum].size) ? filedata[fnum].size : pos, SEEK_SET);
            fileerror = errno;
        }
    }
    sprintf(outbuf, "%d", ret);
    goto fexit;
}
if (chkCmd(1, "FLUSH")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    int fnum = atoi(farg[1]);
    if (fnum < 0 || fnum >= filemaxct) {
        outbuf[0] = '-';
        outbuf[1] = '1';
        outbuf[2] = 0;
        fileerror = EINVAL;
        goto fexit;
    }
    errno = 0;
    outbuf[0] = (fflush(filedata[fnum].fptr) != EOF);
    outbuf[1] = 0;
    fileerror = errno;
    goto fexit;
}
if (chkCmd(2, "MD", "MKDIR")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    errno = 0;
    #ifndef _WIN32
    outbuf[0] = '0' + !mkdir(farg[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    #else
    outbuf[0] = '0' + !mkdir(farg[1]);
    #endif
    outbuf[1] = 0;
    fileerror = errno;
    goto fexit;
}
if (chkCmd(2, "RM", "REMOVE")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    outbuf[0] = '0' + cbrm(farg[1]);
    outbuf[1] = 0;
    goto fexit;
}
if (chkCmd(4, "MV", "MOVE", "REN", "RENAME")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 1 || fargt[2] != 1) {cerr = 2; goto fexit;}
    errno = 0;
    outbuf[0] = '0' + !rename(farg[1], farg[2]);
    outbuf[1] = 0;
    fileerror = errno;
    goto fexit;
}
if (chkCmd(1, "ISFILE")) {
    cerr = 0;
    fileerror = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    struct stat pathstat;
    if (stat(farg[1], &pathstat)) {
        outbuf[0] = '-';
        outbuf[1] = '1';
        outbuf[2] = 0;
        fileerror = errno;
        goto fexit;
    }
    outbuf[0] = '0' + !(S_ISDIR(pathstat.st_mode));
    outbuf[1] = 0;
    goto fexit;
}
goto fexit;
_func:
if (chkCmd(1, "_HOME$")) {
    cerr = 0;
    ftype = 1;
    if (fargct) {cerr = 3; goto fexit;}
    copyStr(gethome(), outbuf);
    goto fexit;
}
if (chkCmd(1, "_ENV$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    char* tmpenv = getenv(farg[1]);
    if (tmpenv) {
        copyStr(tmpenv, outbuf);
    }
    goto fexit;
}
if (chkCmd(1, "_ENVSET")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    char* tmpenv = getenv(farg[1]);
    outbuf[0] = '0' + (tmpenv != NULL);
    outbuf[1] = 0;
    goto fexit;
}
if (chkCmd(1, "_RET")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", retval);
    goto fexit;
}
if (chkCmd(1, "_ERRNOSTR$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    copyStr(strerror(atoi(farg[1])), outbuf);
    goto fexit;
}
if (chkCmd(1, "_FILEERROR")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", fileerror);
    goto fexit;
}
if (chkCmd(1, "_PROMPT$")) {
    cerr = 0;
    ftype = 1;
    if (fargct) {cerr = 3; goto fexit;}
    int tmpt = getVal(prompt, outbuf);
    if (tmpt != 1) strcpy(outbuf, "CLIBASIC> ");
    goto fexit;
}
if (chkCmd(1, "_TXTLOCK")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (int)textlock);
    goto fexit;
}
if (chkCmd(1, "_VER$")) {
    cerr = 0;
    ftype = 1;
    if (fargct) {cerr = 3; goto fexit;}
    copyStr(VER, outbuf);
    goto fexit;
}
if (chkCmd(1, "_BITS$")) {
    cerr = 0;
    ftype = 1;
    if (fargct) {cerr = 3; goto fexit;}
    copyStr(BVER, outbuf);
    goto fexit;
}
if (chkCmd(1, "_OS$")) {
    cerr = 0;
    ftype = 1;
    if (fargct) {cerr = 3; goto fexit;}
    copyStr(OSVER, outbuf);
    goto fexit;
}
if (chkCmd(1, "_VT")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    #ifdef _WIN_NO_VT
    outbuf[0] = '0';
    #else
    outbuf[0] = '1';
    #endif
    outbuf[1] = 0;
    goto fexit;
}
if (chkCmd(1, "_STARTCMD$")) {
    cerr = 0;
    ftype = 1;
    if (fargct) {cerr = 3; goto fexit;}
    copyStr(startcmd, outbuf);
    goto fexit;
}
if (chkCmd(1, "_ARG$")) {
    if (!inProg) {cerr = 125; goto fexit;}
    cerr = 0;
    ftype = 1;
    if (fargct == 0) {
        for (int i = 1; i < progargc; i++) {
            copyStrApnd(progargs[i], outbuf);
            if (i != progargc - 1) strApndChar(outbuf, ' ');
        }
    } else if (fargct == 1) {
        if (fargt[1] != 2) {cerr = 2; goto fexit;}
        int i = atoi(farg[1]);
        if (i < 0) {cerr = 16; goto fexit;}
        if (i > progargc - 1 && i > 0) {outbuf[0] = 0; goto fexit;}
        if (i == 0) copyStr(progfn[progindex], outbuf);
        else copyStr(progargs[i], outbuf);
    } else {
        cerr = 3; goto fexit;
    }
}
if (chkCmd(1, "_ARGC")) {
    if (!inProg) {cerr = 125; goto fexit;}
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (progargc > 0) ? progargc - 1 : progargc);
    goto fexit;
}
