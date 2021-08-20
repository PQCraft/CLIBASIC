r = val(input$("red:   "))
g = val(input$("green: "))
b = val(input$("blue:  "))
c = rgb(r, g, b)
h = pad$(hex$(c), 6, "0")
put "Color "
_txtattrib "truecolor", "on"
color c
print "#"; h
_txtattrib "bgc", "on"
color , c
put "  "
_txtattrib "truecolor", "off"
print

