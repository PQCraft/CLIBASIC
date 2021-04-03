'_txtlock
print "Press Ctrl+C to exit..."
do
    k$ = inkey$()
    l = len(k$)
    if asc(k$) <> 0
        p = 0
        locate curx() - 1
        put "\t[", len(k$), "]"
        dowhile p < l
            put ", ", asc(k$, p)
            p = p + 1
        loop
        put "\n"
    endif
loop
'run "examples/input.bas"
