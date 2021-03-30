_txtlock
do
  k$ = inkey$()
  if asc(k$) <> 0
    print k$
  endif
loop
