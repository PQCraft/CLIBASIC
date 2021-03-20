do
    set e, cint(rand(31, 126))
    set c, cint(rand(1))
    color c*8+2
    if cint(rand(1)) = 0: $txtbold: else: $txtreset: endif
    if cint(rand(3)) = 0:
        color ,0: print "\t"
        if cint(rand(15)) = 0: color ,15: endif
        waitms cint(rand(5, 10))
    else
        print chr$(e)
    endif
loop
