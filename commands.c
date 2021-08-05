if (chkCmd(2, "EXIT", "QUIT")) {
    if (argct > 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (argct == 1) {if (!solvearg(1)) goto cmderr; err = atoi(arg[1]);}
    if (inProg) {inProg = false; goto cmderr;}
    cleanExit();
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
if (chkCmd(1, "DIM")) {
    if (argct != 3) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(2)) goto cmderr;
    if (!solvearg(3)) goto cmderr;
    if (argt[2] != 2) {cerr = 2; goto cmderr;}
    int32_t asize = atoi(arg[2]);
    if (asize < 0) {cerr = 16; goto cmderr;}
    if (!setVar(tmpargs[1], arg[3], argt[3], asize)) goto cmderr;
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
    int tmp = 0;
    if (argt[1] == 0) {} else
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {
        tmp = atoi(arg[1]);
        if (tmp < 0 || tmp > 255) {cerr = 16; goto cmderr;}
        fgc = (uint8_t)tmp;
        #ifndef _WIN_NO_VT
        if (txt_fgc) printf("\e[38;5;%um", fgc);
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
            if (tmp < 0 || tmp > 255) {cerr = 16; goto cmderr;}
            bgc = (uint8_t)tmp;
            #ifndef _WIN_NO_VT
            if (txt_bgc) printf("\e[48;5;%um", bgc);
            #else
            updateTxtAttrib();
            #endif
        }
    }
    fflush(stdout);
    goto noerr;
}
if (chkCmd(1, "LOCATE")) {
    cerr = 0;
    int tmp = 0;
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {} else
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {
        tmp = atoi(arg[1]);
        if (tmp < 1) {cerr = 16; goto cmderr;}
        curx = tmp;
    }
    if (argct > 1) {
        if (!solvearg(2)) goto cmderr;
        if (argt[2] == 0) {} else
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
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
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
    uint8_t tbgc = bgc;
    if (argct) {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 2) {cerr = 2; goto cmderr;}
        tbgc = (uint8_t)atoi(arg[1]);
    }
    #ifndef _WIN_NO_VT
    if (argct) printf("\e[48;5;%um", tbgc);
    fputs("\e[H\e[2J\e[3J", stdout);
    if (txt_bgc) printf("\e[48;5;%um", bgc);
    fflush(stdout);
    #else
	SetConsoleTextAttribute(hConsole, (fgc % 16) + (tbgc % 16) * 16);
    system("cls");
    updateTxtAttrib();
    #endif
    goto noerr;
}
if (chkCmd(1, "WAITUS")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    cb_wait(d);
    goto noerr;
}
if (chkCmd(1, "WAITMS")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    cb_wait(d * 1000);
    goto noerr;
}
if (chkCmd(1, "WAIT")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    cb_wait(d * 1000000);
    goto noerr;
}
if (chkCmd(1, "RESETTIMER")) {
    cerr = 0;
    if (argct) {cerr = 3; goto cmderr;}
    resetTimer();
    goto noerr;
}
if (chkCmd(2, "SRAND", "SRND")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    double rs;
    sscanf(arg[1], "%lf", &rs);
    srand(rs);
    goto noerr;
}
if (chkCmd(1, "CALL")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    inprompt = !runfile;
    signal(SIGINT, cleanExit);
    if (!loadProg(arg[1])) goto cmderr;
    chkinProg = true;
    cp = 0;
    didloop = true;
    goto noerr;
}
if (chkCmd(1, "RUN")) {
    cerr = 0;
    if (argct < 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN32
    char** runargs = (char**)malloc((argct + 3) * sizeof(char*));
    runargs[0] = startcmd;
    runargs[1] = "-x";
    runargs[2] = arg[1];
    argct += 2;
    int argno;
    for (argno = 3; argno < argct; argno++) {
        if (!solvearg(argno - 1)) {free(runargs); goto cmderr;}
        runargs[argno] = arg[argno - 1];
    }
    argct -= 2;
    runargs[argno] = NULL;
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        execvp(startcmd, runargs);
        exit(0);
    }
    if (pid > 0) {
        pid = wait(&status);
        (void)status;
    }
    free(runargs);
    if (pid < 0) {
        cerr = -1;
        goto cmderr;
    }
    #else
    char* tmpcmd = malloc(CB_BUF_SIZE);
    copyStr(startcmd, tmpcmd);
    copyStrApnd(" -x ", tmpcmd);
    copyStrApnd(arg[1], tmpcmd);
    int32_t strpos = strlen(tmpcmd);
    for (int argno = 2; argno <= argct; argno++) {
        tmpcmd[strpos] = ' ';
        strpos++;
        solvearg(argno);
        for (int32_t i = 0; arg[argno][i] && strpos < CB_BUF_SIZE - 1; i++) {
            if (arg[argno][i] == ' ' || arg[argno][i] == '^') {
                tmpcmd[strpos] = '^';
                strpos++;
            }
            tmpcmd[strpos] = arg[argno][i];
            strpos++;
        }
    }
    tmpcmd[strpos] = 0;
    puts(tmpcmd);
    int ret = system(tmpcmd);
    (void)ret;
    free(tmpcmd);
    #endif
    updateTxtAttrib();
    goto noerr;
}
if (chkCmd(2, "SH", "EXEC")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    #ifndef _WIN_NO_VT
    if (sh_clearAttrib) fputs("\e[0m", stdout);
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
    cerr = system(arg[1]);
    dup2(duperr, 2);
    close(duperr);
    if (sh_restoreAttrib) updateTxtAttrib();
    cerr = 0;
    goto noerr;
}
if (chkCmd(1, "FILES")) {
    cerr = 0;
    DIR* cwd = opendir(".");
    if (!cwd) {cerr = 20; goto cmderr;}
    DIR* tmpdir;
    struct dirent* dir;
    #ifdef _WIN32
        #define DIRPFS "%s\\\n"
        puts(".\\\n..\\");
    #else
        #define DIRPFS "%s/\n"
        puts("./\n../");
    #endif
    while ((dir = readdir(cwd))) {
        if ((tmpdir = opendir(dir->d_name)) && strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) printf(DIRPFS, dir->d_name);
        if (tmpdir) closedir(tmpdir);
    }
    closedir(cwd);
    cwd = opendir(".");
    while ((dir = readdir(cwd))) {
        if (!(tmpdir = opendir(dir->d_name))) puts(dir->d_name);
        if (tmpdir) closedir(tmpdir);
    }
    closedir(cwd);
    goto noerr;
}
if (chkCmd(2, "CHDIR", "CD")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    if (chdir(arg[1])) {
        seterrstr(arg[1]);
        cerr = 17;
        goto cmderr;
    }
    goto noerr;
}
if (chkCmd(1, "_RESETTITLE")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
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
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
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
if (chkCmd(1, "_PROMPT")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    copyStr(tmpargs[1], prompt);
    goto noerr;
}
if (chkCmd(1, "_PROMPTTAB")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    tab_width = atoi(arg[1]);
}
if (chkCmd(1, "_AUTOCMDHIST")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    autohist = true;
    goto noerr;
}
if (chkCmd(1, "_SAVECMDHIST")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
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
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
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
        txt_fgc = true;
        txt_bgc = false;
        txt_underlncolor = 0;
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
        case 9: txt_hidden = (bool)val; break;
        case 11: txt_reverse = (bool)val; break;
        case 12: txt_underlncolor = val; break;
        case 13: txt_fgc = (bool)val; break;
        case 14: txt_bgc = (bool)val; break;
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
