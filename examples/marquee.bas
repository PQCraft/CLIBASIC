# -- config --
direction$="L"
inplace=1
padding=8
delayms=95
colors=0
bg=0
# ------------
if _argc()=0
    ostr$=input$("Text: ")
    locate ,-1
else
    ostr$=_arg$()
endif
_txtlock
if colors<>0
    _txtattrib "truecolor"
    oc=fgc()
    r=cint(rand(255)):g=cint(rand(255)):b=cint(rand(255))
    x=2:y=rand(0.5, 1):x=x-y:z=rand(0.5,1):x=x-z
    x=x*(delayms/10):y=y*(delayms/10):z=z*(delayms/10)
    if cint(rand(1))=1:x=x*-1:endif
    if cint(rand(1))=1:y=y*-1:endif
    if cint(rand(1))=1:z=z*-1:endif
endif
d$=ucase$(direction$)
@refresh
w=width()
nw=w-len(ostr$)
str$=pad$("",padding)+ostr$+pad$("",nw)
if _os$()="Windows"
    cy=cury()
endif
do
    resettimer
    if colors<>0
        color rgb(limit(r,0,255),limit(g,0,255),limit(b,0,255))
        if bg<>0:_txtattrib "bgc":color ,rgb(255-limit(r,0,255),255-limit(g,0,255),255-limit(b,0,255)):endif
        print "\r";snip$(str$,0,w);
        if bg<>0:_txtattrib "bgc",0:endif
        color oc
        print
        r=r+x
        g=g+y
        b=b+z
        if r>255:x=x*-1:endif
        if r<0:x=x*-1:endif
        if g>255:y=y*-1:endif
        if g<0:y=y*-1:endif
        if b>255:z=z*-1:endif
        if b<0:z=z*-1:endif
    else
        print "\r";snip$(str$,0,w)
    endif
    if d$="R"
        str$=snip$(str$,len(str$)-1,len(str$))+snip$(str$,0,len(str$)-1)
    else
        str$=snip$(str$,1,len(str$))+snip$(str$,0,1)
    endif
    waitms limit(delayms-timerms(),0,delayms)
    if _os$()="Windows"
        if inplace<>0:locate ,cy:else:locate ,cy+1:cy=cury():endif
    else
        if inplace<>0:locate ,-1:endif
    endif
    if width()<>w:goto refresh:endif
loop
                                                                           
                                                                            
