#version 330

// Input attributes
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texcoord;
layout (location = 2) in mat3 instanceMatrix;
layout (location = 5) in vec2 uv_start;
layout (location = 6) in vec2 uv_end;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	texcoord = vec2(uv_start.x + in_texcoord.x * (uv_end.x-uv_start.x), uv_start.y + in_texcoord.y * (uv_end.y-uv_start.y));
	// texcoord = in_texcoord;
	vec3 pos = projection * instanceMatrix * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}