# config
d$="L"
delayms=100
# ------
_txtlock
if _argc()=0
    ostr$=input$("Text: ")
elseif _argc()=1
    ostr$=_arg$()
else
    ?"Expected 1 argument, recieved ";_argc();" arguments instead"
endif
d$=ucase$(d$)
@refresh
w=width()
nw=w-len(ostr$)
str$="        "+ostr$+pad$("",nw)
if len(str$)<w:str$=str$+" ":endif
do
    resettimer
    print snip$(str$,0,w)
    if d$="R"
        str$=snip$(str$,len(str$)-1,len(str$))+snip$(str$,0,len(str$)-1)
    else
        str$=snip$(str$,1,len(str$))+snip$(str$,0,1)
    endif
    waitms limit(delayms-timerms(),0,delayms)
    locate ,-1
    if width()<>w:goto refresh:endif
loop
                                                                           
                                                                            
