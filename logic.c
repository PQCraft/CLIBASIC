if (chkCmd(1, "REM")) return true;
if (chkCmd(2, "?", "PRINT") || cmd[i] == '?') {
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] : -1)) {
        if (dldcmd[dlstackp]) return true;
    }
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] : -1)) {
        if (itdcmd[itstackp]) return true;
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] : -1)) {
        if (fndcmd[fnstackp]) return true;
    }
    if (cmd[i] == '?') {j = i + 1;}
    {
        int32_t tmpj = j;
        while (cmd[tmpj] == ' ') ++tmpj;
        if (cmd[tmpj] == '=') return false;
    }
    while (cmd[j] == ' ') ++j;
    if (cmd[j] == 0) {putchar('\n'); return true;}
    else {j--;}
    bool newline = false;
    bool inStr = false;
    bool lookingForSpChar = false;
    bool sawSpChar = false;
    int pct = 0, bct = 0;
    int32_t ptr = 0;
    int32_t i = j;
    while (cmd[i]) {
        ++i;
        if (cmd[i] == '"') {inStr = !inStr;}
        if (!inStr) {
            switch (cmd[i]) {
                case '(': ++pct; break;
                case ')': --pct; break;
                case '[': ++bct; break;
                case ']': --bct; break;
            }
        }
        if ((cmd[i] == ',' || cmd[i] == ';' || cmd[i] == 0) && !inStr && pct == 0 && bct == 0) {
            ltmp[1][ptr] = 0;
            ptr = 0;
            int32_t len = strlen(ltmp[1]);
            int tmpt;
            if (!(tmpt = getVal(ltmp[1], ltmp[1]))) return true;
            newline = false;
            if (cmd[j] == ',') {
                if (tmpt == 255 && !cmd[j + 1]) {newline = true;}
                putchar('\t');
            }
            fputs(ltmp[1], stdout);
            if (cmd[i] == 0 && len > 0) putchar('\n');
            j = i;
            lookingForSpChar = false;
            sawSpChar = false;
        } else {
            if (!inStr) {
                if (cmd[i] == ' ' && !sawSpChar && ptr > 0) {lookingForSpChar = true;}
                if (isExSpChar(cmd[i])) {lookingForSpChar = false; sawSpChar = true;}
            }
            if (inStr || cmd[i] != ' ') {
                if (!isExSpChar(cmd[i])) sawSpChar = false;
                if (lookingForSpChar) {cerr = 1; return true;}
                ltmp[1][ptr] = cmd[i];
                ptr++;
            }
        }
    }
    if (newline) putchar('\n');
    if (pct || bct || inStr) {cerr = 1; return true;}
    fflush(stdout);
    return true;
}
if (chkCmd(1, "DO")) {
    if (dlstackp >= CB_PROG_LOGIC_MAX - 1) {cerr = 12; return true;}
    dlstackp++;
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] : -1)) {
        if (itdcmd[itstackp]) {dldcmd[dlstackp] = true; return true;}
    }
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] + 1 : 0)) {
        if (dldcmd[dlstackp - 1]) {dldcmd[dlstackp] = true; return true;}
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] : -1)) {
        if (fndcmd[fnstackp]) return true;
    }
    while (cmd[j]) {if (cmd[j] != ' ') {cerr = 3; return true;} ++j;}
    dldcmd[dlstackp] = false;
    dlstack[dlstackp].cp = cmdpos;
    dlstack[dlstackp].pl = progLine;
    dlstack[dlstackp].fnsp = fnstackp;
    dlstack[dlstackp].itsp = itstackp;
    dlstack[dlstackp].brkinfo = brkinfo;
    brkinfo.block = 1;
    #ifdef _WIN32
    updatechars();
    #endif
    return true;
}
if (chkCmd(2, "WHILE", "DOWHILE")) {
    if (dlstackp >= CB_PROG_LOGIC_MAX - 1) {cerr = 12; return true;}
    dlstackp++;
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] : -1)) {
        if (itdcmd[itstackp]) {dldcmd[dlstackp] = true; return true;}
    }
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] + 1 : 0)) {
        if (dldcmd[dlstackp - 1]) {dldcmd[dlstackp] = true; return true;}
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] : -1)) {
        if (fndcmd[fnstackp]) return true;
    }
    copyStrSnip(cmd, j + 1, strlen(cmd), ltmp[1]);
    if (getArgCt(ltmp[1]) != 1) {cerr = 3; return true;}
    uint8_t testval = logictest(ltmp[1]);
    if (testval == 255) return true;
    if (testval == 1) {
        dlstack[dlstackp].pl = progLine;
        dlstack[dlstackp].cp = cmdpos;
        dlstack[dlstackp].fnsp = fnstackp;
        dlstack[dlstackp].itsp = itstackp;
        dlstack[dlstackp].brkinfo = brkinfo;
        brkinfo.block = 1;
        #ifdef _WIN32
        updatechars();
        #endif
    }
    dldcmd[dlstackp] = !testval;
    return true;
}
if (chkCmd(1, "LOOP")) {
    if (dlstackp <= -1) {cerr = 6; return true;}
    bool brk = false;
    if (brkinfo.block == 1) {
        if (brkinfo.type >= 1) {dldcmd[dlstackp] = false;}
        if (brkinfo.type == 2) {brk = true;}
    }
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] : -1)) {
        if (dldcmd[dlstackp]) {dlstackp--; return true;}
    }
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] : -1)) {
        if (itdcmd[itstackp]) return true;
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] : -1)) {
        if (fndcmd[fnstackp]) return true;
    }
    while (cmd[j]) {if (cmd[j] != ' ') {cerr = 3; return true;} ++j;}
    if (!brk) {
        if (inProg) {
            cp = dlstack[dlstackp].cp;
        } else {
            concp = dlstack[dlstackp].cp;
        }
        progLine = dlstack[dlstackp].pl;
        fnstackp = dlstack[dlstackp].fnsp;
        itstackp = dlstack[dlstackp].itsp;
        brkinfo = dlstack[dlstackp].brkinfo;
        lockpl = true;
    }
    dldcmd[dlstackp] = false;
    didloop = true;
    dlstackp--;
    return true;
}
if (chkCmd(1, "LOOPWHILE")) {
    if (dlstackp <= -1) {cerr = 6; return true;}
    bool brk = false;
    if (brkinfo.block == 1) {
        if (brkinfo.type >= 1) {dldcmd[dlstackp] = false;}
        if (brkinfo.type == 2) {brk = true;}
    }
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] : -1)) {
        if (dldcmd[dlstackp]) {dlstackp--; return true;}
    }
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] : -1)) {
        if (itdcmd[itstackp]) return true;
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] : -1)) {
        if (fndcmd[fnstackp]) return true;
    }
    if (!brk) {
        copyStrSnip(cmd, j + 1, strlen(cmd), ltmp[1]);
        if (getArgCt(ltmp[1]) != 1) {cerr = 3; return true;}
        uint8_t testval = logictest(ltmp[1]);
        if (testval == 255) return true;
        if (testval == 1) {
            if (inProg) {
                cp = dlstack[dlstackp].cp;
            } else {
                concp = dlstack[dlstackp].cp;
            }
            progLine = dlstack[dlstackp].pl;
            fnstackp = dlstack[dlstackp].fnsp;
            itstackp = dlstack[dlstackp].itsp;
            brkinfo = dlstack[dlstackp].brkinfo;
            lockpl = true;
        }
    }
    dldcmd[dlstackp] = false;
    dlstackp--;
    didloop = true;
    return true;
}
if (chkCmd(1, "IF")) {
    if (itstackp >= CB_PROG_LOGIC_MAX - 1) {cerr = 13; return true;}
    itstackp++;
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] + 1 : 0)) {
        if (itdcmd[itstackp - 1]) {itdcmd[itstackp] = true; return true;}
    }
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] : -1)) {
        if (dldcmd[dlstackp]) {itdcmd[itstackp] = true; return true;}
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] : -1)) {
        if (fndcmd[fnstackp]) return true;
    }
    copyStrSnip(cmd, j + 1, strlen(cmd), ltmp[1]);
    if (getArgCt(ltmp[1]) != 1) {cerr = 3; return true;}
    uint8_t testval = logictest(ltmp[1]);
    if (testval == 255) return true;
    didelseif[itstackp] = testval;
    itdcmd[itstackp] = !testval;
    return true;
}
if (chkCmd(1, "ELSE")) {
    if (itstackp <= -1) {cerr = 8; return true;}
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] + 1 : 0)) {
        if (itdcmd[itstackp - 1]) return true;
    }
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] : -1)) {
        if (dldcmd[dlstackp]) return true;
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] : -1)) {
        if (fndcmd[fnstackp]) return true;
    }
    while (cmd[j]) {if (cmd[j] != ' ') {cerr = 3; return true;} ++j;}
    if (didelse[itstackp]) {cerr = 11; return true;}
    if (didelseif[itstackp]) {itdcmd[itstackp] = true; return true;}
    didelse[itstackp] = true;
    itdcmd[itstackp] = !itdcmd[itstackp];
    return true;
}
if (chkCmd(1, "ELSEIF")) {
    if (itstackp <= -1) {cerr = 8; return true;}
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] + 1 : 0)) {
        if (itdcmd[itstackp - 1]) return true;
    }
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] : -1)) {
        if (dldcmd[dlstackp]) return true;
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] : -1)) {
        if (fndcmd[fnstackp]) return true;
    }
    copyStrSnip(cmd, j + 1, strlen(cmd), ltmp[1]);
    if (getArgCt(ltmp[1]) != 1) {cerr = 3; return true;}
    if (didelseif[itstackp]) {itdcmd[itstackp] = true; return true;}
    uint8_t testval = logictest(ltmp[1]);
    if (testval == 255) return true;
    didelseif[itstackp] = testval;
    itdcmd[itstackp] = !testval;
    return true;
}
if (chkCmd(1, "ENDIF")) {
    if (itstackp <= -1) {cerr = 7; return true;}
    itstackp--;
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] : -1)) {
        if (dldcmd[dlstackp]) return true;
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] : -1)) {
        if (fndcmd[fnstackp]) return true;
    }
    while (cmd[j]) {if (cmd[j] != ' ') {cerr = 3; return true;} ++j;}
    itdcmd[itstackp + 1] = false;
    didelse[itstackp + 1] = false;
    didelseif[itstackp + 1] = false;
    return true;
}
if (chkCmd(1, "FOR")) {
    if (itstackp >= CB_PROG_LOGIC_MAX - 1) {cerr = 13; return true;}
    fnstack[fnstackp].brkinfo = brkinfo;
    brkinfo.type = 0;
    fnstackp++;
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] : -1)) {
        if (itdcmd[itstackp]) {return true;}
    }
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] : -1)) {
        if (dldcmd[dlstackp]) {return true;}
    }
    if (fnstackp > ((progindex > -1) ? minfnstackp[progindex] + 1 : 0)) {
        if (fndcmd[fnstackp - 1]) {return true;}
    }
    copyStrSnip(cmd, j + 1, strlen(cmd), ltmp[1]);
    if (getArgCt(ltmp[1]) != 4) {cerr = 3; return true;}
    cerr = 2;
    if (getArg(0, ltmp[1], fnvar) == -1) return true;
    if (getVar(fnvar, forbuf[0]) != 2) return true;
    if (getArg(1, ltmp[1], forbuf[1]) == -1) return true;
    if (getVal(forbuf[1], forbuf[1]) != 2) return true;
    if (getArg(3, ltmp[1], forbuf[3]) == -1) return true;
    if (getVal(forbuf[3], forbuf[3]) != 2) return true;
    setVar(fnvar, forbuf[1], 2, -1);
    if (fnstack[fnstackp].cp == -1) {
        copyStr(forbuf[1], forbuf[0]);
    }
    if (getArg(2, ltmp[1], forbuf[2]) == -1) return true;
    if (fninfor[fnstackp]) {
        sprintf(forbuf[0], "%lf", atof(forbuf[0]) + atof(forbuf[3]));
        setVar(fnvar, forbuf[0], 2, -1);
    }
    int testval = logictest(forbuf[2]);
    if (testval == 255) return true;
    fndcmd[fnstackp] = !testval;
    if (!(fninfor[fnstackp] = testval)) {cerr = 0; return true;}
    if (!fninfor[fnstackp] && fnstack[fnstackp].cp == -1) {
        sprintf(forbuf[0], "%lf", atof(forbuf[0]) - atof(forbuf[3]));
        setVar(fnvar, forbuf[0], 2, -1);
    }
    cerr = 0;
    fnstack[fnstackp].cp = cmdpos;
    fnstack[fnstackp].pl = progLine;
    fnstack[fnstackp].dlsp = dlstackp;
    fnstack[fnstackp].itsp = itstackp;
    brkinfo.block = 2;
    #ifdef _WIN32
    updatechars();
    #endif
    return true;
}
if (chkCmd(1, "NEXT")) {
    if (fnstackp <= -1) {cerr = 9; return true;}
    if (brkinfo.block == 2) {
        if (brkinfo.type >= 1) {fndcmd[dlstackp] = false;}
        if (brkinfo.type == 2) {fninfor[fnstackp] = false;}
    }
    fnstackp--;
    if (itstackp > ((progindex > -1) ? minitstackp[progindex] : -1)) {
        if (itdcmd[itstackp]) {return true;}
    }
    if (dlstackp > ((progindex > -1) ? mindlstackp[progindex] : -1)) {
        if (dldcmd[dlstackp]) {return true;}
    }
    fnstackp++;
    while (cmd[j]) {if (cmd[j] != ' ') {cerr = 3; return true;} ++j;}
    if (fninfor[fnstackp]) {
        if (inProg) {
            cp = fnstack[fnstackp].cp;
        } else {
            concp = fnstack[fnstackp].cp;
        }
        progLine = fnstack[fnstackp].pl;
        dlstackp = fnstack[fnstackp].dlsp;
        itstackp = fnstack[fnstackp].itsp;
        lockpl = true;
        didloop = true;
    } else {
        fnstack[fnstackp].cp = -1;
        brkinfo.type = 0;
    }
    fnstackp--;
    brkinfo = fnstack[fnstackp].brkinfo;
    return true;
}
