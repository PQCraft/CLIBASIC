score = 0                    'set the 'score' variable to 0
resettimer                   'reset the timer to 0
dowhile timer() < 5          'do the following commands while the timer is less than 5 seconds
    score = score + 1        'add 1 to the score
loop                         'loop back to the dowhile
put "Score: ", score, "\n"   'output the score
