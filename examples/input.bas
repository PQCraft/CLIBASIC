PRINT "Press Ctrl+C to exit..."
DO
    K$ = INKEY$()
    L = LEN(K$)
    IF ASC(K$) <> 0
        LOCATE 16
        IF ASC(K$, LEN(K$) - 1) = 10: LOCATE , CURY() - 1: ENDIF
        PUT LEN(K$), ": ["
        FOR P, 0, P < L, 1
            IF P > 0: PUT ", ": ENDIF
            PUT ASC(K$, P)
        NEXT
        PUT "]"
        STR$ = ""
        FOR I, 0, ASC(K$, I) <> 0, 1
            C$ = SNIP$(K$, I, I + 1)
            IF C$ = "\n"
                STR$ = STR$ + "\\n"
            ELSEIF C$ = "\r"
                STR$ = STR$ + "\\r"
            ELSEIF C$ = "\f"
                STR$ = STR$ + "\\f"
            ELSEIF C$ = "\t"
                STR$ = STR$ + "\\t"
            ELSEIF C$ = "\v"
                STR$ = STR$ + "\\v"
            ELSEIF C$ = "\b"
                STR$ = STR$ + "\\b"
            ELSEIF C$ = "\e"
                STR$ = STR$ + "\\e"
            ELSEIF C$ = "\a"
                STR$ = STR$ + "\\a"
            ELSEIF ASC(C$) < 32 | ASC(C$) > 126
                STR$ = STR$ + "\\x" + PAD$(HEX$(ASC(C$)), 2, "0")
            ELSE
                STR$ = STR$ + C$
            ENDIF
        NEXT
        PUT ": {", STR$, "}"
        PUT "\n"
    ENDIF
LOOP
