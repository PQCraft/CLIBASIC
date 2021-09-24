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
        else inProg = false;
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
    if (!setVar(tmpargs[1], arg[2], argt[2], -1)) goto cmderr;
    goto noerr;
}
if (chkCmd(3, "@", "LABEL", "LBL")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    int i = -1;
    for (int j = 0; j < gotomaxct; ++j) {
        if (!gotodata[j].used) {i = j; break;}
        else if (!strcmp(gotodata[j].name, tmpargs[1])) {
            if (gotodata[j].cp == cmdpos) {goto noerr;}
            cerr = 28; goto cmderr;
        }
    }
    if (i == -1) {
        i = gotomaxct;
        ++gotomaxct;
        gotodata = realloc(gotodata, gotomaxct * sizeof(cb_goto));
    }
    gotodata[i].name = malloc(argl[i] + 1);
    copyStr(tmpargs[1], gotodata[i].name);
    gotodata[i].cp = cmdpos;
    gotodata[i].pl = progLine;
    gotodata[i].used = true;
    gotodata[i].dlsp = dlstackp;
    gotodata[i].fnsp = fnstackp;
    gotodata[i].itsp = itstackp;
    goto noerr;
}
if (chkCmd(3, "%", "GOTO", "GO")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    int i = -1;
    for (int j = 0; j < gotomaxct; ++j) {
        if (gotodata[j].used) {
            if (!strcmp(gotodata[j].name, tmpargs[1])) {i = j;}
        }
    }
    if (i == -1) {cerr = 29; goto cmderr;}
    if (inProg) {
        cp = gotodata[i].cp;
    } else {
        concp = gotodata[i].cp;
    }
    progLine = gotodata[i].pl;
    dlstackp = gotodata[i].dlsp;
    fnstackp = gotodata[i].fnsp;
    itstackp = gotodata[i].itsp;
    gotodata[i].used = false;
    bool r = false;
    while (gotomaxct > 0 && !gotodata[gotomaxct - 1].used) {--gotomaxct; r = true;}
    if (r) gotodata = realloc(gotodata, gotomaxct * sizeof(cb_goto));
    didloop = true;
    lockpl = true;
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
    if (!tmpargs[1][0]) {cerr = 4; seterrstr(""); goto cmderr;}
    if (!setVar(tmpargs[1],\
    ((argct == 3) ? arg[3] : ((tmpargs[1][argl[1] - 1] == '$') ? "" : "0")),\
    ((argct == 3) ? argt[3] : 2 - (tmpargs[1][argl[1] - 1] == '$')),\
    asize)) goto cmderr;
    goto noerr;
}
if (chkCmd(1, "DEL")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!delVar(tmpargs[1])) goto cmderr;
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
        if (txt_truecolor) {
            if (tmp < 0 || tmp > 0xFFFFFF) {cerr = 16; goto cmderr;}
            truefgc = tmp;
        } else {
            if (tmp < 0 || tmp > 255) {cerr = 16; goto cmderr;}
            fgc = (uint8_t)tmp;
        }
        #ifndef _WIN_NO_VT
        if (txt_fgc) {
            if (txt_truecolor) printf("\e[38;2;%u;%u;%um", (uint8_t)(truefgc >> 16), (uint8_t)(truefgc >> 8), (uint8_t)truefgc);
            else printf("\e[38;5;%um", fgc);
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
            if (txt_truecolor) {
                if (tmp < 0 || tmp > 0xFFFFFF) {cerr = 16; goto cmderr;}
                truebgc = tmp;
            } else {
                if (tmp < 0 || tmp > 255) {cerr = 16; goto cmderr;}
                bgc = (uint8_t)tmp;
            }
            #ifndef _WIN_NO_VT
            if (txt_bgc) {
                if (txt_truecolor) printf("\e[48;2;%u;%u;%um", (uint8_t)(truebgc >> 16), (uint8_t)(truebgc >> 8), (uint8_t)truebgc);
                else printf("\e[48;5;%um", bgc);
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
    getCurPos();
    if (argt[1] == 0) {}
    else if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {
        tmp = atoi(arg[1]);
        if (tmp < 1) {cerr = 16; goto cmderr;}
        curx = tmp;
    }
    if (argct > 1) {
        if (!solvearg(2)) goto cmderr;
        if (argt[2] == 0) {}
        else if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {
            tmp = atoi(arg[2]);
            if (tmp < 1) {cerr = 16; goto cmderr;}
            cury = tmp;
        }
    }
    #ifndef _WIN_NO_VT
    printf("\e[%d;%dH", cury, curx);
    #else
    SetConsoleCursorPosition(hConsole, (COORD){curx - 1, cury - 1});
    #endif
    fflush(stdout);
    goto noerr;
}
if (chkCmd(1, "CLS")) {
    if (argct > 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    uint8_t tbgc = bgc;
    #ifndef _WIN_NO_VT
    uint32_t ttbgc = truebgc;
    #endif
    if (argct) {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 2) {cerr = 2; goto cmderr;}
        #ifndef _WIN_NO_VT
        if (txt_truecolor) {
            ttbgc = (uint32_t)atoi(arg[1]);
        } else {
            tbgc = (uint8_t)atoi(arg[1]);
        }
        #else
        tbgc = (uint8_t)atoi(arg[1]);
        #endif
    }
    #ifndef _WIN_NO_VT
    if (argct) {
        if (txt_truecolor) printf("\e[48;2;%u;%u;%um", (uint8_t)(ttbgc >> 16), (uint8_t)(ttbgc >> 8), (uint8_t)ttbgc);
        else printf("\e[48;5;%um", tbgc);
    }
    fputs("\e[H\e[2J\e[3J", stdout);
    updateTxtAttrib();
    fflush(stdout);
    #else
	SetConsoleTextAttribute(hConsole, (fgc % 16) + (tbgc % 16) * 16);
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
if (chkCmd(1, "CALL")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    inprompt = !runfile;
    setsig(SIGINT, cleanExit);
    if (!loadProg(arg[1])) goto cmderr;
    chkinProg = true;
    cp = 0;
    didloop = true;
    goto noerr;
}
if (chkCmd(1, "RUN")) {
    if (argct < 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN32
    char** runargs = (char**)malloc((argct + 3) * sizeof(char*));
    runargs[0] = startcmd;
    runargs[1] = "-x";
    runargs[2] = arg[1];
    argct += 2;
    int argno = 3;
    for (; argno < argct; argno++) {
        if (!solvearg(argno - 1)) {free(runargs); goto cmderr;}
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
        if (argno > 1) solvearg(argno);
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
    updateTxtAttrib();
    goto noerr;
}
if (chkCmd(2, "$", "SH")) {
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN_NO_VT
    if (sh_clearAttrib) fputs("\e[0m", stdout);
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
if (chkCmd(1, "EXEC")) {
    if (argct < 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN_NO_VT
    if (sh_clearAttrib) fputs("\e[0m", stdout);
    #else
    if (sh_clearAttrib) SetConsoleTextAttribute(hConsole, ocAttrib);
    #endif
    fflush(stdout);
    #ifndef _WIN32
    char** runargs = (char**)malloc((argct + 1) * sizeof(char*));
    runargs[0] = arg[1];
    int argno = 1;
    for (; argno < argct; ++argno) {
        if (!solvearg(argno + 1)) {free(runargs); goto cmderr;}
        runargs[argno] = arg[argno + 1];
    }
    runargs[argno] = NULL;
    int stdout_dup = 0, stderr_dup = 0;
    if (sh_silent) {
        stdout_dup = dup(1);
        stderr_dup = dup(2);
        int fd = open("/dev/null", O_WRONLY | O_CREAT);
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
        if (argno > 1) if (!solvearg(argno)) {free(tmpcmd); goto cmderr;};
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
    if (sh_restoreAttrib) updateTxtAttrib();
    if (cerr) goto cmderr;
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
        getcwd(olddn, CB_BUF_SIZE);
        if (chdir(arg[1])) {
            free(olddn);
            seterrstr(arg[1]);
            cerr = 17;
            goto cmderr;
        }
    }
    DIR* cwd = opendir(".");
    if (!cwd) {if (argct) {chdir(olddn); free(olddn);} goto noerr;}
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
        chdir(olddn);
        free(olddn);
    }
    closedir(cwd);
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
goto cmderr;
_cmd:
if (chkCmd(1, "_RESETTITLE")) {
    if (inProg) {cerr = 254; goto cmderr;}
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    #ifndef _WIN_NO_VT
    if (!changedtitle) {
        if (changedtitlecmd) fputs("\e[23;0t", stdout);
        goto noerr;
    }
    printf("\e]2;CLIBASIC %s (%s-bit)%c", VER, BVER, 7);
    fflush(stdout);
    #else
    char* tmpstr = malloc(CB_BUF_SIZE);
    sprintf(tmpstr, "CLIBASIC %s (%s-bit)", VER, BVER);
    SetConsoleTitleA(tmpstr);
    free(tmpstr);
    #endif
    goto noerr;
}
if (chkCmd(1, "_TITLE")) {
    if (inProg) {cerr = 254; goto cmderr;}
    if (argct != 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN_NO_VT
    if (!changedtitle) {
        fputs("\e[22;0t", stdout);
        fflush(stdout);
        changedtitle = true;
    }
    changedtitlecmd = true;
    printf("\e]2;%s%c", arg[1], 7);
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
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    copyStr(tmpargs[1], prompt);
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
    if (inProg) {cerr = 254; goto cmderr;}
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
    if (inProg) {cerr = 254; goto cmderr;}
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
        if (!strcmp(arg[1], "STRIKETROUGH")) attrib = 6; else
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
        txt_bold = false;
        txt_italic = false;
        txt_underln = false;
        txt_underlndbl = false;
        txt_underlnsqg = false;
        txt_strike = false;
        txt_overln = false;
        txt_dim = false;
        txt_blink = false;
        txt_hidden = false;
        txt_reverse = false;
        txt_underlncolor = 0;
        txt_fgc = true;
        txt_bgc = false;
        txt_truecolor = false;
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
        case 1: txt_bold = (bool)val; break;
        case 2: txt_italic = (bool)val; break;
        case 3: txt_underln = (bool)val; break;
        case 4: txt_underlndbl = (bool)val; break;
        case 5: txt_underlnsqg = (bool)val; break;
        case 6: txt_strike = (bool)val; break;
        case 7: txt_overln = (bool)val; break;
        case 8: txt_dim = (bool)val; break;
        case 9: txt_blink = (bool)val; break;
        case 10: txt_hidden = (bool)val; break;
        case 11: txt_reverse = (bool)val; break;
        case 12: txt_underlncolor = val; break;
        case 13: txt_fgc = (bool)val; break;
        case 14: txt_bgc = (bool)val; break;
        case 15: txt_truecolor = (bool)val; break;
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
