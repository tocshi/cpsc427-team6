#version 330

// Input attributes
in vec3 in_color;
in vec3 in_position;
in int radius;

out vec3 vcolor;
out vec3 fog_pos;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
	vcolor = in_color;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	fog_pos = pos;
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
