if (chkCmd(2, arg[0], "EXIT", "QUIT")) {
    if (argct > 1) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (argct == 1) {if (!solvearg(1)) goto cmderr; err = atoi(arg[1]);}
    if (inProg) {inProg = false; goto cmderr;}
    cleanExit();
}
if (chkCmd(1, arg[0], "PUT")) {
    cerr = 0;
    for (int i = 1; i <= argct; i++) {if (!solvearg(i)) {goto cmderr;} fputs(arg[i], stdout);}
    fflush(stdout);
    goto cmderr;
}
if (chkCmd(2, arg[0], "SET", "LET")) {
    if (argct != 2) {cerr = 3; goto cmderr;}
    cerr = 0;
    solvearg(1);
    if (!solvearg(2)) goto cmderr;
    if (argt[1] == 0 || argt[2] == 0) {cerr = 1; goto cmderr;}
    int v = -1;
    for (int i = 0; i < varmaxct; i++) {
        if (varinuse[i] && !strcmp(arg[1], varname[i])) {v = i; break;}
    }
    if (argt[1] != argt[2] && v != -1) {cerr = 2; goto cmderr;}
    if (getType(tmpargs[1]) != 255) {
        cerr = 4;
        errstr = realloc(errstr, (strlen(tmpargs[1]) + 1) * sizeof(char));
        copyStr(tmpargs[1], errstr);
        goto cmderr;
    }
    if (!setVar(tmpargs[1], arg[2], argt[2])) goto cmderr;
    goto cmderr;
}
if (chkCmd(1, arg[0], "DEL")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!delVar(tmpargs[1])) goto cmderr;
    goto cmderr;
}
if (chkCmd(1, arg[0], "COLOR")) {
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
        printf("\e[38;5;%um", fgc);
    }
    if (argct > 1) {
        if (!solvearg(2)) goto cmderr;
        if (argt[2] == 0) {} else
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {
            tmp = atoi(arg[2]);
            if (tmp < 0 || tmp > 255) {cerr = 16; goto cmderr;}
            bgc = (uint8_t)tmp;
            printf("\e[48;5;%um", bgc);
        }
    }
    fflush(stdout);
    goto cmderr;
}
if (chkCmd(1, arg[0], "LOCATE")) {
    cerr = 0;
    int tmp = 0;
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {} else
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    else {
        tmp = atoi(arg[1]);
        if (tmp < 0) {cerr = 16; goto cmderr;}
        curx = tmp;
    }
    if (argct > 1) {
        if (!solvearg(2)) goto cmderr;
        if (argt[2] == 0) {} else
        if (argt[2] != 2) {cerr = 2; goto cmderr;}
        else {
            tmp = atoi(arg[2]);
            if (tmp < 0) {cerr = 16; goto cmderr;}
            cury = tmp;
        }
    }
    printf("\e[%d;%dH", cury, curx);
    fflush(stdout);
    goto cmderr;
}
if (chkCmd(1, arg[0], "CLS")) {
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
    uint8_t tbgc = bgc;
    if (argct) {
        if (!solvearg(1)) goto cmderr;
        if (argt[1] != 2) {cerr = 2; goto cmderr;}
        tbgc = (uint8_t)atoi(arg[1]);
    }
    printf("\e[48;5;%um\e[H\e[2J\e[3J\e[48;5;%um", tbgc, bgc);
    fflush(stdout);
    goto cmderr;
}
if (chkCmd(1, arg[0], "WAITUS")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    wait(d);
    goto cmderr;
}
if (chkCmd(1, arg[0], "WAITMS")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    wait(d * 1000);
    goto cmderr;
}
if (chkCmd(1, arg[0], "WAIT")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    wait(d * 1000000);
    goto cmderr;
}
if (chkCmd(1, arg[0], "RESETTIMER")) {
    cerr = 0;
    if (argct) {cerr = 3; goto cmderr;}
    resetTimer();
    goto cmderr;
}
if (chkCmd(2, arg[0], "SH", "EXEC")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    if (sh_clearAttrib) fputs("\e[0m", stdout);
    fflush(stdout);
    #ifdef _WIN32
    if (sh_silent) {arg[1] = realloc(arg[1], sizeof(arg[1]) + 13); copyStrApnd(arg[1], " 1>nul 2>nul");}
    #else
    if (sh_silent) {arg[1] = realloc(arg[1], sizeof(arg[1]) + 13); copyStrApnd(arg[1], " &>/dev/null");}
    #endif
    fflush(stdout);
    cerr = system(arg[1]);
    if (sh_restoreAttrib) updateTxtAttrib();
    cerr = 0;
    goto cmderr;
}
if (chkCmd(1, arg[0], "RUN")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    prog = fopen(arg[1], "r");
    if (!prog) {
        errstr = realloc(errstr, (argl[1] + 1) * sizeof(char));
        copyStr(arg[1], errstr);
        cerr = 15;
        goto cmderr;
    }
    progFilename = malloc(argl[1] + 1);
    copyStr(arg[1], progFilename);
    if (!isFile(progFilename)) {cerr = 18; goto cmderr;}
    loadProg();
    chkinProg = true;
    goto cmderr;
}
if (chkCmd(2, arg[0], "SRAND", "SRND")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    double rs;
    sscanf(arg[1], "%lf", &rs);
    srand(rs);
    goto cmderr;
}
if (chkCmd(1, arg[0], "FILES")) {
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
    goto cmderr;
}
if (chkCmd(2, arg[0], "CHDIR", "CD")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    if (chdir(arg[1])) {
        errstr = realloc(errstr, (argl[1] + 1) * sizeof(char));
        copyStr(arg[1], errstr);
        cerr = 17;
        goto cmderr;
    }
    goto cmderr;
}
if (chkCmd(1, arg[0], "_PROMPT")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    copyStr(tmpargs[1], prompt);
    goto cmderr;
}
if (chkCmd(1, arg[0], "_PROMPTTAB")) {
    if (inProg) {cerr = 254; goto cmderr;}
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    tab_width = atoi(arg[1]);
    cerr = 0;
}
if (chkCmd(1, arg[0], "_AUTOHIST")) {
    if (inProg) {cerr = 254; goto cmderr;}
    autohist = true;
    goto cmderr;
}
if (chkCmd(1, arg[0], "_SAVECMDHIST")) {
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
        write_history(".clibasic_history");
        ret = chdir(tmpcwd);
        (void)ret;
    }
    goto cmderr;
}
if (chkCmd(1, arg[0], "_LOADCMDHIST")) {
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
    goto cmderr;
}
if (chkCmd(1, arg[0], "_TXTLOCK")) {
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
    goto cmderr;
}
if (chkCmd(1, arg[0], "_TXTUNLOCK")) {
    if (argct) {cerr = 3; goto cmderr;}
    cerr = 0;
    #ifndef _WIN32
    if (textlock) tcsetattr(0, TCSANOW, &restore);
    #endif
    textlock = false;
    goto cmderr;
}
if (chkCmd(1, arg[0], "_TXTATTRIB")) {
    if (argct < 1 || argct > 2) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {cerr = 3; goto cmderr;}
    int attrib = 0;
    if (argt[1] == 1) {
        for (int i = 0; arg[1][i]; i++) {
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
    }
    updateTxtAttrib();
    goto cmderr;
}
if (chkCmd(1, arg[0], "_SHATTRIB")) {
    if (argct < 1 || argct > 2) {cerr = 3; goto cmderr;}
    cerr = 0;
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {cerr = 3; goto cmderr;}
    int attrib = 0;
    if (argt[1] == 1) {
        for (int i = 0; arg[1][i]; i++) {
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
    goto cmderr;
}
if (chkCmd(1, arg[0], "_DEBUGON")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct) {cerr = 3; goto cmderr;}
    if (!debug) puts("Enabled debug mode.");
    debug = true;
    goto cmderr;
}
if (chkCmd(1, arg[0], "_DEBUGOFF")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct) {cerr = 3; goto cmderr;}
    if (debug) puts("Disabled debug mode.");
    debug = false;
    goto cmderr;
}
