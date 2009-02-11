input.bind('quit', input.funcs.exit)
input.bind('escape', input.funcs.exit)

input.bind('a', input.funcs.forward)
input.bind('z', input.funcs.reverse)
input.bind('s', input.funcs.slide_left)
input.bind('d', input.funcs.slide_right)
input.bind('f', input.funcs.slide_up)
input.bind('v', input.funcs.slide_down)

input.bind('q', input.funcs.roll_left)
input.bind('e', input.funcs.roll_right)

input.bind('left', input.funcs.turn_left)
input.bind('right', input.funcs.turn_right)
input.bind('up', input.funcs.pitch_up)
input.bind('down', input.funcs.pitch_down)

input.bind('joy1axis1', input.funcs.turn)
input.bind('joy1axis2', input.funcs.pitch)
input.bind('joy1axis3', input.funcs.roll)

input.bind('mousemove', input.funcs.rotatexy)
input.bind('mouse1', function(event)
	if event.pressed then
		input.setmousecontrol(true)
	end
end)
input.bind('mouse3', function(event)
	if event.pressed then
		input.setmousecontrol(false)
	end
end)
--input.bind('mouse', function(event) print(event.x..", "..event.y) end)
