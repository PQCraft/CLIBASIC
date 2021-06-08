for y,0,y<16,1
    for x,0,x<16,1
        color x+y*16,255-(x+y*16)
        put " ",x+y*16," \t"
    next
    color 15,0
    print
next
