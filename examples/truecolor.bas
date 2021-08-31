r = limit(val(input$("red:   ")), 0, 255)
g = limit(val(input$("green: ")), 0, 255)
b = limit(val(input$("blue:  ")), 0, 255)
c = rgb(r, g, b)
h = pad$(hex$(c), 6, "0")
_txtattrib "truecolor", "on"
print "Color #"; h
_txtattrib "bgc", "on"
color , c
put "  "
_txtattrib "truecolor", "off"
print

