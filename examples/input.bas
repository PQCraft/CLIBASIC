$txtlock
? "Press Ctrl+C to exit...\n"
do
    k$ = inkey$()
    l = len(k$)
    if asc(k$) <> 0
        p = 0
        ? "[", len(k$), "]"
        dowhile p < l
            ? ", ", asc(k$, p)
            p = p + 1
        loop
        ? "\n"
    endif
loop
'run "examples/input.bas"