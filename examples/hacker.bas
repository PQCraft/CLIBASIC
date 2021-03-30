do
    set e, cint(rand(31, 126))
    set c, cint(rand(1))
    color c*8+2
    if cint(rand(1)) = 0: _txtattrib 1, 1: else: _txtattrib 0: endif
    put chr$(e)
    if cint(rand(15)) = 0: put "\r\e[2K": endif
    if cint(rand(255)) = 0: put "\n": endif
loop
