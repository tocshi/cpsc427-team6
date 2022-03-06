#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

enum class PLAYER_ACTION {
	MOVING = 0,
	ATTACKING = MOVING + 1,
	ACTION_COUNT = ATTACKING + 1
};

// Inventory Items
enum class CONSUMABLE {
	REDPOT = 0,
	BLUPOT = REDPOT + 1,
	YELPOT = BLUPOT + 1,
	CONSUMABLE_COUNT = YELPOT + 1
};

enum class WEAPON {
	STICK = 0,
	BOW0 = STICK + 1,
	SWORD0 = BOW0 + 1,
	BLUNT0 = SWORD0 + 1,
	BOW1 = BLUNT0 + 1,
	SWORD1 = BOW1 + 1,
	BLUNT1 = SWORD1 + 1,
	WEAPON_COUNT = BLUNT1 + 1
};

enum class ARMOUR {
	FAMCLOTH = 0,
	ARMOUR_COUNT = FAMCLOTH + 1
};

enum class ARTIFACT {
	POISON_FANG = 0,
	GLAD_HOPLON = POISON_FANG + 1,
	PIOUS_PRAYER = GLAD_HOPLON + 1,
	BLADE_POLISH = PIOUS_PRAYER + 1,
	HQ_FLETCHING = BLADE_POLISH + 1,
	HOMEMADE_MEAL = HQ_FLETCHING + 1,
	THUNDER_TWIG = HOMEMADE_MEAL + 1,
	LUCKY_CHIP = THUNDER_TWIG + 1,
	GUIDE_HEALBUFF = LUCKY_CHIP + 1,
	THICK_TOME = GUIDE_HEALBUFF + 1,
	GOLIATH_BELT = THICK_TOME + 1,
	BLOOD_RUBY = GOLIATH_BELT + 1,
	WINDBAG = BLOOD_RUBY + 1,
	KB_MALLET = WINDBAG + 1,
	WEAPON_UPGRADE = KB_MALLET + 1,
	ARMOUR_UPGRADE = WEAPON_UPGRADE + 1,
	ARTIFACT_COUNT = ARMOUR_UPGRADE + 1
};

// Inventory component
struct Inventory
{
	int equipped[2] = { -1, -1 };
	int consumable[static_cast<int>(CONSUMABLE::CONSUMABLE_COUNT)];
	int weapon[static_cast<int>(WEAPON::WEAPON_COUNT)];
	int armour[static_cast<int>(ARMOUR::ARMOUR_COUNT)];
	int artifact[static_cast<int>(ARTIFACT::ARTIFACT_COUNT)];
};

// Player component
struct Player
{
	float s;
	Inventory inv;
	// current action taking (count acts as no current action being taken)
	PLAYER_ACTION action = PLAYER_ACTION::ACTION_COUNT;

	// true if the player has already attacked that turn
	bool attacked = false;
};

// Mode visualization objects
struct ModeVisualization {

};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	vec2 destination = { 0, 0 };
	float movement_speed = 0;
	bool in_motion = false;
};

struct Collidable 
{

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

// A timer that will be associated to dying player
struct DeathTimer
{
	float counter_ms = 3000;
};

// Squishing effect timer
struct SquishTimer
{
	float counter_ms = 3000;
	vec2 orig_scale = { 0, 0 };
};

// Wobble effect timer
struct WobbleTimer
{
	float counter_ms = 3000;
	vec2 orig_scale = { 0, 0 };
};

struct ProjectileTimer
{
	float counter_ms = 3000;
	Entity owner;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl)
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

struct Damageable {

};

struct Consumable {
	// recover mp
	bool magic_potion = true;
	// recover hp
	bool hp_potion = true;

};

struct Equipable {
	bool axe = true;
	bool sword = true;
	bool wand = true;
};

struct Guardable {

};

struct Interactable {
	
};

struct Stats {
	// Name goes here too
	std::string name = "Placeholder Name";
	std::string prefix = "";
	// set intial stats to 100 points
	float hp    = 100.f;
	float maxhp = 100.f;
	float mp    = 100.f;
	float maxmp = 100.f;
	float ep    = 100.f;
	float maxep = 100.f;
	float atk   = 10.f;
	float def   = 10.f;
	float speed = 10.f;
	float range = 450.f;
	float chase = 450.f;
	bool guard = false;
};

struct Queueable {
	bool doing_turn = false;
};

struct Test {

};

struct Solid {

};

enum class ENEMY_STATE {
	IDLE = 0,
	AGGRO = IDLE + 1,
	ATTACK = AGGRO + 1,
	DEATH = ATTACK + 1,
	STATE_COUNT = DEATH + 1
};

enum class ENEMY_TYPE {
	SLIME = 0,
	PLANT_SHOOTER = SLIME + 1,
	TYPE_COUNT = PLANT_SHOOTER + 1
};

// simple component for all enemies
struct Enemy {
	vec2 initialPosition = { 0, 0 };
	Inventory inv;
	ENEMY_STATE state = ENEMY_STATE::STATE_COUNT;
	ENEMY_TYPE type = ENEMY_TYPE::TYPE_COUNT;
	bool hit_by_player = false;
};

struct ActionButton {

};

enum class BUTTON_ACTION_ID {
	MENU_QUIT = 0,
	MENU_START = MENU_QUIT + 1,
	ACTIONS_MOVE = MENU_START + 1,
	ACTIONS_ATTACK = ACTIONS_MOVE + 1,
	ACTIONS_GUARD = ACTIONS_ATTACK + 1,
	ACTIONS_END_TURN = ACTIONS_GUARD + 1,
	ACTIONS_ITEM = ACTIONS_END_TURN + 1,
	ACTIONS_BACK = ACTIONS_ITEM + 1,
	ACTIONS_CANCEL = ACTIONS_BACK + 1,
	PAUSE = ACTIONS_CANCEL + 1,
	COLLECTION = PAUSE + 1,
	ACTION_COUNT = COLLECTION + 1
};
const int button_action_count = (int)BUTTON_ACTION_ID::ACTION_COUNT;

struct Button {
	BUTTON_ACTION_ID action_taken = BUTTON_ACTION_ID::ACTION_COUNT;
};

struct MenuItem {

};

struct EpRange {
	float radius = 450.f;
	float resolution = 2000.f;
	vec2 screen_resolution = { 1600.f, 900.f };
};

struct Fog {
	float radius = 450.f;
	float resolution = 2000.f;
	vec2 screen_resolution = { 1600.f, 900.f };
};

struct Camera {
	vec2 position = { 0,0 };
	bool active = false;
};

struct Text {
	vec2 position = { 0.f, 0.f };
	float scale = 1.0f;
	vec3 textColor = { 0.f, 0.f, 0.f };
	std::string message = "";
};

// A timer that will be associated will text logging
struct TextTimer
{
	float counter_ms = 4000;
};

// temp struct for artifacts
struct Artifact {
	bool artifact1 = true; 
};
struct Door {
	bool collidedWithDoor = false; 
};
struct TileUV {
	std::string layer;
	int tileID = 0;
	vec2 uv_start = { 0,0 };
	vec2 uv_end = { 0,0 };
};

struct Sign {
	int counter_ms = 0;
	std::vector<std::pair<std::string, int>> messages; // a list of messages, and the time since click at which each message is logged
	bool playing = false; // counter_ms only updates when this is true
	int next_message = 0;
};

enum class StatusType {
	POISON = 0,
	STUN = POISON + 1,
	ATK_BUFF = STUN + 1,
	DEF_BUFF = ATK_BUFF + 1
};

struct StatusEffect {
	float value;
	int turns_remaining;
	StatusType effect;
	bool percentage; // if true, the struct's value field is used as a percentage in calculations, otherwise it is applied as a flat buff after percentages are calculated
};

struct StatusContainer {
	std::vector<StatusEffect> statuses;
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
	PLAYER = BG + 1,
	SLIME = PLAYER + 1,
	PLANT_SHOOTER = SLIME + 1,
	PLANT_PROJECTILE = PLANT_SHOOTER + 1,
	BOSS = PLANT_PROJECTILE + 1,
	ARTIFACT = BOSS + 1,
	CONSUMABLE = ARTIFACT + 1,
	EQUIPABLE = CONSUMABLE + 1,
	CHEST = EQUIPABLE + 1,
	DOOR = CHEST + 1,
	SIGN = DOOR + 1,
	SIGN_GLOW_SPRITESHEET = SIGN + 1,
	STAIR = SIGN_GLOW_SPRITESHEET + 1,
	START = STAIR + 1,
	QUIT = START + 1,
	TITLE = QUIT + 1,
	WALL = TITLE + 1,
	HPBAR = WALL + 1,
	MPBAR = HPBAR + 1,
	EPBAR = MPBAR + 1,
	HPFILL = EPBAR + 1,
	MPFILL = HPFILL + 1,
	EPFILL = MPFILL + 1,
	ACTIONS_MOVE = EPFILL + 1,
	ACTIONS_ATTACK = ACTIONS_MOVE + 1,
	ACTIONS_BAR = ACTIONS_ATTACK + 1,
	ACTIONS_GUARD = ACTIONS_BAR + 1,
	ACTIONS_ITEM = ACTIONS_GUARD + 1,
	ACTIONS_END_TURN = ACTIONS_ITEM + 1,
	ACTIONS_BACK = ACTIONS_END_TURN + 1,
	ACTIONS_CANCEL = ACTIONS_BACK + 1,
	ACTIONS_ATTACK_MODE = ACTIONS_CANCEL + 1,
	ACTIONS_MOVE_MODE = ACTIONS_ATTACK_MODE + 1,
	PAUSE = ACTIONS_MOVE_MODE + 1,
	COLLECTION_BUTTON = PAUSE + 1,
	DUNGEON_TILESHEET = COLLECTION_BUTTON + 1,
	CAMPFIRE_SPRITESHEET = DUNGEON_TILESHEET + 1,
	EXPLOSION_SPRITESHEET = CAMPFIRE_SPRITESHEET + 1,
	TEXTURE_COUNT = EXPLOSION_SPRITESHEET + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	LINE = COLOURED + 1,
	TEXTURED = LINE + 1,
	WIND = TEXTURED + 1,
	TEXT = WIND + 1,
	FOG = TEXT + 1,
	EP = FOG + 1,
	EFFECT_COUNT = EP + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPRITE = 0,
	TILEMAP = SPRITE + 1,
	LINE = TILEMAP + 1,
	ANIMATION = LINE + 1,
	FOG = ANIMATION + 1,
	EP = FOG + 1,
	DEBUG_LINE = EP + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	TEXTQUAD = SCREEN_TRIANGLE + 1,
	GEOMETRY_COUNT = TEXTQUAD + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

enum class RENDER_LAYER_ID {
	BG = 0,
	FLOOR = BG + 1,
	FLOOR_DECO = FLOOR + 1,
	SPRITE = FLOOR_DECO + 1,
	WALLS = SPRITE + 1,
	RANDOM_WALLS = WALLS + 1,
	EFFECT = RANDOM_WALLS + 1,
	UI = EFFECT + 1,
	UI_TOP = UI + 1,
	DEBUG_LAYER = UI_TOP + 1,
	LAYER_COUNT = DEBUG_LAYER + 1
};
const int layer_count = (int)RENDER_LAYER_ID::LAYER_COUNT;


struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
	RENDER_LAYER_ID used_layer = RENDER_LAYER_ID::SPRITE;
};

struct AnimationData {
	// spritesheet data required for animation (maybe split into another component later)
	TEXTURE_ASSET_ID spritesheet_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	int spritesheet_width; // width of the source image
	int spritesheet_height; // height of the source image
	int spritesheet_columns; // number of columns the spritesheet image is split into
	int spritesheet_rows; // number of columns the spritesheet image is split into
	vec2 frame_size; // width and height of a "tile" in the spritesheet

	// animation data
	int current_frame = 0; // the current frame of the animation. Gets updated and used to access frame_indices
	int animation_time_ms = 0; // elapsed time in animation. Reset to 0 when animation reaches end and animation is looping
	int frametime_ms; // how long it should take before switching frames
	std::vector<int> frame_indices; // indices refer to a "tile" within the sheet. List the indices as frames in an animation

	bool loop = true;
	bool delete_on_finish = false; // if the entitiy should be deleted when the loop is finished
};

// guard button
struct GuardButton {
	// which texture to show
	TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::ACTIONS_GUARD;

	// which button action to take
	BUTTON_ACTION_ID action = BUTTON_ACTION_ID::ACTIONS_GUARD;
};