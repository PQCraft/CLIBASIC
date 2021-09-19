'clibasic test program

print "test"
if _vt() = 1
    color 21
else
    color 12
endif
print "blue"
_txtattrib 1, 1

set E, 0
do
    set E, E+1
    print E, "";
loopwhile e < 256

print
