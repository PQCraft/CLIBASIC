for y,0,y<16,1
    for x,0,x<16,1
        color 255-(x+y*16),x+y*16
        _txtattrib "bgc", "on"
        put " ",x+y*16," "
        _txtattrib "bgc", "off"
        put "\t"
    next
    color 15,0
    print
next
