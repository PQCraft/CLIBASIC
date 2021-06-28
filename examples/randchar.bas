_txtattrib "bgc", "on"
do
    color cint(rand(255)), cint(rand(255))
    put chr$(cint(rand(31, 126)))
loop
