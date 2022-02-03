#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

// Player component
struct Player
{

};

// Eagles have a hard shell
struct Deadly
{

};

// Bug and Chicken have a soft shell
struct Eatable
{

};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	vec2 destination = { -100, -100 };
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying chicken
struct DeathTimer
{
	float counter_ms = 3000;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

// =========================================
// 
struct Damageable {

};

struct Consumable {

};

struct Equipable {

};

struct Guardable {

};

struct Interactable {
	
};

struct Stat {
	float hp = 0;
	float mp = 0;
	float ep = 0;
};

struct Queueable {

};

struct Test {

};

enum class BUTTON_ACTION_ID {
	MENU_QUIT = 0,
	MENU_START = MENU_QUIT + 1,
	ACTION_COUNT = MENU_START + 1
};
const int button_action_count = (int)BUTTON_ACTION_ID::ACTION_COUNT;

struct Button {
	BUTTON_ACTION_ID action_taken = BUTTON_ACTION_ID::ACTION_COUNT;
};

struct MenuItem {

};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	BG = 0,
	BUG = BG + 1,
	EAGLE = BUG + 1,
	PLAYER = EAGLE + 1,
	ENEMY = PLAYER + 1,
	BOSS = ENEMY + 1,
	ARTIFACT = BOSS + 1,
	CONSUMABLE = ARTIFACT + 1,
	EQUIPABLE = CONSUMABLE + 1,
	CHEST = EQUIPABLE + 1,
	DOOR = CHEST + 1,
	SIGN = DOOR + 1,
	STAIR = SIGN + 1,
	START = STAIR + 1,
	QUIT = START + 1,
	TITLE = QUIT + 1,
	TEXTURE_COUNT = TITLE + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	EGG = COLOURED + 1,
	CHICKEN = EGG + 1,
	TEXTURED = CHICKEN + 1,
	WIND = TEXTURED + 1,
	EFFECT_COUNT = WIND + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	CHICKEN = 0,
	SPRITE = CHICKEN + 1,
	EGG = SPRITE + 1,
	DEBUG_LINE = EGG + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

enum class RENDER_LAYER_ID {
	BG = 0,
	SPRITE = BG + 1,
	EFFECT = SPRITE + 1,
	UI = EFFECT + 1,
	DEBUG = UI + 1,
	LAYER_COUNT = DEBUG + 1
};
const int layer_count = (int)RENDER_LAYER_ID::LAYER_COUNT;


struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
	RENDER_LAYER_ID used_layer = RENDER_LAYER_ID::SPRITE;
};

