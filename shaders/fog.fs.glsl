#version 330

// From Vertex Shader
in vec3 vcolor;
in vec3 fog_pos;

// Application data
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	float circle = 0.0;
	vec2 st = gl_FragCoord.xy;
	// color = vec4(fcolor * vcolor, 1.0);

	circle = distance(vec2(0.0), fog_pos.xy);

	circle = smoothstep(0.9, 0.304, 1.0 - circle);

	vec3 circle_color = fcolor * vcolor;
	circle_color = circle_color * circle;

	color = vec4(circle_color, 1.0 * circle);
}