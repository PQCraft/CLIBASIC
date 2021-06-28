do
    set e, cint(rand(31, 126))
    set c, cint(rand(1))
    color c*8+2
    _txtattrib 1, cint(rand(1)):
    if cint(rand(3)) = 0:
        _txtattrib "bgc", "off"
        color ,0: put "\t"
        if cint(rand(15)) = 0: color ,15: _txtattrib "bgc", "on": endif
        waitms cint(rand(5, 10))
    else
        put chr$(e)
    endif
loop
