-- Public domain

local vertices = {
	{-5, -5, 10};
	{ 5, -5, 10};
	{ 5,  5, 10};
	{-5,  5, 10};
	{-5, -5, 20};
	{ 5, -5, 20};
	{ 5,  5, 20};
	{-5,  5, 20};
}

local faces = {
	{4, 3, 2, 1, normal = {0, 0, -1}};
	{2, 6, 5, 1, normal = {0, -1, 0}};
	{5, 8, 4, 1, normal = {-1, 0, 0}};
	{2, 3, 7, 6, normal = {1, 0, 0}};
	{3, 4, 8, 7, normal = {0, 1, 0}};
	{5, 6, 7, 8, normal = {0, 0, 1}};
};

for _, f in ipairs(faces) do
	for i, v in ipairs(f) do
		if type(v) == 'number' then
			f[i] = {
				index = v;
				texcoords = {0, 0};
			}
		end
	end
end

return { vertices = vertices, faces = faces }
