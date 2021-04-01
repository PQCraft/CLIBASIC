if (chkCmd(2, arg[0], "EXIT", "QUIT")) {
    cerr = 0;
    if (argct > 1) {cerr = 3; goto cmderr;}
    if (argct == 1) {if (!solvearg(1)) goto cmderr; err = atoi(arg[1]);}
    if (inProg) {inProg = false; goto cmderr;}
    cleanExit();
}
if (chkCmd(1, arg[0], "PUT")) {
    cerr = 0;
    for (int i = 1; i <= argct; i++) {if (!solvearg(i)) goto cmderr; printf("%s", arg[i]);}
    fflush(stdout);
    goto cmderr;
}
if (chkCmd(2, arg[0], "SET", "LET")) {
    cerr = 0;
    if (argct != 2) {cerr = 3; goto cmderr;}
    solvearg(1);
    if (!solvearg(2)) goto cmderr;
    if (argt[1] == 0 || argt[2] == 0) {cerr = 1; goto cmderr;}
    int v = -1;
    for (int i = 0; i < varmaxct; i++) {
        if (varinuse[i] && !qstrcmp(arg[1], varname[i])) {v = i; break;}
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
if (chkCmd(1, arg[0], "COLOR")) {
    cerr = 0;
    int tmp = 0;
    if (argct > 2 || argct < 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
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
    if (argct != 0) {
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
    uint64_t t = d + time_us();
    while (t > time_us()) {}
    goto cmderr;
}
if (chkCmd(1, arg[0], "WAITMS")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    uint64_t t = d * 1000 + time_us();
    while (t > time_us()) {}
    goto cmderr;
}
if (chkCmd(1, arg[0], "WAIT")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 2) {cerr = 2; goto cmderr;}
    uint64_t d;
    sscanf(arg[1], "%llu", (long long unsigned *)&d);
    uint64_t t = d * 1000000 + time_us();
    while (t > time_us()) {}
    goto cmderr;
}
if (chkCmd(1, arg[0], "RESETTIMER")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    resetTimer();
    goto cmderr;
}
if (chkCmd(2, arg[0], "SH", "EXEC")) {
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    if (sh_clearAttrib) fputs("\e[0m", stdout);
    #ifdef _WIN32
    if (sh_silent) {arg[1] = realloc(arg[1], sizeof(arg[1]) + 13); copyStrApnd(arg[1], " 1>nul 2>nul");}
    #else
    if (sh_silent) {arg[1] = realloc(arg[1], sizeof(arg[1]) + 13); copyStrApnd(arg[1], " &>/dev/null");}
    #endif
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
    if (prog == NULL) {errstr = realloc(errstr, (argl[1] + 1) * sizeof(char)); copyStr(arg[1], errstr); cerr = 15; goto cmderr;}
    progFilename = malloc(argl[1] + 1);
    copyStr(arg[1], progFilename);
    printf("Loading...");
    fflush(stdout);
    loadProg();
    for (uint8_t i = 0; i < 10; i++) printf("\b");
    for (uint8_t i = 0; i < 10; i++) printf(" ");
    for (uint8_t i = 0; i < 10; i++) printf("\b");
    fflush(stdout);
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
if (chkCmd(1, arg[0], "_PROMPT")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    copyStr(tmpargs[1], prompt);
    goto cmderr;
}
if (chkCmd(1, arg[0], "_SAVECMDHST")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    write_history(arg[1]);
    goto cmderr;
}
if (chkCmd(1, arg[0], "_LOADCMDHST")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 1) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] != 1) {cerr = 2; goto cmderr;}
    read_history(arg[1]);
    goto cmderr;
}
if (chkCmd(1, arg[0], "_TXTLOCK")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (!textlock) {
        tcgetattr(0, &term);
        tcgetattr(0, &restore);
        term.c_lflag &= ~(ICANON|ECHO);
        tcsetattr(0, TCSANOW, &term);
    }
    textlock = true;
    goto cmderr;
}
if (chkCmd(1, arg[0], "_TXTUNLOCK")) {
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (textlock) tcsetattr(0, TCSANOW, &restore);
    textlock = false;
    goto cmderr;
}

if (chkCmd(1, arg[0], "_TXTATTRIB")) {
    cerr = 0;
    if (argct < 1 || argct > 2) {cerr = 3; goto cmderr;}
    if (!solvearg(1)) goto cmderr;
    if (argt[1] == 0) {cerr = 3; goto cmderr;}
    int attrib = 0;
    if (argt[1] == 1) {
        for (int i = 0; arg[1][i] != 0; i++) {
            if (arg[1][i] >= 'a' && arg[1][i] <= 'z') arg[1][i] -= 32;
            if (arg[1][i] == ' ') arg[1][i] = '_';
        }
        if (!qstrcmp(arg[1], "RESET")) attrib = 0; else
        if (!qstrcmp(arg[1], "BOLD")) attrib = 1; else
        if (!qstrcmp(arg[1], "ITALIC")) attrib = 2; else
        if (!qstrcmp(arg[1], "UNDERLINE")) attrib = 3; else
        if (!qstrcmp(arg[1], "DBL_UNDERLINE") || !qstrcmp(arg[1], "DOUBLE_UNDERLINE")) attrib = 4; else
        if (!qstrcmp(arg[1], "SQG_UNDERLINE") || !qstrcmp(arg[1], "SQUIGGLY_UNDERLINE")) attrib = 5; else
        if (!qstrcmp(arg[1], "STRIKETROUGH")) attrib = 6; else
        if (!qstrcmp(arg[1], "OVERLINE")) attrib = 7; else
        if (!qstrcmp(arg[1], "DIM")) attrib = 8; else
        if (!qstrcmp(arg[1], "BLINK")) attrib = 9; else
        if (!qstrcmp(arg[1], "HIDDEN")) attrib = 10; else
        if (!qstrcmp(arg[1], "REVERSE")) attrib = 11; else
        if (!qstrcmp(arg[1], "UNDERLINE_COLOR")) attrib = 12; else
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
                if (!qstrcmp(arg[2], "ON") || !qstrcmp(arg[2], "TRUE") || !qstrcmp(arg[2], "YES")) val = 1; else
                if (!qstrcmp(arg[2], "OFF") || !qstrcmp(arg[2], "FALSE") || !qstrcmp(arg[2], "NO")) val = 0; else
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

if (chkCmd(1, arg[0], "_DEBUGON")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (!debug) printf("Enabled debug mode.\n");
    debug = true;
    goto cmderr;
}
if (chkCmd(1, arg[0], "_DEBUGOFF")) {
    if (inProg) {cerr = 254; goto cmderr;}
    cerr = 0;
    if (argct != 0) {cerr = 3; goto cmderr;}
    if (debug) printf("Disabled debug mode.\n");
    debug = false;
    goto cmderr;
}
