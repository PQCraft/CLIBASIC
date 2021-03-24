score = 0                    'set the 'score' variable to 0
resettimer                   'reset the timer to 0
dowhile timer() < 5          'do the following commands while the timer is less than 5000000 us (5 sec)
    score = score + 1        'add 1 to the score
loop                         'loop back to the dowhile
print "Score ", score, "\n"  'print out the score