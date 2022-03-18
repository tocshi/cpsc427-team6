#include "common.hpp"
#include <stdlib.h>
#include <time.h>

// Note, we could also use the functions from GLM but we write the transformations here to show the uderlying math
void Transform::scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform::rotate(float radians)
{
	float c = cosf(radians);
	float s = sinf(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform::translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	mat = mat * T;
}

bool gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) return false;

	while (error != GL_NO_ERROR)
	{
		const char* error_str = "";
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		fprintf(stderr, "OpenGL: %s", error_str);
		error = glGetError();
		assert(false);
	}

	return true;
}

int irand(int i) {
	if (i == 0) {
		return 0;
	}
	return rand() % i;
}

int irandRange(int i, int j) {
	if (j - i == 0) {
		return i;
	}
	return rand() % (j - i) + i;
}

int ichoose(int i, int j) {
	int rng = irand(2);
	if (rng) {
		return i;
	}
	else {
		return j;
	}
}

// extrapolate position
vec2 dirdist_extrapolate(vec2 pos, float angle, float dist) {
	return { pos.x + (dist * cos(angle)), pos.y + (dist * sin(angle)) };
}

// to radians
float degtorad(float angle) {
	return angle * (M_PI / 180);
}

// to deg
float radtodeg(float angle) {
	return angle * (180 / M_PI);
}