PRINT "clibasic: {" + _STARTCMD$() + "}"
PRINT "file: {" + _ARG$(I) + "}"
PRINT "cmdline: {" + _ARG$() + "}"
PRINT "count: ["; _ARGC(); "]"
FOR I, 1, I <= _ARGC(), 1
    print "arg [" + STR$(I) + "]" + ": {" + _ARG$(I) + "}"
NEXT

