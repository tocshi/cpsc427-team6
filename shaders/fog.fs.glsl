#version 330

// From Vertex Shader
in vec3 vcolor;

// Application data
uniform vec3 fcolor;
uniform float distance;
// (just the width because this is a box)
uniform float resolution;
// screen resolution
uniform vec2 screen_resolution;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	// pixel point
	vec2 st = gl_FragCoord.xy / vec2(resolution);

	// player point (div by 2 to get center of screen)
	vec2 pos = vec2(0.0);
    pos.x += (screen_resolution.x / 2) / resolution;
	pos.y += (screen_resolution.y / 2) / resolution;

	// is the current point within the radius
	float absSum = pow(st.x - pos.x, 2) + pow(st.y - pos.y, 2);

	if (absSum <= pow(distance, 2)) {
		// set gradient
		float gradient = 1.0;
		gradient = (pow(distance, 2) - absSum) * 50.f;

		// draw the color based on how far the current point is from the radius
		color = vec4(fcolor * vcolor, 0.7 - gradient);
	} else {
		// alpha is higher
		color = vec4(fcolor * vcolor, 0.7);
	}
}