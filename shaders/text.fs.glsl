#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec4 fcolor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	// color = texture(sampler0, vec2(texcoord.x, texcoord.y));
	// color.a = 1;
	color = vec4(fcolor.xyz, ceil(texture(sampler0, vec2(texcoord.x, texcoord.y)).r)) * vec4(1.0, 1.0, 1.0, fcolor.w); 
}