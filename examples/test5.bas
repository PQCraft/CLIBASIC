_txtattrib "bgc", 1
_txtlock
do
    locate cint(rand(1, width())), cint(rand(1, height()))
    color cint(rand(255)), cint(rand(255))
    _txtattrib cint(rand(1, 11)), cint(rand(1))
    put chr$(rand(32, 126))
loop
