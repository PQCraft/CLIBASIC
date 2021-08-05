PRINT "clibasic: {" + _STARTCMD$() + "}"
PRINT "cmdline: {" + _ARG$() + "}"
FOR I, 0, I < _ARGC(), 1
    print "arg [" + STR$(I) + "]" + ": {" + _ARG$(I) + "}"
NEXT

