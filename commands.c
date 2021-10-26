int extcerr = 255;
for (int i = extmaxct - 1; i > -1; --i) {
    if (extdata[i].inuse && extdata[i].runcmd) {
        extcerr = extdata[i].runcmd(argct, arg, argt, argl);
        if (extcerr != 255) {
            cerr = extcerr;
            if (!extcerr) goto noerr;
            else goto cmderr;
        }
    }
}
if (chkCmdPtr[0] == '_') goto _cmd;
if (chkCmd(2, "EXIT", "QUIT")) {
    if (argct > 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    err = 0;
    if (argct == 1) {
        if (!solvearg(1)) goto cmderr;
        if (runfile) {
            err = retval = atoi(arg[1]);
        } else {
            err = atoi(arg[1]);
        }
    }
    if (inProg) {
        if (progindex > 0) unloadProg();
        else cmdint = true;
        retval = err;
    } else {
        cleanExit();
    }
    goto cmderr;
}
if (chkCmd(1, "PUT")) {
    cerr = 0;
    for (int i = 1; i <= argct; i++) {if (!solvearg(i)) {goto cmderr;} fputs(arg[i], stdout);}
    fflush(stdout);
    goto noerr;
}
if (chkCmd(2, "SET", "LET")) {
    if (argct != 2) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(2)) goto cmderr;
    if (!arg[1][0] || !argt[2]) {cerr = 1; goto cmderr;}
    if (!setVar(arg[1], arg[2], argt[2], -1)) goto cmderr;
    goto noerr;
}
if (chkCmd(1, "DIM")) {
    if (argct < 2 || argct > 3) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(2)) goto cmderr;
    if (argct == 3 && !solvearg(3)) goto cmderr;
    if (argt[2] != 2) {cerr = 2; goto cmderr;}
    int32_t asize = atoi(arg[2]);
    if (asize < 0) {cerr = 16; goto cmderr;}
    if (!arg[1][0]) {cerr = 4; seterrstr(""); goto cmderr;}
    char* val = NULL; uint8_t type = 0;
    if (argct == 3) {
        puts("THREE");
        val = arg[3];
        type = argt[3];
    } else {
        puts("TWO");
        val = ((arg[1][argl[1] - 1] == '$') ? "" : "0");
        type = 2 - (arg[1][argl[1] - 1] == '$');
    }
    puts("OK");
    printf("arg[1]: {%s}\n", arg[1]);
    if (!setVar(arg[1], val, type, asize)) goto cmderr;
    goto noerr;
}
if (chkCmd(1, "REDIM")) {
    if (argct < 2) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(2)) goto cmderr;
    if (argt[2] != 2) {cerr = 2; goto cmderr;}
    int v = -1;
    for (register int i = 0; i < varmaxct; ++i) {
        if (vardata[i].inuse && !strcmp(arg[1], vardata[i].name)) {v = i; break;}
    }
    if (v == -1 || vardata[v].size == -1) {cerr = 23; seterrstr(arg[1]); goto cmderr;}
    int32_t s = atoi(arg[2]);
    if (s == vardata[v].size) {goto noerr;}
    int32_t os = vardata[v].size;
    if (s < 0) {cerr = 16; goto cmderr;}
    vardata[v].size = s;
    char** newdata = (char**)malloc((s + 1) * sizeof(char*));
    int32_t i = 0;
    for (; i <= s && i <= os; ++i) {
        newdata[i] = vardata[v].data[i];
    }
    for (; i <= s; ++i) {
        newdata[i] = malloc(vardata[v].type);
        if (vardata[v].type == 1) {
            newdata[i][0] = 0;
        } else {
            newdata[i][0] = '0';
            newdata[i][1] = 0;
        }
    }
    for (i = s + 1; i <= os; ++i) {
        free(vardata[v].data[i]);
    }
    free(vardata[v].data);
    vardata[v].data = newdata;
    goto noerr;
}
if (chkCmd(1, "FILL")) {
    if (argct < 1 || argct > 2) {cerr = 3; goto cmderr;}
    if (getType(arg[1]) != 255) {cerr = 4; seterrstr(arg[1]); goto cmderr;}
    upCase(arg[1]);
    int v = -1;
    for (register int i = 0; i < varmaxct; ++i) {
        if (vardata[i].inuse && !strcmp(arg[1], vardata[i].name)) {v = i; break;}
    }
    if (v == -1 || vardata[v].size == -1) {cerr = 23; seterrstr(arg[1]); goto cmderr;}
    for (int i = 0; i <= vardata[v].size; ++i) {
        if (argct > 1) {
            uint8_t t = 0;
            char* tmpbuf = malloc(CB_BUF_SIZE);
            if (!(t = getVal(arg[2], tmpbuf))) {free(tmpbuf); goto cmderr;}
            if (t != vardata[v].type) {free(tmpbuf); cerr = 2; goto cmderr;}
            tmpbuf = realloc(tmpbuf, strlen(tmpbuf) + 1);
            swap(tmpbuf, vardata[v].data[i]);
            free(tmpbuf);
        } else {
            copyStr(((vardata[v].type == 1) ? "" : "0"), vardata[v].data[i]);
        }
    }
    goto noerr;
}
if (chkCmd(1, "SWAP")) {
    if (argct != 2) {cerr = 3; goto cmderr;}
    cerr = 0;
    int v1 = -1;
    for (register int i = 0; i < varmaxct; ++i) {
        if (vardata[i].inuse && !strcmp(arg[1], vardata[i].name)) {v1 = i; break;}
    }
    if (v1 == -1 || vardata[v1].size == -1) {cerr = 23; seterrstr(arg[1]); goto cmderr;}
    int v2 = -1;
    for (register int i = 0; i < varmaxct; ++i) {
        if (vardata[i].inuse && !strcmp(arg[2], vardata[i].name)) {v2 = i; break;}
    }
    if (v2 == -1 || vardata[v2].size == -1) {cerr = 23; seterrstr(arg[2]); goto cmderr;}
    swap(vardata[v1].name, vardata[v2].name);
    goto noerr;
}   
if (chkCmd(1, "DEL")) {
    cerr = 0;
    if (argct < 1) {cerr = 3; goto cmderr;}
    for (int i = 1; i <= argct; ++i) {
        if (!delVar(arg[i])) goto cmderr;
    }
    goto noerr;
}
if (chkCmd(1, "DEFRAG")) {
    cerr = 0;
    if (argct > 0) {cerr = 3; goto cmderr;}
    int vo = 0;
    for (register int i = 0; i < varmaxct;) {
        if (!vardata[i].inuse) {++vo; --varmaxct;}
        else {++i;}
        if (vo) {vardata[i] = vardata[i + vo];}
    }
    goto noerr;
}
if (chkCmd(3, "@", "LABEL", "LBL")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    upCase(arg[1]);
    int i = -1;
    for (int j = 0; j < gotomaxct; ++j) {
        if (!gotodata[j].used) {i = j; break;}
        else if (!strcmp(gotodata[j].name, arg[1])) {
            if (gotodata[j].cp == cmdpos) {goto noerr;}
            cerr = 28; goto cmderr;
        }
    }
    if (i == -1) {
        i = gotomaxct;
        ++gotomaxct;
        gotodata = realloc(gotodata, gotomaxct * sizeof(cb_goto));
    }
    gotodata[i].name = malloc(strlen(arg[1]) + 1);
    copyStr(arg[1], gotodata[i].name);
    gotodata[i].cp = cmdpos;
    gotodata[i].pl = progLine;
    gotodata[i].used = true;
    gotodata[i].dlsp = dlstackp;
    gotodata[i].fnsp = fnstackp;
    gotodata[i].itsp = itstackp;
    #ifdef _WIN32
    updatechars();
    #endif
    goto noerr;
}
if (chkCmd(3, "%", "GOTO", "GO")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    upCase(arg[1]);
    int i = -1;
    for (int j = 0; j < gotomaxct; ++j) {
        if (gotodata[j].used) {
            if (!strcmp(gotodata[j].name, arg[1])) {i = j;}
        }
    }
    if (i == -1) {cerr = 29; goto cmderr;}
    nfree(gotodata[i].name);
    if (inProg) {
        cp = gotodata[i].cp;
    } else {
        concp = gotodata[i].cp;
    }
    progLine = gotodata[i].pl;
    gotodata[i].used = false;
    bool r = false;
    while (gotomaxct > 0 && !gotodata[gotomaxct - 1].used) {--gotomaxct; r = true;}
    if (r) gotodata = realloc(gotodata, gotomaxct * sizeof(cb_goto));
    didloop = true;
    lockpl = true;
    goto noerr;
}
if (chkCmd(1, "GOSUB")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (gsstackp >= CB_PROG_LOGIC_MAX - 1) {cerr = 32; goto cmderr;}
    upCase(arg[1]);
    int i = -1;
    for (int j = 0; j < gotomaxct; ++j) {
        if (gotodata[j].used) {
            if (!strcmp(gotodata[j].name, arg[1])) {i = j;}
        }
    }
    if (i == -1) {cerr = 29; goto cmderr;}
    nfree(gotodata[i].name);
    ++gsstackp;
    gsstack[gsstackp].cp = ((inProg) ? cp : concp);
    gsstack[gsstackp].pl = progLine;
    gsstack[gsstackp].dlsp = dlstackp;
    gsstack[gsstackp].fnsp = fnstackp;
    gsstack[gsstackp].itsp = itstackp;
    gsstack[gsstackp].brkinfo = brkinfo;
    if (inProg) {
        cp = gotodata[i].cp;
    } else {
        concp = gotodata[i].cp;
    }
    progLine = gotodata[i].pl;
    gotodata[i].used = false;
    bool r = false;
    while (gotomaxct > 0 && !gotodata[gotomaxct - 1].used) {--gotomaxct; r = true;}
    if (r) gotodata = realloc(gotodata, gotomaxct * sizeof(cb_goto));
    didloop = true;
    lockpl = true;
    goto noerr;
}
if (chkCmd(1, "RETURN")) {
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (gsstackp < 0) {cerr = 31; goto cmderr;}
    if (inProg) {
        cp = gsstack[gsstackp].cp;
    } else {
        concp = gsstack[gsstackp].cp;
    }
    progLine = gsstack[gsstackp].pl;
    dlstackp = gsstack[gsstackp].dlsp;
    fnstackp = gsstack[gsstackp].fnsp;
    itstackp = gsstack[gsstackp].itsp;
    brkinfo = gsstack[gsstackp].brkinfo;
    --gsstackp;
    didloop = true;
    lockpl = true;
    goto noerr;
}
if (chkCmd(2, "CONTINUE", "BREAK")) {
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (brkinfo.block == 1) {
        dldcmd[dlstackp] = !dldcmd[dlstackp];
    } else if (brkinfo.block == 2) {
        fndcmd[fnstackp] = !fndcmd[fnstackp];
    } else {
        cerr = 30;
        goto cmderr;
    }
    brkinfo.type = 1 + !strcmp(arg[0], "BREAK");
    goto noerr;
}
if (chkCmd(1, "COLOR")) {
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    int32_t tmp = 0;
    if (argt[1] == 0) {} else
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {
        tmp = atoi(arg[1]);
        if (txtattrib.truecolor) {
            if (tmp < 0 || tmp > 0xFFFFFF) {cerr = 16; goto cmderr;}
            txtattrib.truefgc = tmp;
        } else {
            if (tmp < 0 || tmp > 255) {cerr = 16; goto cmderr;}
            txtattrib.fgc = (uint8_t)tmp;
        }
        #ifndef _WIN_NO_VT
        if (esc && txtattrib.fgce) {
            if (txtattrib.truecolor) printf("\e[38;2;%u;%u;%um", (uint8_t)(txtattrib.truefgc >> 16), (uint8_t)(txtattrib.truefgc >> 8), (uint8_t)txtattrib.truefgc);
            else printf("\e[38;5;%um", txtattrib.fgc);
        }
        #else
        updateTxtAttrib();
        #endif
    }
    if (argct > 1) {
        if (!solvearg(2)) goto cmderr;
        if (argt[2] == 0) {} else
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {
            tmp = atoi(arg[2]);
            if (txtattrib.truecolor) {
                if (tmp < 0 || tmp > 0xFFFFFF) {cerr = 16; goto cmderr;}
                txtattrib.truebgc = tmp;
            } else {
                if (tmp < 0 || tmp > 255) {cerr = 16; goto cmderr;}
                txtattrib.bgc = (uint8_t)tmp;
            }
            #ifndef _WIN_NO_VT
            if (esc && txtattrib.bgce) {
                if (txtattrib.truecolor) printf("\e[48;2;%u;%u;%um", (uint8_t)(txtattrib.truebgc >> 16), (uint8_t)(txtattrib.truebgc >> 8), (uint8_t)txtattrib.truebgc);
                else printf("\e[48;5;%um", txtattrib.bgc);
            }
            #else
            updateTxtAttrib();
            #endif
        }
    }
    fflush(stdout);
    goto noerr;
}
if (chkCmd(1, "LOCATE")) {
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    int tmp = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {}
    else if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {
        tmp = atoi(arg[1]);
        if (tmp < 1) {cerr = 16; goto cmderr;}
        else {
            #ifndef _WIN_NO_VT
            if (esc) printf("\e[%dG", tmp);
            #else
            curx = tmp;
            #endif
        }
    }
    if (argct > 1) {
        if (!solvearg(2)) goto cmderr;
        if (argt[1] == 0 && argt[2] == 0) {cerr = 3; goto cmderr;}
        else if (argt[2] == 0) {}
        else if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {
            tmp = atoi(arg[2]);
            if (tmp < 1) {cerr = 16; goto cmderr;}
            else {
                #ifndef _WIN_NO_VT
                if (esc) {
                    --tmp;
                    fputs("\e[32767A", stdout);
                    if (tmp) printf("\e[%dB", tmp);
                }
                #else
                cury = tmp;
                #endif
            }
        }
    }
    #ifdef _WIN_NO_VT
    SetConsoleCursorPosition(hConsole, (COORD){curx - 1, cury - 1});
    #else
    fflush(stdout);
    #endif
    goto noerr;
}
if (chkCmd(1, "RLOCATE")) {
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    int tmp = 0;
    if (!solvearg(1)) goto cmderr;
    #ifndef _WIN_NO_VT
    getCurPos();
    #endif
    if (argt[1] == 0) {}
    else if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {
        tmp = atoi(arg[1]);
        if (tmp == 0) {}
        else if (tmp < 0) {
            #ifndef _WIN_NO_VT
            if (esc) printf("\e[%dD", -tmp);
            #else
            curx += tmp;
            if (curx < 0) curx = 0;
            #endif
        } else {
            #ifndef _WIN_NO_VT
            if (esc) printf("\e[%dC", tmp);
            #else
            curx += tmp;
            #endif
        }
    }
    if (argct > 1) {
        if (!solvearg(2)) goto cmderr;
        if (argt[1] == 0 && argt[2] == 0) {cerr = 3; goto cmderr;}
        else if (argt[2] == 0) {}
        else if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {
            tmp = atoi(arg[2]);
            if (tmp == 0) {}
            else if (tmp < 0) {
                #ifndef _WIN_NO_VT
                if (esc) printf("\e[%dA", -tmp);
                #else
                cury += tmp;
                if (cury < 0) cury = 0;
                #endif
            } else {
                #ifndef _WIN_NO_VT
                if (esc) printf("\e[%dB", tmp);
                #else
                cury += tmp;
                #endif
            }
        }
    }
    #ifdef _WIN_NO_VT
    SetConsoleCursorPosition(hConsole, (COORD){curx - 1, cury - 1});
    #else
    fflush(stdout);
    #endif
    goto noerr;
}
if (chkCmd(1, "CLS")) {
    if (argct > 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    uint8_t tbgc = txtattrib.bgc;
    #ifndef _WIN_NO_VT
    uint32_t ttbgc = txtattrib.truebgc;
    #endif
    if (argct) {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 2) {cerr = 2; goto cmderr;}
        #ifndef _WIN_NO_VT
        if (txtattrib.truecolor) {
            ttbgc = (uint32_t)atoi(arg[1]);
        } else {
            tbgc = (uint8_t)atoi(arg[1]);
        }
        #else
        tbgc = (uint8_t)atoi(arg[1]);
        #endif
    }
    #ifndef _WIN_NO_VT
    if (esc && argct) {
        if (txtattrib.truecolor) printf("\e[48;2;%u;%u;%um", (uint8_t)(ttbgc >> 16), (uint8_t)(ttbgc >> 8), (uint8_t)ttbgc);
        else printf("\e[48;5;%um", tbgc);
    }
    if (esc) fputs("\e[H\e[2J\e[3J", stdout);
    updateTxtAttrib();
    fflush(stdout);
    #else
	SetConsoleTextAttribute(hConsole, (txtattrib.fgc % 16) + (tbgc % 16) * 16);
    system("cls");
    updateTxtAttrib();
    #endif
    goto noerr;
}
if (chkCmd(1, "WAITUS")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    if (arg[1][0] == '-') {cerr = 16; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    cb_wait(d);
    goto noerr;
}
if (chkCmd(1, "WAITMS")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    if (arg[1][0] == '-') {cerr = 16; goto cmderr;}
    double d;
    sscanf(arg[1], "%lf", &d);
    cb_wait(d * 1000);
    goto noerr;
}
if (chkCmd(1, "WAIT")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    if (arg[1][0] == '-') {cerr = 16; goto cmderr;}
    double d;
    sscanf(arg[1], "%lf", &d);
    cb_wait(d * 1000000);
    goto noerr;
}
if (chkCmd(1, "RESETTIMER")) {
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    resetTimer();
    goto noerr;
}
if (chkCmd(2, "SRAND", "SRND")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    double rs;
    sscanf(arg[1], "%lf", &rs);
    srand(rs);
    goto noerr;
}
if (chkCmd(2, "CALL", "CALLA")) {
    if (argct < 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    bool execa = false;
    char** tmparg = NULL;
    int tmpargct = 0;
    if (!strcmp(arg[0], "CALLA")) {
        if (argct != 1) {cerr = 3; goto cmderr;}
        execa = true;
        int v = -1;
        for (register int i = 0; i < varmaxct; ++i) {
            if (vardata[i].inuse && !strcmp(arg[1], vardata[i].name)) {v = i; break;}
        }
        if (v == -1 || vardata[v].size == -1) {cerr = 23; seterrstr(arg[1]); goto cmderr;}
        if (vardata[v].type != 1) {cerr = 2; goto cmderr;}
        tmparg = arg;
        tmpargct = argct;
        arg = vardata[v].data - 1;
        argct = vardata[v].size + 1;
    } else {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 1) {cerr = 2; goto cmderr;}
    }
    newprogargc = argct;
    newprogargs = (char**)malloc((argct + 1) * sizeof(char*));
    for (int i = 2; i <= argct; ++i) {
        if (!execa) {
            if (!solvearg(i)) {
                for (int j = i - 1; j > 0; --j) {
                    free(newprogargs[j]);
                }
                free(newprogargs);
                goto cmderr;
            }
        }
        newprogargs[i - 1] = malloc(argl[i] + 1);
        copyStr(arg[i], newprogargs[i - 1]);
    }
    inprompt = !runfile;
    setsig(SIGINT, cleanExit);
    if (!loadProg(arg[1])) goto cmderr;
    chkinProg = true;
    cp = 0;
    didloop = true;
    if (execa) {
        argct = tmpargct;
        arg = tmparg;
    }
    goto noerr;
}
if (chkCmd(2, "RUN", "RUNA")) {
    if (argct < 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    bool execa = false;
    char** tmparg = NULL;
    int tmpargct = 0;
    if (!strcmp(arg[0], "RUNA")) {
        if (argct != 1) {cerr = 3; goto cmderr;}
        execa = true;
        int v = -1;
        for (register int i = 0; i < varmaxct; ++i) {
            if (vardata[i].inuse && !strcmp(arg[1], vardata[i].name)) {v = i; break;}
        }
        if (v == -1 || vardata[v].size == -1) {cerr = 23; seterrstr(arg[1]); goto cmderr;}
        if (vardata[v].type != 1) {cerr = 2; goto cmderr;}
        tmparg = arg;
        tmpargct = argct;
        arg = vardata[v].data - 1;
        argct = vardata[v].size + 1;
    } else {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 1) {cerr = 2; goto cmderr;}
    }
    #ifndef _WIN32
    char** runargs = (char**)malloc((argct + 3) * sizeof(char*));
    runargs[0] = startcmd;
    runargs[1] = roptstr;
    runargs[2] = arg[1];
    argct += 2;
    int argno = 3;
    for (; argno < argct; argno++) {
        if (!execa) if (!solvearg(argno - 1)) {free(runargs); goto cmderr;}
        runargs[argno] = arg[argno - 1];
    }
    argct -= 2;
    runargs[argno] = NULL;
    pid_t pid = fork();
    if (pid < 0) cerr = -1;
    else if (pid == 0) {
        execvp(startcmd, runargs);
        exit(0);
    }
    else if (pid > 0) {
        while (wait(&retval) != pid) {}
        retval = WEXITSTATUS(retval);
    }
    free(runargs);
    #else
    char* tmpcmd = malloc(CB_BUF_SIZE);
    tmpcmd[0] = 0;
    bool nq;
    if ((nq = winArgNeedsQuotes(startcmd))) copyStrApnd(" \"", tmpcmd);
    copyStrApnd(startcmd, tmpcmd);
    if (nq) strApndChar(tmpcmd, '"');
    copyStrApnd(" -x", tmpcmd);
    for (int argno = 1; argno <= argct; ++argno) {
        if (!execa) if (argno > 1) if (!solvearg(argno)) {free(tmpcmd); goto cmderr;}
        strApndChar(tmpcmd, ' ');
        bool nq = winArgNeedsQuotes(arg[argno]);
        if (nq) strApndChar(tmpcmd, '"');
        copyStrApnd(arg[argno], tmpcmd);
        if (nq) strApndChar(tmpcmd, '"');
    }
    int ret = system(tmpcmd);
    (void)ret;
    free(tmpcmd);
    #endif
    if (execa) {
        argct = tmpargct;
        arg = tmparg;
    }
    updateTxtAttrib();
    goto noerr;
}
if (chkCmd(2, "$", "SH")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN_NO_VT
    if (esc && sh_clearAttrib) fputs("\e[0m", stdout);
    #else
    if (sh_clearAttrib) SetConsoleTextAttribute(hConsole, ocAttrib);
    #endif
    fflush(stdout);
    arg[1] = realloc(arg[1], strlen(arg[1]) + 6); copyStrApnd(" 2>&1", arg[1]);
    #ifdef _WIN32
    if (sh_silent) {arg[1] = realloc(arg[1], strlen(arg[1]) + 13); copyStrApnd(" 1>nul 2>nul", arg[1]);}
    #else
    if (sh_silent) {arg[1] = realloc(arg[1], strlen(arg[1]) + 13); copyStrApnd(" &>/dev/null", arg[1]);}
    #endif
    fflush(stdout);
    int duperr;
    duperr = dup(2);
    close(2);
    retval = WEXITSTATUS(system(arg[1]));
    dup2(duperr, 2);
    close(duperr);
    if (sh_restoreAttrib) updateTxtAttrib();
    cerr = 0;
    goto noerr;
}
if (chkCmd(2, "EXEC", "EXECA")) {
    if (argct < 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    bool execa = false;
    char** tmparg = NULL;
    int tmpargct = 0;
    if (!strcmp(arg[0], "EXECA")) {
        if (argct != 1) {cerr = 3; goto cmderr;}
        execa = true;
        int v = -1;
        for (register int i = 0; i < varmaxct; ++i) {
            if (vardata[i].inuse && !strcmp(arg[1], vardata[i].name)) {v = i; break;}
        }
        if (v == -1 || vardata[v].size == -1) {cerr = 23; seterrstr(arg[1]); goto cmderr;}
        if (vardata[v].type != 1) {cerr = 2; goto cmderr;}
        tmparg = arg;
        tmpargct = argct;
        arg = vardata[v].data - 1;
        argct = vardata[v].size + 1;
    } else {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 1) {cerr = 2; goto cmderr;}
    }
    #ifndef _WIN_NO_VT
    if (esc && sh_clearAttrib) fputs("\e[0m", stdout);
    #else
    if (sh_clearAttrib) SetConsoleTextAttribute(hConsole, ocAttrib);
    #endif
    fflush(stdout);
    #ifndef _WIN32
    char** runargs = (char**)malloc((argct + 1) * sizeof(char*));
    runargs[0] = arg[1];
    int argno = 1;
    for (; argno < argct; ++argno) {
        if (!execa) if (!solvearg(argno + 1)) {free(runargs); goto cmderr;}
        runargs[argno] = arg[argno + 1];
    }
    runargs[argno] = NULL;
    int stdout_dup = 0, stderr_dup = 0;
    if (sh_silent) {
        stdout_dup = dup(1);
        stderr_dup = dup(2);
        int fd = open("/dev/null", O_WRONLY);
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
        while (wait(&retval) != pid) {}
        retval = ((retval >> 8) & 0xFF);
    }
    else if (sh_silent) {
        dup2(stdout_dup, 1);
        dup2(stderr_dup, 2);
    }
    getCurPos();
    free(runargs);
    #else
    char* tmpcmd = malloc(CB_BUF_SIZE);
    tmpcmd[0] = 0;
    bool winecho = false;
    for (int argno = 1; argno <= argct; ++argno) {
        if (!execa) if (argno > 1) if (!solvearg(argno)) {free(tmpcmd); goto cmderr;};
        strApndChar(tmpcmd, ' ');
        bool nq = winArgNeedsQuotes(arg[argno]);
        if (argno == 1) {
            upCase(arg[argno]);
            winecho = !strcmp(arg[argno], "ECHO");
        }
        if (nq && !winecho) copyStrApnd(" \"", tmpcmd);
        copyStrApnd(arg[argno], tmpcmd);
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
    if (execa) {
        argct = tmpargct;
        arg = tmparg;
    }
    if (sh_restoreAttrib) updateTxtAttrib();
    if (cerr) goto cmderr;
    goto noerr;
}
if (chkCmd(1, "BELL")) {
    cerr = 0;
    int ct = 1;
    double d = 750;
    if (argct > 2) {cerr = 3; goto cmderr;}
    if (argct >= 1) {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] == 0) {cerr = 3; goto cmderr;}
        if (argt[1] != 2) {cerr = 2; goto cmderr;}
        ct = atoi(arg[1]);
        if (ct < 1) {cerr = 16; goto cmderr;}
    }
    if (argct == 2) {
        if (!solvearg(2)) goto cmderr;
        if (argt[2] == 0) {cerr = 3; goto cmderr;}
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
        if (arg[2][0] == '-') {cerr = 16; goto cmderr;}
        sscanf(arg[2], "%lf", &d);
    }
    putchar('\a');
    fflush(stdout);
    for (int i = 1; i < ct; ++i) {
        cb_wait(d * 1000);
        if (cmdint) {goto noerr;}
        putchar('\a');
        fflush(stdout);
    }
    goto noerr;
}
if (chkCmd(1, "FILES")) {
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
    char* olddn = NULL;
    if (argct) {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 1) {cerr = 2; goto cmderr;}
        int tmpret = isFile(arg[1]);
        if (tmpret) {
            if (tmpret == -1) {cerr = 15; seterrstr(arg[1]);}
            else {cerr = 19;}
            goto cmderr;
        }
        olddn = malloc(CB_BUF_SIZE);
        char* bret = getcwd(olddn, CB_BUF_SIZE);
        (void)bret;
        if (chdir(arg[1])) {
            free(olddn);
            seterrstr(arg[1]);
            cerr = 17;
            goto cmderr;
        }
    }
    DIR* cwd = opendir(".");
    int ret;
    if (!cwd) {if (argct) {ret = chdir(olddn); free(olddn);} goto noerr;}
    struct dirent* dir;
    #ifdef _WIN32
        #define DIRPFS "%s\\\n"
        puts(".\\\n..\\");
    #else
        #define DIRPFS "%s/\n"
        puts("./\n../");
    #endif
    long dbegin = telldir(cwd);
    struct stat pathstat;
    while ((dir = readdir(cwd))) {
        stat(dir->d_name, &pathstat);
        if (S_ISDIR(pathstat.st_mode) && strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) printf(DIRPFS, dir->d_name);
    }
    seekdir(cwd, dbegin);
    while ((dir = readdir(cwd))) {
        stat(dir->d_name, &pathstat);
        if (!(S_ISDIR(pathstat.st_mode))) puts(dir->d_name);
    }
    if (argct) {
        ret = chdir(olddn);
        free(olddn);
    }
    closedir(cwd);
    (void)ret;
    goto noerr;
}
if (chkCmd(1, "EXTENSIONS")) {
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    for (register int i = 0; i < extmaxct; ++i) {
        if (extdata[i].inuse) {puts(extdata[i].name);}
    }
    goto noerr;
}
if (chkCmd(2, "CHDIR", "CD")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    if (chdir(arg[1])) {
        seterrstr(arg[1]);
        cerr = 17;
        goto cmderr;
    }
    goto noerr;
}
if (chkCmd(1, "FCLOSE")) {
    cerr = 0;
    fileerror = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) {goto cmderr;}
    if (argt[1] != 2) {cerr = 3; goto cmderr;}
    if (!closeFile(atoi(arg[1]))) {cerr = 16; goto cmderr;}
    goto noerr;
}
if (chkCmd(1, "FWRITE")) {
    cerr = 0;
    fileerror = 0;
    if (argct != 2) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) {goto cmderr;}
    if (!solvearg(2)) {goto cmderr;}
    if (argt[1] != 2 || argt[2] != 1) {cerr = 2; goto cmderr;}
    int fnum = atoi(arg[1]);
    if (fnum < 0 || fnum >= filemaxct) {
        cerr = 16;
        goto cmderr;
    } else {
        errno = 0;
        fputs(arg[2], filedata[fnum].fptr);
        fileerror = errno;
    }
    goto noerr;
}
if (chkCmd(1, "FSEEK")) {
    cerr = 0;
    fileerror = 0;
    if (argct != 2) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) {goto cmderr;}
    if (!solvearg(2)) {goto cmderr;}
    if (argt[1] != 2 || argt[2] != 2) {cerr = 2; goto cmderr;}
    int fnum = atoi(arg[1]);
    if (fnum < 0 || fnum >= filemaxct) {
        cerr = 16;
        goto cmderr;
    } else {
        errno = 0;
        int32_t pos = atoi(arg[2]);
        if (pos < 0) {
            cerr = 16;
        } else {
            fseek(filedata[fnum].fptr, (pos > filedata[fnum].size) ? filedata[fnum].size : pos, SEEK_SET);
            fileerror = errno;
        }
    }
    goto noerr;
}
if (chkCmd(1, "FLUSH")) {
    cerr = 0;
    fileerror = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) {goto cmderr;}
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    int fnum = atoi(arg[1]);
    if (fnum < 0 || fnum >= filemaxct) {
        cerr = 16;
        goto cmderr;
    }
    errno = 0;
    fflush(filedata[fnum].fptr);
    fileerror = errno;
    goto noerr;
}
if (chkCmd(2, "MD", "MKDIR")) {
    cerr = 0;
    fileerror = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) {goto cmderr;}
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    errno = 0;
    #ifndef _WIN32
    mkdir(arg[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    #else
    mkdir(arg[1]);
    #endif
    fileerror = errno;
    goto noerr;
}
if (chkCmd(2, "RM", "REMOVE")) {
    cerr = 0;
    fileerror = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) {goto cmderr;}
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    cbrm(arg[1]);
    goto noerr;
}
if (chkCmd(4, "MV", "MOVE", "REN", "RENAME")) {
    cerr = 0;
    fileerror = 0;
    if (argct != 2) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) {goto cmderr;}
    if (!solvearg(2)) {goto cmderr;}
    if (argt[1] != 1 || argt[2] != 1) {cerr = 2; goto cmderr;}
    errno = 0;
    rename(arg[1], arg[2]);
    fileerror = errno;
    goto noerr;
}
if (chkCmd(1, "LOADEXT")) {
    if (argct < 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    for (int i = 1; i <= argct; i++) {
        if (!solvearg(i) || argt[i] != 1) {cerr = 2; goto cmderr;}
        if (loadExt(arg[i]) < 0) {goto cmderr;}
    }
    goto noerr;
}
if (chkCmd(1, "UNLOADEXT")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 1) {
        upCase(arg[1]);
        for (register int i = 0; i < extmaxct; ++i) {
            if (extdata[i].inuse && !strcmp(arg[1], extdata[i].name)) {
                unloadExt(i);
                goto noerr;
            }
        }
        cerr = 16;
        goto cmderr;
    } else {
        if (!unloadExt(atoi(arg[1]))) goto cmderr;
    }
    goto noerr;
}
goto cmderr;
_cmd:;
if (chkCmd(1, "_RESETTITLE")) {
    if (inProg) {cerr = 254; goto cmderr;}
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    #ifndef _WIN_NO_VT
    if (esc) {
        if (!changedtitle) {
            if (changedtitlecmd) fputs("\e[23;0t", stdout);
            goto noerr;
        }
        printf("\e]2;CLIBASIC %s (%s-bit)%c", VER, BVER, 7);
        fflush(stdout);
    }
    #else
    char* tmpstr = malloc(CB_BUF_SIZE);
    sprintf(tmpstr, "CLIBASIC %s (%s-bit)", VER, BVER);
    SetConsoleTitleA(tmpstr);
    free(tmpstr);
    #endif
    goto noerr;
}
if (chkCmd(1, "_TITLE")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN_NO_VT
    if (esc) {
        if (!changedtitle) {
            fputs("\e[22;0t", stdout);
            fflush(stdout);
            changedtitle = true;
        }
        changedtitlecmd = true;
        printf("\e]2;%s%c", arg[1], 7);
    }
    #else
    SetConsoleTitleA(arg[1]);
    #endif
    goto noerr;
}
if (chkCmd(1, "_SETENV")) {
    if (argct != 2) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (!solvearg(2)) goto cmderr;
    if (argt[1] != 1 || argt[2] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN32
    setenv(arg[1], arg[2], 1);
    #else
    SetEnvironmentVariable(arg[1], arg[2]);
    #endif
    goto noerr;
}
if (chkCmd(1, "_UNSETENV")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN32
    unsetenv(arg[1]);
    #else
    SetEnvironmentVariable(arg[1], "");
    #endif
    goto noerr;
}
if (chkCmd(1, "_PROMPT")) {
    if (inProg && !autorun) {cerr = 254; goto cmderr;}
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    copyStr(arg[1], prompt);
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    goto noerr;
}
if (chkCmd(1, "_PROMPTTAB")) {
    if (inProg && !autorun) {cerr = 254; goto cmderr;}
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    tab_width = atoi(arg[1]);
    goto noerr;
}
if (chkCmd(1, "_AUTOCMDHIST")) {
    if (inProg) {cerr = 254; goto cmderr;}
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    autohist = true;
    goto noerr;
}
if (chkCmd(1, "_SAVECMDHIST")) {
    if (inProg && !autorun) {cerr = 254; goto cmderr;}
    if (argct > 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (argct) {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 1) {cerr = 2; goto cmderr;}
        write_history(arg[1]);
    } else {
        char* tmpcwd = getcwd(NULL, 0);
        int ret = chdir(gethome());
        write_history(HIST_FILE);
        #ifdef _WIN32
        SetFileAttributesA(HIST_FILE, FILE_ATTRIBUTE_HIDDEN);
        #endif
        ret = chdir(tmpcwd);
        (void)ret;
    }
    goto noerr;
}
if (chkCmd(1, "_LOADCMDHIST")) {
    if (inProg && !autorun) {cerr = 254; goto cmderr;}
    if (argct > 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    clear_history();
    if (argct) {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 1) {cerr = 2; goto cmderr;}
        read_history(arg[1]);
    } else {
        char* tmpcwd = getcwd(NULL, 0);
        int ret = chdir(gethome());
        read_history(".clibasic_history");
        ret = chdir(tmpcwd);
        (void)ret;
    }
    goto noerr;
}
if (chkCmd(1, "_LIMITCMDHIST")) {
    if (inProg && !autorun) {cerr = 254; goto cmderr;}
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    int32_t l = atoi(arg[1]);
    if (l < -1) {
        cerr = 16; goto cmderr;
    } else if (l == -1) {
        unstifle_history();
    } else {
        stifle_history(l);
    }
    goto noerr;
}
if (chkCmd(1, "_TXTLOCK")) {
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    #ifndef _WIN32
    if (!textlock) {
        tcgetattr(0, &term);
        tcgetattr(0, &restore);
        term.c_lflag &= ~(ICANON|ECHO);
        tcsetattr(0, TCSANOW, &term);
    }
    #endif
    textlock = true;
    goto noerr;
}
if (chkCmd(1, "_TXTUNLOCK")) {
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    #ifndef _WIN32
    if (textlock) tcsetattr(0, TCSANOW, &restore);
    #endif
    textlock = false;
    goto noerr;
}
if (chkCmd(1, "_TXTATTRIB")) {
    if (argct < 1 || argct > 2) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {cerr = 3; goto cmderr;}
    int attrib = 0;
    if (argt[1] == 1) {
        for (int32_t i = 0; arg[1][i]; i++) {
            if (arg[1][i] >= 'a' && arg[1][i] <= 'z') arg[1][i] -= 32;
            if (arg[1][i] == ' ') arg[1][i] = '_';
        }
        if (!strcmp(arg[1], "RESET")) attrib = 0; else
        if (!strcmp(arg[1], "BOLD")) attrib = 1; else
        if (!strcmp(arg[1], "ITALIC")) attrib = 2; else
        if (!strcmp(arg[1], "UNDERLINE")) attrib = 3; else
        if (!strcmp(arg[1], "DBL_UNDERLINE") || !strcmp(arg[1], "DOUBLE_UNDERLINE")) attrib = 4; else
        if (!strcmp(arg[1], "SQG_UNDERLINE") || !strcmp(arg[1], "SQUIGGLY_UNDERLINE")) attrib = 5; else
        if (!strcmp(arg[1], "STRIKETHROUGH")) attrib = 6; else
        if (!strcmp(arg[1], "OVERLINE")) attrib = 7; else
        if (!strcmp(arg[1], "DIM")) attrib = 8; else
        if (!strcmp(arg[1], "BLINK")) attrib = 9; else
        if (!strcmp(arg[1], "HIDDEN")) attrib = 10; else
        if (!strcmp(arg[1], "REVERSE")) attrib = 11; else
        if (!strcmp(arg[1], "UNDERLINE_COLOR")) attrib = 12; else
        if (!strcmp(arg[1], "FGC")) attrib = 13; else
        if (!strcmp(arg[1], "BGC")) attrib = 14; else
        if (!strcmp(arg[1], "TRUECOLOR") || !strcmp(arg[1], "TRUE_COLOR") || !strcmp(arg[1], "24BITCOLOR") || !strcmp(arg[1], "24BIT_COLOR")) attrib = 15; else
        {cerr = 16; goto cmderr;}
    } else {
        attrib = atoi(arg[1]);
        if (attrib < 0 || attrib > 12) {cerr = 16; goto cmderr;}
    }
    int val = 0;
    if (attrib == 0) {
        if (argct == 2) {cerr = 3; goto cmderr;}
        memset(&txtattrib, 0, sizeof(cb_txt));
        txtattrib.fgce = true;
        goto cmderr;
    } else if (argct != 2) {
        if (attrib == 12) {cerr = 16; goto cmderr;}
        val = 1;
    } else {
        if (!solvearg(2)) goto cmderr;
        if (attrib == 12) {
            if (argt[2] != 2) {cerr = 2; goto cmderr;}
            val = atoi(arg[2]);
            if (val < 0 || val > 255) {cerr = 16; goto cmderr;}
        } else {
            if (argt[2] == 0) {cerr = 3; goto cmderr;}
            if (argt[2] == 1) {
                upCase(arg[2]);
                if (!strcmp(arg[2], "ON") || !strcmp(arg[2], "TRUE") || !strcmp(arg[2], "YES")) val = 1; else
                if (!strcmp(arg[2], "OFF") || !strcmp(arg[2], "FALSE") || !strcmp(arg[2], "NO")) val = 0; else
                {cerr = 16; goto cmderr;}
            } else {
                sscanf(arg[2], "%d", &val);
                if (val < 0 || val > 1) {cerr = 16; goto cmderr;}
            }
        }
    }
    switch (attrib) {
        case 1: txtattrib.bold = (bool)val; break;
        case 2: txtattrib.italic = (bool)val; break;
        case 3: txtattrib.underln = (bool)val; break;
        case 4: txtattrib.underlndbl = (bool)val; break;
        case 5: txtattrib.underlnsqg = (bool)val; break;
        case 6: txtattrib.strike = (bool)val; break;
        case 7: txtattrib.overln = (bool)val; break;
        case 8: txtattrib.dim = (bool)val; break;
        case 9: txtattrib.blink = (bool)val; break;
        case 10: txtattrib.hidden = (bool)val; break;
        case 11: txtattrib.reverse = (bool)val; break;
        case 12: txtattrib.underlncolor = val; break;
        case 13: txtattrib.fgce = (bool)val; break;
        case 14: txtattrib.bgce = (bool)val; break;
        case 15: txtattrib.truecolor = (bool)val; break;
    }
    updateTxtAttrib();
    goto noerr;
}
if (chkCmd(1, "_SHATTRIB")) {
    if (argct < 1 || argct > 2) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {cerr = 3; goto cmderr;}
    int attrib = 0;
    if (argt[1] == 1) {
        for (int32_t i = 0; arg[1][i]; i++) {
            if (arg[1][i] >= 'a' && arg[1][i] <= 'z') arg[1][i] -= 32;
            if (arg[1][i] == ' ') arg[1][i] = '_';
        }
        if (!strcmp(arg[1], "RESET")) attrib = 0; else
        if (!strcmp(arg[1], "SILENT")) attrib = 1; else
        if (!strcmp(arg[1], "CLEARATTRIB")) attrib = 2; else
        if (!strcmp(arg[1], "RESTOREATTRIB")) attrib = 3; else
        {cerr = 16; goto cmderr;}
    } else {
        attrib = atoi(arg[1]);
        if (attrib < 0 || attrib > 12) {cerr = 16; goto cmderr;}
    }
    int val = 0;
    if (attrib == 0) {
        if (argct == 2) {cerr = 3; goto cmderr;}
        sh_silent = false;
        sh_clearAttrib = true;
        sh_restoreAttrib = true;
        goto cmderr;
    } else if (argct != 2) {
        if (attrib == 12) {cerr = 16; goto cmderr;}
        val = 1;
    } else {
        if (!solvearg(2)) goto cmderr;
        if (argt[2] == 0) {cerr = 3; goto cmderr;}
        if (argt[2] == 1) {
            upCase(arg[2]);
            if (!strcmp(arg[2], "ON") || !strcmp(arg[2], "TRUE") || !strcmp(arg[2], "YES")) val = 1; else
            if (!strcmp(arg[2], "OFF") || !strcmp(arg[2], "FALSE") || !strcmp(arg[2], "NO")) val = 0; else
            {cerr = 16; goto cmderr;}
        } else {
            sscanf(arg[2], "%d", &val);
            if (val < 0 || val > 1) {cerr = 16; goto cmderr;}
        }
    }
    switch (attrib) {
        case 1: sh_silent = (bool)val; break;
        case 2: sh_clearAttrib = (bool)val; break;
        case 3: sh_restoreAttrib = (bool)val; break;
    }
    updateTxtAttrib();
    goto noerr;
}
