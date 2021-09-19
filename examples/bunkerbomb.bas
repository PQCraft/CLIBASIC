@ again
set i$, input$("Bomb le bunker? 💣 [Y/n]: ")
i$=ucase$(snip$(i$, 1))
if i$ <> "" & i$ <> "Y" & i$ <> "N"
    goto again
endif
print "You have chosen..."
if i$ <> "N"
    print "☢️ Sending nukes...";
    wait 3
    print "\nDone. 💥"
else
    print "Le buker lives another day."
endif
