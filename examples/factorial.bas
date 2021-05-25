str=INPUT$("Enter a factorial number: ")
num=CINT(VAL(str))
fact=1

IF num < 1
    PRINT "Factorial can't be 0 or negative!"
    EXIT 1
ENDIF

FOR i, 1, i <= num, 1
    fact = fact * i
NEXT

PRINT "!" + STR$(num) + " is " + STR$(fact)
