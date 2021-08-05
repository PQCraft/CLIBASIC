#!/usr/bin/clibasic -x
PRINT "location: " + _ARG$(0)
PRINT "args: " + _ARG$()
FOR I, 1, I < _ARGC(), 1
    print "arg #" + STR$(I) + ": " + _ARG$(I)
NEXT
 
