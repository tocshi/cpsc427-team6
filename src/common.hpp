#pragma once

// stlib
#include <fstream> // stdout, stderr..
#include <string>
#include <tuple>
#include <vector>
#include <cmath>
#include <../ext/json/single_include/nlohmann/json.hpp>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
using namespace glm;
using json = nlohmann::json;

#include "tiny_ecs.hpp"

// Simple utility functions to avoid mistyping directory name
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "../ext/project_path.hpp"
inline std::string data_path() { return std::string(PROJECT_SOURCE_DIR) + "data"; };
inline std::string shader_path(const std::string& name) { return std::string(PROJECT_SOURCE_DIR) + "/shaders/" + name; };
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + std::string(name); };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + std::string(name); };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + std::string(name); };
inline std::string font_path(const std::string& name) { return data_path() + "/fonts/" + std::string(name); };
inline std::string tilemaps_path(const std::string& name) { return data_path() + "/maps/" + std::string(name); };

const std::string SAVE_CONFIG_PATH = data_path() + "/config.json";

extern int window_width_px;
extern int window_height_px;

extern float ui_scale;

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void loadConfig(); // loading config 

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
// We recomment making all components non-copyable by derving from ComponentNonCopyable
struct Transform {
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

bool gl_has_errors();
int irand(int i);
int irandRange(int i, int j);
int ichoose(int i, int j);
vec2 dirdist_extrapolate(vec2 pos, float angle, float dist);
float degtorad(float angle);
float radtodeg(float angle);
