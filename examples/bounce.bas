i = 0.5
v = 0.99
f = 0.1
text = "test"
# ---
l = len(text)
h = height()-cint(((height()-1)*sin(pi()*i))*limit(v,0,1))
do
    for i, i, i => 0, -0.01
        oh = h
        h = height()-cint(((height()-1)*sin(pi()*i))*limit(v,0,1))
        locate 1, h
        print text;
        if h<>oh
            locate 1, oh
            print pad$("", l);
        endif
        locate 1, h
        waitms 20 * v
    next
    v=v-f
    if v<0:v=0:endif
    for i, i, i <= 0.5, 0.01
        oh = h
        h = height()-cint(((height()-1)*sin(pi()*i))*limit(v,0,1))
        locate 1, h
        print text;
        if h<>oh
            locate 1, oh
            print pad$("", l);
        endif
        locate 1, h
        waitms 20 * v
    next
loop

