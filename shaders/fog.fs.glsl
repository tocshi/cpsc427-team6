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
	vec2 st = gl_FragCoord.xy / vec2(2000, 2000);
    st.x *= (2000 / 2000);

	vec2 pos = fog_pos.xy;

	// screen resolution / 2
    pos.x += 800.f;
	pos.y += 450.f;

	// calcualte a circle around the given position (the player's position)
	circle = distance(st, pos / vec2(2000, 2000));

	// render the color in the locations outside of the circle's area
	circle = smoothstep(0.1, 0.204, circle);

	vec3 circle_color = fcolor * vcolor;
	circle_color = circle_color * circle;

	color = vec4(circle_color, 1.0 * circle);
}