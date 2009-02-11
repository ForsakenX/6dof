-- Public domain

local T30 = math.tan(math.rad(30))
local T60 = math.tan(math.rad(60))

local function normal(v1,v2,v3)
	local n = (v2-v1):cross(v3-v2):norm()
	print(("Normal: %g,%g,%g"):format(n[1],n[2],n[3]))
	return n
end

local verts = {
	 vec(0,5*T60,5)
	,vec(-5,0,5)
	,vec(5,0,5)
	,vec(0,5*T30,5+math.sqrt(100-(5*T60-5*T30)^2))
}
local faces = {
	 {1,3,2,normal(verts[1], verts[3], verts[2])}
	,{1,2,4,normal(verts[1], verts[2], verts[4])}
	,{1,4,3,normal(verts[1], verts[4], verts[3])}
	,{2,3,4,normal(verts[2], verts[3], verts[4])}
}

return { vertices = verts, faces = faces }
