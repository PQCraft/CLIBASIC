cb_funcret extfr = {127, 0};
for (int i = extmaxct - 1; i > -1; --i) {
    if (extdata[i].inuse && extdata[i].runfunc) {
        if (skipfargsolve && extsas != i) {
            skipfargsolve = false;
            for (int j = 1; j <= fargct; ++j) {
                fargt[j] = getVal(farg[j], farg[j]);
                if (fargt[j] == 0) goto fnoerrscan;
                if (fargt[j] == 255) fargt[j] = 0;
                flen[j] = strlen(farg[j]);
                farg[j] = realloc(farg[j], flen[j] + 1);
            }
        }
        extfr = extdata[i].runfunc(fargct, farg, fargt, flen, outbuf);
        if (extfr.cerr != 127) {
            cerr = extfr.cerr;
            ftype = extfr.ftype;
            goto fexit;
        }
    }
}
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
    long double min = 0;
    long double max;
    if (fargct == 1) {
        if (fargt[1] != 2) {cerr = 2; goto fexit;}
        sscanf(farg[1], "%Lf", &max);
    } else if (fargct == 2) {
        if (fargt[1] + fargt[2] != 4) {cerr = 2; goto fexit;}
        sscanf(farg[1], "%Lf", &min);
        sscanf(farg[2], "%Lf", &max);
    } else {
        cerr = 3;
        goto fexit;
    }
    sprintf(outbuf, "%Lf", randNum(min, max));
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
    if (esc && sh_clearAttrib) fputs("\e[0m", stdout);
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
if (chkCmd(2, "EXEC", "EXECA")) {
    cerr = 0;
    ftype = 2;
    if (fargct < 1) {cerr = 3; goto fexit;} 
    bool execa = false;
    char** tmpfarg = NULL;
    int tmpfargct = 0;
    if (farg[0][4] == 'A') {
        if (fargct != 1) {cerr = 3; goto fexit;}
        execa = true;
        int v = -1;
        for (register int i = 0; i < varmaxct; ++i) {
            if (vardata[i].inuse && !strcmp(farg[1], vardata[i].name)) {v = i; break;}
        }
        if (v == -1 || vardata[v].size == -1) {cerr = 23; seterrstr(farg[1]); goto fexit;}
        if (vardata[v].type != 1) {cerr = 2; goto fexit;}
        tmpfarg = farg;
        tmpfargct = fargct;
        farg = vardata[v].data - 1;
        fargct = vardata[v].size + 1;
    } else {
        if (fargt[1] != 1) {cerr = 2; goto fexit;}
    }
    #ifndef _WIN_NO_VT
    if (esc && sh_clearAttrib) fputs("\e[0m", stdout);
    #else
    if (sh_clearAttrib) SetConsoleTextAttribute(hConsole, ocAttrib);
    #endif
    char** runargs = (char**)malloc((fargct + 1) * sizeof(char*));
    runargs[0] = farg[1];
    int argno = 1;
    for (; argno < fargct; argno++) {
        runargs[argno] = farg[argno + 1];
    }
    runargs[argno] = NULL;
    int stdout_dup = 0, stderr_dup = 0, fd = 0;
    if (sh_silent) {
        stdout_dup = dup(1);
        stderr_dup = dup(2);
        fd = open("/dev/null", O_WRONLY);
        dup2(fd, 1);
        dup2(fd, 2);
    }
    sprintf(outbuf, "%d", (retval = cb_exec(runargs)));
    if (sh_silent) {
        dup2(stdout_dup, 1);
        dup2(stderr_dup, 2);
        close(fd);
    }
    free(runargs);
    if (execa) {
        fargct = tmpfargct;
        farg = tmpfarg;
    }
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
if (chkCmd(2, "EXEC$", "EXECA$")) {
    cerr = 0;
    ftype = 1;
    if (fargct < 1) {cerr = 3; goto fexit;} 
    bool execa = false;
    char** tmpfarg = NULL;
    int tmpfargct = 0;
    if (farg[0][4] == 'A') {
        if (fargct != 1) {cerr = 3; goto fexit;}
        execa = true;
        int v = -1;
        for (register int i = 0; i < varmaxct; ++i) {
            if (vardata[i].inuse && !strcmp(farg[1], vardata[i].name)) {v = i; break;}
        }
        if (v == -1 || vardata[v].size == -1) {cerr = 23; seterrstr(farg[1]); goto fexit;}
        if (vardata[v].type != 1) {cerr = 2; goto fexit;}
        tmpfarg = farg;
        tmpfargct = fargct;
        farg = vardata[v].data - 1;
        fargct = vardata[v].size + 1;
    } else {
        if (fargt[1] != 1) {cerr = 2; goto fexit;}
    }
    char** runargs = (char**)malloc((fargct + 1) * sizeof(char*));
    runargs[0] = farg[1];
    int argno = 1;
    for (; argno < fargct; argno++) {
        runargs[argno] = farg[argno + 1];
    }
    runargs[argno] = NULL;
    int stdout_dup = 0, stderr_dup = 0, fd[2];
    #ifndef _WIN32
    int ret = pipe2(fd, O_NONBLOCK);
    (void)ret;
    #else
    _pipe(fd, CB_BUF_SIZE, _O_BINARY);
    #endif
    stdout_dup = dup(1);
    stderr_dup = dup(2);
    dup2(fd[1], 1);
    dup2(fd[1], 2);
    retval = cb_exec(runargs);
    *outbuf = 0;
    if (retval >= 0) {
        #ifndef _WIN32
        int inchar = 0;
        ioctl(fd[0], FIONREAD, &inchar);
        #else
        DWORD inchar = 0;
        PeekNamedPipe((HANDLE)_get_osfhandle(fd[0]), NULL, 0, NULL, &inchar, NULL);
        #endif
        if (inchar) outbuf[read(fd[0], outbuf, CB_BUF_SIZE - 1)] = 0;
    }
    dup2(stdout_dup, 1);
    dup2(stderr_dup, 2);
    close(fd[0]);
    close(fd[1]);
    free(runargs);
    if (execa) {
        fargct = tmpfargct;
        farg = tmpfarg;
    }
    goto fexit;
}
if (chkCmd(1, "CALLFUNC")) {
    if (fargct < 1) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 1;
    newprogargc = fargct;
    newprogargs = (char**)malloc((fargct + 1) * sizeof(char*));
    for (int i = 1; i < newprogargc; ++i) {
        newprogargs[i] = NULL;
    }
    for (int i = 2; i <= fargct; ++i) {
        newprogargs[i - 1] = malloc(CB_BUF_SIZE);
        if (!getVal(farg[i], newprogargs[i - 1])) {
            for (int j = i - 1; j > 0; --j) {
                free(newprogargs[j]);
            }
            free(newprogargs);
            goto fexit;
        }
        newprogargs[i - 1] = realloc(newprogargs[i - 1], strlen(newprogargs[i - 1]) + 1);
    }
    
    if (!loadSub(farg[1], true, &subinfo.type)) goto callfunc_err;
    subinfo.insub = true;
    ftype = subinfo.type;
    bool oip = inProg;
    inProg = true;
    int ptr1 = 0, ptr2 = 0;
    char* buf = (char*)malloc(CB_BUF_SIZE);
    while (1) {
        while (progbuf[progindex][ptr2] && progbuf[progindex][ptr2] != '\n') {++ptr2;}
        copyStrSnip(progbuf[progindex], ptr1, ptr2, buf);
        if ((cerr = runcmd(buf))) {hideerror = true; break;}
        if (!progbuf[progindex][ptr2]) break;
        if (!didloop) ++progLine;
        else didloop = false;
        ++ptr2;
        ptr1 = ptr2;
    }
    free(buf);
    inProg = oip;
    unloadProg();
    goto callfunc_noerr;
    callfunc_err:;
    for (int i = 1; i < newprogargc; ++i) {
        nfree(newprogargs[i]);
    }
    nfree(newprogargs);
    newprogargc = 0;
    goto fexit;
    callfunc_noerr:;
    if (!cerr) {
        if (funcret) {
            copyStr(funcret, outbuf);
            nfree(funcret);
        } else {
            copyStr((ftype == 1) ? defaultstr : defaultnum, outbuf);
        }
    }
    goto fexit;
}
if (chkCmd(1, "CINT")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    sprintf(outbuf, "%d", (int)round(strtold(farg[1], NULL)));
    goto fexit;
}
if (chkCmd(1, "INT")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
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
    long double dbl;
    int act = 0;
    uint64_t num;
    char* tmpstr = NULL;
    int32_t tmplen = 0;
    int32_t tmppos = 0;
    if (fargct == 2) act = atoi(farg[2]);
    switch (act) {
        case 0:;
            tmplen = strlen(farg[1]);
            if (!tmplen) {copyStr(defaultstr, outbuf); break;}
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
            sscanf(tmpstr, "%Lf", &dbl);
            free(tmpstr);
            sprintf(outbuf, "%Lf", dbl);
            break;
        case 1:;
            sscanf(farg[1], "%llx", (long long unsigned int*)&num);
            sprintf(outbuf, "%llu", (long long unsigned int)num);
            break;
        case 2:;
            sscanf(farg[1], "%llo", (long long unsigned int*)&num);
            sprintf(outbuf, "%llu", (long long unsigned int)num);
            break;
        case 3:;
            char* tmpi = farg[1];
            num = 0;
            while (*tmpi == '0' || *tmpi == '1' || *tmpi == 'b') {
                num <<= 1;
                num += (*tmpi++ == '1');
            }
            sprintf(outbuf, "%llu", (long long unsigned int)num);
            break;            
        default:;
            cerr = 16;
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
    long double dbl1, dbl2;
    sscanf(farg[1], "%Lf", &dbl1);
    sscanf(farg[2], "%Lf", &dbl2);
    sprintf(outbuf, "%Lf", fmodl(dbl1, dbl2));
    goto fexit;
}
if (chkCmd(1, "PI")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    strcpy(outbuf, "3.14159265358979323846264338327950288");
    goto fexit;
}
if (chkCmd(1, "ABS")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    sprintf(outbuf, "%Lf", fabsl(strtold(farg[1], NULL)));
    goto fexit;
}
if (chkCmd(1, "SIN")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
    sprintf(outbuf, "%Lf", sinl(dbl));
    goto fexit;
}
if (chkCmd(1, "COS")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
    sprintf(outbuf, "%Lf", cosl(dbl));
    goto fexit;
}
if (chkCmd(1, "TAN")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
    dbl = tanl(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%Lf", dbl);
    goto fexit;
}
if (chkCmd(1, "SINH")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
    dbl = sinhl(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%Lf", dbl);
    goto fexit;
}
if (chkCmd(1, "COSH")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
    dbl = coshl(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%Lf", dbl);
    goto fexit;
}
if (chkCmd(1, "TANH")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
    dbl = tanhl(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%Lf", dbl);
    goto fexit;
}
if (chkCmd(1, "LOG")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
    dbl = log(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%Lf", dbl);
    goto fexit;
}
if (chkCmd(1, "LOG10")) {
    cerr = 0;
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
    dbl = log10(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%Lf", dbl);
    goto fexit;
}
if (chkCmd(1, "SHIFT")) {
    if (fargct != 2) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 2;
    if (fargt[1] != 2 || fargt[2] != 2) {cerr = 2; goto fexit;}
    uint64_t num;
    sscanf(farg[1], "%llu", (long long unsigned int*)&num);
    int l = atoi(farg[2]);
    if (l < 0) {
        l *= -1;
        num >>= l;
    } else {
        num <<= l;
    }
    sprintf(outbuf, "%llu", (long long unsigned int)num);
    goto fexit;
}
if (chkCmd(1, "NOT")) {
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 2;
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    uint64_t num;
    sscanf(farg[1], "%llu", (long long unsigned int*)&num);
    sprintf(outbuf, "%llu", (long long unsigned int)~num);
    goto fexit;
}
if (chkCmd(1, "AND")) {
    if (fargct != 2) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 2;
    if (fargt[1] != 2 || fargt[2] != 2) {cerr = 2; goto fexit;}
    uint64_t num1, num2;
    sscanf(farg[1], "%llu", (long long unsigned int*)&num1);
    sscanf(farg[2], "%llu", (long long unsigned int*)&num2);
    sprintf(outbuf, "%llu", (long long unsigned int)(num1 & num2));
    goto fexit;
}
if (chkCmd(1, "OR")) {
    if (fargct != 2) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 2;
    if (fargt[1] != 2 || fargt[2] != 2) {cerr = 2; goto fexit;}
    uint64_t num1, num2;
    sscanf(farg[1], "%llu", (long long unsigned int*)&num1);
    sscanf(farg[2], "%llu", (long long unsigned int*)&num2);
    sprintf(outbuf, "%llu", (long long unsigned int)(num1 | num2));
    goto fexit;
}
if (chkCmd(1, "XOR")) {
    if (fargct != 2) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 2;
    if (fargt[1] != 2 || fargt[2] != 2) {cerr = 2; goto fexit;}
    uint64_t num1, num2;
    sscanf(farg[1], "%llu", (long long unsigned int*)&num1);
    sscanf(farg[2], "%llu", (long long unsigned int*)&num2);
    sprintf(outbuf, "%llu", (long long unsigned int)(num1 ^ num2));
    goto fexit;
}
if (chkCmd(1, "EXP")) {
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 2;
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
    dbl = exp(dbl);
    if (!isfinite(dbl)) {cerr = 5; goto fexit;}
    sprintf(outbuf, "%Lf", dbl);
    goto fexit;
}
if (chkCmd(1, "INKEY$")) {
    if (fargct) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 1;
    #ifndef _WIN32
    int32_t tmp = 0;
    copyStr(inkeybuf, outbuf);
    int obp = strlen(inkeybuf);
    inkeybuf[0] = 0;
    if (!(tmp = kbhit())) goto fexit;
    tmp += obp;
    while (obp < tmp) {
        outbuf[obp] = 0;
        outbuf[obp] = getchar();
        if (outbuf[obp] == 0) {break;}
        obp++;
    }
    outbuf[obp] = 0;
    #else
    uctStop();
    copyStr(kbinbuf, outbuf);
    kbinbuf[0] = 0;
    uctStart();
    #endif
    goto fexit;
}
if (chkCmd(1, "UCASE$")) {
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 1;
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    upCase(farg[1]);
    copyStr(farg[1], outbuf);
    goto fexit;
}
if (chkCmd(1, "LCASE$")) {
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 1;
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    lowCase(farg[1]);
    copyStr(farg[1], outbuf);
    goto fexit;
}
if (chkCmd(1, "LEN")) {
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 2;
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    sprintf(outbuf, "%lu", (long unsigned)flen[1]);
    goto fexit;
}
if (chkCmd(1, "TYPEOF")) {
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 2;
    sprintf(outbuf, "%u", fargt[1]);
    goto fexit;
}
if (chkCmd(1, "SNIP$")) {
    if (fargct < 2 || fargct > 3) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 1;
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    int32_t start, end;
    if (fargct == 2) {
        if (fargt[2] == 1) {cerr = 2; goto fexit;}
        if (fargt[2] == 0) {cerr = 3; goto fexit;}
        start = 0;
        end = atoi(farg[2]);
    } else {
        if (fargt[2] == 1 || fargt[3] == 1) {cerr = 2; goto fexit;}
        if (fargt[2] + fargt[3]) {
            if (!fargt[2]) {start = 0;}
            else {start = atoi(farg[2]);}
            if (!fargt[3]) {end = 32;}
            else {end = atoi(farg[3]);}
        } else {
            cerr = 3;
            goto fexit;
        }
    }
    if (start < 0) start = 0;
    if (start > flen[1]) start = flen[1];
    if (end < start) end = start;
    if (end > flen[1]) end = flen[1];
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
    uint64_t num;
    sscanf(farg[1], "%llu", (long long unsigned int *)&num);
    sprintf(outbuf, "%llx", (long long unsigned int)num);
    upCase(outbuf);
    goto fexit;
}
if (chkCmd(1, "OCT$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    long double dbl;
    sscanf(farg[1], "%Lf", &dbl);
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
    long double num = 0;
    if (fargct < 2 || fargct > 3) {cerr = 3; goto fexit;}
    if (fargt[1] != 2 || fargt[2] != 2) {cerr = 2; goto fexit;}
    if (fargct == 3 && fargt[3] == 1) {cerr = 2; goto fexit;}
    num = strtold(farg[1], NULL);
    if (fargct == 2) {
        long double max = strtold(farg[2], NULL);
        if (num > max) {num = max;}
    } else if (fargct == 3 && fargt[3] == 0) {
        long double min = strtold(farg[2], NULL);
        if (num < min) {num = min;}
    } else if (fargct == 3) {
        long double min = strtold(farg[2], NULL);
        long double max = strtold(farg[3], NULL);
        if (num > max) {num = max;}
        else if (num < min) {num = min;}
    } else {
        cerr = 3;
        goto fexit;
    }
    sprintf(outbuf, "%Lf", num);
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
    ioctl(0, TIOCGWINSZ, &max);
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
    ioctl(0, TIOCGWINSZ, &max);
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
    if (txtattrib.truecolor) sprintf(outbuf, "%u", txtattrib.truefgc);
    else sprintf(outbuf, "%u", txtattrib.fgc);
    goto fexit;
}
if (chkCmd(1, "BGC")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    if (txtattrib.truecolor) sprintf(outbuf, "%u", txtattrib.truebgc);
    else sprintf(outbuf, "%u", txtattrib.bgc);
    goto fexit;
}
if (chkCmd(1, "TRUECOLOR")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (int)txtattrib.truecolor);
    goto fexit;
}
if (chkCmd(1, "INPUT$")) {
    cerr = 0;
    ftype = 1;
    if (fargct > 1) {cerr = 3; goto fexit;}
    if (fargct && fargt[1] != 1) {cerr = 2; goto fexit;}
    copyStr((fargct) ? farg[1] : "?: ", gpbuf);
    char* tmp = NULL;
    #ifdef _WIN32
    uctStop();
    #else
    getCurPos();
    char* tfarg = gpbuf + strlen(gpbuf);
    while (--curx) {*tfarg++ = 22;}
    *tfarg = 0;
    __typeof__(rl_getc_function) old_rl_getc_function = rl_getc_function;
    rl_getc_function = getc;
    #endif
    tmp = readline(gpbuf);
    #ifndef _WIN32
    rl_getc_function = old_rl_getc_function;
    #else
    uctStart();
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
if (chkCmd(1, "WORDS")) {
    cerr = 0;
    ftype = 2;
    if (fargct < 1 || fargct > 2) {cerr = 3; goto fexit;}
    if (fargt[1] != 1 || fargt[1] != 1) {cerr = 2; goto fexit;}
    int num = 0;
    if (num < 0) {cerr = 16; goto fexit;}
    char div = ' ';
    if (fargct == 2) {
        if (fargt[2] != 1) {cerr = 2; goto fexit;}
        if (!farg[2][0] || farg[2][1]) {cerr = 16; goto fexit;}
        div = farg[2][0];
    }
    int ip = 0, jp = 0;
    {
        int i = 0;
        while (1) {
            while (farg[1][ip] == ' ' || farg[1][ip] == '\t') {++ip;}
            if (!farg[1][ip]) break;
            jp = ip;
            while (farg[1][jp] && farg[1][jp] != div) {++jp;}
            ++num;
            if (!farg[1][jp]) break;
            ip = jp;
            if (farg[1][ip]) {++ip;}
            else {break;}
            ++i;
        }
    }
    sprintf(outbuf, "%d", num);
    goto fexit;
}
if (chkCmd(1, "WORD$")) {
    cerr = 0;
    ftype = 1;
    if (fargct < 2 || fargct > 3) {cerr = 3; goto fexit;}
    if (fargt[1] != 2 || fargt[2] != 1) {cerr = 2; goto fexit;}
    int num = atoi(farg[1]);
    if (num < 0) {cerr = 16; goto fexit;}
    char div = ' ';
    if (fargct == 3) {
        if (fargt[3] != 1) {cerr = 2; goto fexit;}
        if (!farg[3][0] || farg[3][1]) {cerr = 16; goto fexit;}
        div = farg[3][0];
    }
    int ip = 0, jp = 0;
    {
        int i = 0;
        while (1) {
            while (farg[2][ip] == ' ' || farg[2][ip] == '\t') {++ip;}
            if (!farg[2][ip]) break;
            jp = ip;
            while (farg[2][jp] && farg[2][jp] != div) {++jp;}
            if (i == num) {
                int njp = jp;
                while (njp > ip && (farg[2][njp - 1] == ' ' || farg[2][njp - 1] == '\t')) {--njp;}
                copyStrSnip(farg[2], ip, njp, outbuf);
                break;
            }
            if (!farg[2][jp]) break;
            ip = jp;
            if (farg[2][ip]) {++ip;}
            else {break;}
            ++i;
        }
    }
    goto fexit;
}
if (chkCmd(1, "LTRIM$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    char* str = farg[1];
    while (1) {
        if (!*str) break;
        if (*str == ' ' || *str == '\t') ++str;
        else break;
    }
    copyStr(str, outbuf);
    goto fexit;
}
if (chkCmd(1, "RTRIM$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    if (!farg[1][0]) goto fexit;
    int ptr = flen[1];
    while (ptr > 0) {
        --ptr;
        if (farg[1][ptr] != ' ' && farg[1][ptr] != '\t') {++ptr; break;}
    }
    copyStrTo(farg[1], ptr, outbuf);
    goto fexit;
}
if (chkCmd(1, "TRIM$")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    if (!farg[1][0]) goto fexit;
    char* str = farg[1];
    while (1) {
        if (!*str) break;
        if (*str == ' ' || *str == '\t') ++str;
        else break;
    }
    int ptr = flen[1] - (str - farg[1]);
    while (ptr > 0) {
        --ptr;
        if (str[ptr] != ' ' && str[ptr] != '\t') {++ptr; break;}
    }
    copyStrTo(str, ptr, outbuf);
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
        case 0:;
            edata = timeinfo->tm_sec;
            break;
        case 1:;
            edata = timeinfo->tm_min;
            break;
        case 2:;
            edata = timeinfo->tm_hour;
            break;
        case 3:;
            edata = timeinfo->tm_mday;
            break;
        case 4:;
            edata = timeinfo->tm_mon;
            break;
        case 5:;
            edata = 1900 + timeinfo->tm_year;
            break;
        case 6:;
            edata = timeinfo->tm_wday;
            break;
        case 7:;
            edata = timeinfo->tm_yday;
            break;
        case 8:;
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
    char* ret;
    ret = getcwd(outbuf, CB_BUF_SIZE);
    (void)ret;
    goto fexit;
}
if (chkCmd(1, "FILES$")) {
    cerr = 0;
    fileerror = 0;
    ftype = 1;
    if (fargct > 1) {cerr = 3; goto fexit;}
    char* olddn = NULL;
    char* bret;
    int ret;
    if (fargct) {
        if (fargt[1] != 1) {cerr = 2; goto fexit;}
        olddn = malloc(CB_BUF_SIZE);
        bret = getcwd(olddn, CB_BUF_SIZE);
        if (chdir(farg[1])) {
            fileerror = errno;
            free(olddn);
            outbuf[0] = 0;
            goto fexit;
        }
    }
    (void)bret;
    DIR* cwd = opendir(".");
    if (!cwd) {if (fargct) {ret = chdir(olddn); free(olddn);} outbuf[0] = 0; goto fexit;}
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
        ret = chdir(olddn);
        free(olddn);
    }
    closedir(cwd);
    (void)ret;
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
if (chkCmd(1, "LOADEXT")) {
    ftype = 2;
    if (fargct < 1) {cerr = 3; goto fexit;}
    cerr = 0;
    outbuf[0] = '1';
    for (int i = 1; i <= fargct; i++) {
        if (fargt[i] != 1) {cerr = 2; goto fexit;}
        if (loadExt(farg[i]) < 0) {outbuf[0] = '0'; break;}
    }
    outbuf[1] = 0;
    cerr = 0;
    goto fexit;
}
if (chkCmd(1, "UNLOADEXT")) {
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    if (fargt[1] == 1) {
        upCase(farg[1]);
        for (register int i = 0; i < extmaxct; ++i) {
            if (extdata[i].inuse && !strcmp(farg[1], extdata[i].name)) {
                sprintf(outbuf, "%d", (int)unloadExt(i));
            }
        }
    } else {
        sprintf(outbuf, "%d", (int)unloadExt(atoi(farg[1])));
    }
    cerr = 0;
    goto fexit;
}
if (chkCmd(1, "READEXTNAME$")) {
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    if (fargt[1] != 1) {cerr = 2; goto fexit;}
    #ifndef _WIN32
    void* lib = dlopen(farg[1], RTLD_LAZY);
    #else
    void* lib = LoadLibrary(farg[1]);
    #endif
    if (lib) {
        char* extname = (void*)dlsym(lib, "cbext_name");
        if (!extname) {copyStr("", outbuf);}
        else {copyStr(extname, outbuf); upCase(outbuf);}
        dlclose(lib);
    } else {
        outbuf[0] = 0;
    }
    goto fexit;
}
if (chkCmd(1, "EXTNAME$")) {
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    int e = atoi(farg[1]);
    if (e >= 0 && e < extmaxct && extdata[e].inuse) {
        copyStr(extdata[e].name, outbuf);
    } else {
        outbuf[0] = 0;
    }
    goto fexit;
}
if (chkCmd(1, "EXTLOADED")) {
    ftype = 2;
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    if (fargt[1] == 1) {
        upCase(farg[1]);
        outbuf[0] = '0';
        for (register int i = 0; i < extmaxct; ++i) {
            if (extdata[i].inuse && !strcmp(farg[1], extdata[i].name)) {
                outbuf[0] = '1';
            }
        }
    } else {
        int e = atoi(farg[1]);
        if (e >= 0 && e < extmaxct) {
            outbuf[0] = '0' + extdata[e].inuse;
        }
    }
    outbuf[1] = 0;
    goto fexit;
}
if (chkCmd(1, "EXTENSIONS$")) {
    ftype = 1;
    if (fargct) {cerr = 3; goto fexit;}
    cerr = 0;
    for (register int i = 0; i < extmaxct; ++i) {
        if (i < 0) strApndChar(outbuf, '\n');
        if (extdata[i].inuse) {copyStrApnd(extdata[i].name, outbuf);}
    }
    goto fexit;
}
goto fexit;
_func:;
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
if (chkCmd(1, "_ISATTY")) {
    cerr = 0;
    ftype = 1;
    if (fargct != 1) {cerr = 3; goto fexit;}
    if (fargt[1] != 2) {cerr = 2; goto fexit;}
    int n = atoi(farg[1]);
    if (n == 0) {
        n = isatty(STDIN_FILENO);
    } else if (n == 1) {
        n = isatty(STDOUT_FILENO);
    } else if (n == 2) {
        n = isatty(STDERR_FILENO);
    } else {
        cerr = 16;
        goto fexit;
    }
    sprintf(outbuf, "%d", n);
    goto fexit;
}
if (chkCmd(1, "_TXTLOCK")) {
    cerr = 0;
    ftype = 2;
    if (fargct) {cerr = 3; goto fexit;}
    sprintf(outbuf, "%d", (int)textlock);
    goto fexit;
}
if (chkCmd(1, "_TXTATTRIB")) {
    if (fargct != 1) {cerr = 3; goto fexit;}
    cerr = 0;
    ftype = 2;
    if (fargt[1] == 0) {cerr = 3; goto fexit;}
    int attrib = 0;
    if (fargt[1] == 1) {
        for (int32_t i = 0; farg[1][i]; i++) {
            if (farg[1][i] >= 'a' && farg[1][i] <= 'z') farg[1][i] -= 32;
            if (farg[1][i] == ' ') farg[1][i] = '_';
        }
        if (!strcmp(farg[1], "RESET")) attrib = 0; else
        if (!strcmp(farg[1], "BOLD")) attrib = 1; else
        if (!strcmp(farg[1], "ITALIC")) attrib = 2; else
        if (!strcmp(farg[1], "UNDERLINE")) attrib = 3; else
        if (!strcmp(farg[1], "DBL_UNDERLINE") || !strcmp(farg[1], "long double_UNDERLINE")) attrib = 4; else
        if (!strcmp(farg[1], "SQG_UNDERLINE") || !strcmp(farg[1], "SQUIGGLY_UNDERLINE")) attrib = 5; else
        if (!strcmp(farg[1], "STRIKETHROUGH")) attrib = 6; else
        if (!strcmp(farg[1], "OVERLINE")) attrib = 7; else
        if (!strcmp(farg[1], "DIM")) attrib = 8; else
        if (!strcmp(farg[1], "BLINK")) attrib = 9; else
        if (!strcmp(farg[1], "HIDDEN")) attrib = 10; else
        if (!strcmp(farg[1], "REVERSE")) attrib = 11; else
        if (!strcmp(farg[1], "UNDERLINE_COLOR")) attrib = 12; else
        if (!strcmp(farg[1], "FGC")) attrib = 13; else
        if (!strcmp(farg[1], "BGC")) attrib = 14; else
        if (!strcmp(farg[1], "TRUECOLOR") || !strcmp(farg[1], "TRUE_COLOR") || !strcmp(farg[1], "24BITCOLOR") || !strcmp(farg[1], "24BIT_COLOR")) attrib = 15; else
        {cerr = 16; goto fexit;}
    } else {
        attrib = atoi(farg[1]);
        if (attrib < 0 || attrib > 15) {cerr = 16; goto fexit;}
    }
    int val = 0;
    switch (attrib) {
        case 0: val = 0; break;
        case 1: val = txtattrib.bold; break;
        case 2: val = txtattrib.italic; break;
        case 3: val = txtattrib.underln; break;
        case 4: val = txtattrib.underlndbl; break;
        case 5: val = txtattrib.underlnsqg; break;
        case 6: val = txtattrib.strike; break;
        case 7: val = txtattrib.overln; break;
        case 8: val = txtattrib.dim; break;
        case 9: val = txtattrib.blink; break;
        case 10: val = txtattrib.hidden; break;
        case 11: val = txtattrib.reverse; break;
        case 12: val = txtattrib.underlncolor; break;
        case 13: val = txtattrib.fgce; break;
        case 14: val = txtattrib.bgce; break;
        case 15: val = txtattrib.truecolor; break;
    }
    sprintf(outbuf, "%d", (int)val);
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
        if (i == 0) {copyStr(progfn[progindex], outbuf);}
        else {copyStr(progargs[i], outbuf);}
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
