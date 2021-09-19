_txtattrib "fgc", "off"
if _argc() <> 1: print "Expected 1 argument, recieved "; _argc(); " arguments instead": exit 1: endif
del file
file = fopen(_arg$(1), "r")
if file = -1: print "Could not open '"; _arg$(1); "': "; _errnostr$(_fileerror()): endif
dowhile eof(file) = 0: put fread$(file): loop
file = fclose(file)
