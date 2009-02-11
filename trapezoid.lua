local verts = {
	{ 50, -25, 100 },
	{ 50, -25, 120 },
	{ 50, -25, 160 },
	{ 50, -25, 180 },
	{ 230, -25, 180 },
	{ 230, -25, 160 },
	{ 230, -25, 120 },
	{ 230, -25, 100 },
	{ 11.75, 25, 83 },
	{ 11.75, 25, 111.5 },
	{ 11.75, 25, 168.5 },
	{ 11.75, 25, 197 },
	{ 268.25, 25, 197 },
	{ 268.25, 25, 168.5 },
	{ 268.25, 25, 111.5 },
	{ 268.25, 25, 83 }
}
local faces = {
	{1,2,3,4,5,6,7,8, {0, 1, 0}},
	{9,10,2, {0.794245, 0.607598, 0}},
	{10,11,3, {0.794245, 0.607598, 0}},
	{11,12,4, {0.794245, 0.607598, 0}},
	{12,13,5, {0, 0.321903, -0.946773}},
	{13,14,6, {-0.794245, 0.607598, 0}},
	{14,15,7, {-0.794245, 0.607598, 0}},
	{15,16,8, {-0.794245, 0.607598, 0}},
	{16,9,1, {-0, 0.321903, 0.946773}},
	{16,15,14,13,12,11,10,9, {0, -1, 0}},
	{9,2,1, {0.794245, 0.607598, -0}},
	{10,3,2, {0.794245, 0.607598, -0}},
	{11,4,3, {0.794245, 0.607598, -0}},
	{12,5,4, {0, 0.321903, -0.946773}},
	{13,6,5, {-0.794245, 0.607598, 0}},
	{14,7,6, {-0.794245, 0.607598, 0}},
	{15,8,7, {-0.794245, 0.607598, 0}},
	{16,1,8, {0, 0.321903, 0.946773}}
}

return { vertices = verts, faces = faces }
