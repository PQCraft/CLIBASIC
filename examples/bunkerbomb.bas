set e, 0
dowhile e = 0
    set i$, input$("Bomb le bunker? [Y/n]: ")
    if i$ = "": set e, 1: endif
    if chr$(asc(i$)) = "y": set e, 1: endif
    if chr$(asc(i$)) = "Y": set e, 1: endif
    if chr$(asc(i$)) = "n": set e, 2: endif
    if chr$(asc(i$)) = "N": set e, 2: endif
loop
print "You have chosen...\n"
if e = 1
    print "Sending nukes..."
    wait 3
    print "\nDone."
else
    print "Le buker lives another day."
endif
exit