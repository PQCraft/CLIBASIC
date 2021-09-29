_txtlock
do
    locate 1
    h = date("hr")
    if h > 12
        h = h - 12
        m$ = "PM"
    elseif h = 0
        h = 12
        m$ = "AM"
    else
        m$ = "AM"
    endif
    print pad$(h, 2); ":"; pad$(date("min"), 2); ":"; pad$(date("sec"), 2); m$; " "; pad$(date("year"), 4); "-"; pad$(date("mon"), 2); "-"; pad$(date("day"), 2)
    s = date(0)
    waitms 900
    while date(0) = s: loop
    locate , -1
loop
