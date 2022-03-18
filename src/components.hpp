#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include <map>
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

enum class EQUIPMENT {
	ARMOUR = 0,
	SHARP = ARMOUR + 1,
	BLUNT = SHARP + 1,
	RANGED = BLUNT + 1,
	EQUIPMENT_COUNT = RANGED + 1
};

enum class ATTACK {
	NONE = 0,
	ROUNDSLASH = NONE + 1,
	SAPPING_STRIKE = ROUNDSLASH + 1,
	PIERCING_THRUST = SAPPING_STRIKE + 1,
	PARRYING_STANCE = PIERCING_THRUST + 1,
	DISENGAGE = PARRYING_STANCE + 1,
	TERMINUS_VERITAS = DISENGAGE + 1,
	WILD_SWINGS = TERMINUS_VERITAS + 1,
	ARMOURCRUSHER = WILD_SWINGS + 1,
	DISORIENTING_BASH = ARMOURCRUSHER + 1,
	TECTONIC_SLAM = DISORIENTING_BASH + 1,
	FERVENT_CHARGE = TECTONIC_SLAM + 1,
	PRIMAL_RAGE = FERVENT_CHARGE + 1,
	SPREAD_SHOT = PRIMAL_RAGE + 1,
	BINDING_ARROW = SPREAD_SHOT + 1,
	LUMINOUS_ARROW = BINDING_ARROW + 1,
	HOOK_SHOT = LUMINOUS_ARROW + 1,
	FOCUSED_SHOT = HOOK_SHOT + 1,
	SKYBORNE_RAIN = FOCUSED_SHOT + 1
};

//TODO: Fill this out
// Attack name map
const std::map <ATTACK, std::string>attack_names = {
	{ATTACK::ROUNDSLASH, "Roundslash"}
};

enum class ARTIFACT {
	POISON_FANG = 0,
	GLAD_HOPLON = POISON_FANG + 1,
	BLADE_POLISH = GLAD_HOPLON + 1,
	HQ_FLETCHING = BLADE_POLISH + 1,
	MESSENGER_CAP = HQ_FLETCHING + 1,
	WARM_CLOAK = MESSENGER_CAP + 1,
	THUNDER_TWIG = WARM_CLOAK + 1,
	LUCKY_CHIP = THUNDER_TWIG + 1,
	GUIDE_HEALBUFF = LUCKY_CHIP + 1,
	THICK_TOME = GUIDE_HEALBUFF + 1,
	GOLIATH_BELT = THICK_TOME + 1,
	BLOOD_RUBY = GOLIATH_BELT + 1,
	WINDBAG = BLOOD_RUBY + 1,
	KB_MALLET = WINDBAG + 1,
	ARCANE_SPECS = KB_MALLET + 1,
	SCOUT_STRIDE = ARCANE_SPECS + 1,
	ART_CONSERVE = SCOUT_STRIDE + 1,
	ARCANE_FUNNEL = ART_CONSERVE + 1,
	FUNGIFIER = ARCANE_FUNNEL + 1,
	BURRBAG = FUNGIFIER + 1,
	SMOKE_POWDER = BURRBAG + 1,
	LIVELY_BULB = SMOKE_POWDER + 1,
	MALEDICTION = LIVELY_BULB + 1,
	CHIMERARM = MALEDICTION + 1,
	ARTIFACT_COUNT = CHIMERARM + 1
};

// Artifact Rarity Arrays
// Commented artifacts have not yet been completed!
const int artifact_T1[] {
	(int)ARTIFACT::BLADE_POLISH,
	(int)ARTIFACT::HQ_FLETCHING,
	//(int)ARTIFACT::MESSENGER_CAP, // blocked by stat calc system
	//(int)ARTIFACT::WARM_CLOAK, // blocked by stat calc system
	(int)ARTIFACT::GOLIATH_BELT,
	(int)ARTIFACT::BLOOD_RUBY
};
const int artifact_T2[] {
	(int)ARTIFACT::POISON_FANG,
	//(int)ARTIFACT::GUIDE_HEALBUFF, // blocked by healing interactions and stat calc system
	(int)ARTIFACT::WINDBAG,
	//(int)ARTIFACT::SCOUT_STRIDE, // blocked by stat calc system
	//(int)ARTIFACT::ART_CONSERVE, // blocked by stat calc system
	//(int)ARTIFACT::SMOKE_POWDER // blocked by item pickup
};
const int artifact_T3[] {
	(int)ARTIFACT::GLAD_HOPLON,
	//(int)ARTIFACT::THUNDER_TWIG, // not blocked but effect takes some time to implement
	(int)ARTIFACT::KB_MALLET,
	//(int)ARTIFACT::ARCANE_SPECS, // blocked by stat calc system
	//(int)ARTIFACT::ARCANE_FUNNEL, // blocked by mp usage system
	//(int)ARTIFACT::BURRBAG // blocked by stat calc system
};
const int artifact_T4[] {
	(int)ARTIFACT::LUCKY_CHIP,
	(int)ARTIFACT::THICK_TOME,
	//(int)ARTIFACT::FUNGIFIER, // not blocked but effect takes some time to implement
	//(int)ARTIFACT::LIVELY_BULB, // not blocked but effect takes some time to implement
	//(int)ARTIFACT::MALEDICTION, // blocked by stat calc system
	//(int)ARTIFACT::CHIMERARM // blocked by stat calc system and weapon generation
};


// Equipment component
struct Equipment
{
	EQUIPMENT type = EQUIPMENT::EQUIPMENT_COUNT;
	ATTACK attacks[4] = { ATTACK::NONE, ATTACK::NONE, ATTACK::NONE, ATTACK::NONE };
	float atk;
	float def;
	float speed;
	float hp;
	float mp;
	float ep;
	float range;
};

struct ArtifactIcon {
	ARTIFACT artifact = ARTIFACT::ARTIFACT_COUNT;
};

// Inventory component
struct Inventory
{
	Equipment equipped[2]; // [Weapon, Armour]
	int consumable[static_cast<int>(CONSUMABLE::CONSUMABLE_COUNT)];
	int artifact[static_cast<int>(ARTIFACT::ARTIFACT_COUNT)];
};

// Player component
struct Player
{
	float s;
	Inventory inv;
	int gacha_pity;
	int floor;
	int room;
	int total_rooms;
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

struct Guardable {

};

enum class INTERACT_TYPE {
	CHEST = 0,
	DOOR = CHEST + 1,
	STAIRS = DOOR + 1,
	SIGN = STAIRS + 1,
	TYPE_COUNT = SIGN + 1
};

struct Interactable {
	INTERACT_TYPE type;
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
	float mpregen = 10.f;
	float ep    = 100.f;
	float maxep = 100.f;
	float epratemove = 1.f;
	float eprateatk = 1.f;
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
	RETREAT = DEATH + 1,
	STATE_COUNT = RETREAT + 1
};

enum class ENEMY_TYPE {
	SLIME = 0,
	PLANT_SHOOTER = SLIME + 1,
	CAVELING = PLANT_SHOOTER + 1,
	KING_SLIME = CAVELING + 1,
	TYPE_COUNT = KING_SLIME + 1
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
	OPEN_DIALOG = COLLECTION + 1,
	CLOSE_DIALOG = OPEN_DIALOG + 1,
	SCROLL_DOWN = CLOSE_DIALOG + 1,
	SCROLL_UP = SCROLL_DOWN + 1,
	ACTION_COUNT = SCROLL_UP + 1
};
const int button_action_count = (int)BUTTON_ACTION_ID::ACTION_COUNT;

struct Button {
	BUTTON_ACTION_ID action_taken = BUTTON_ACTION_ID::ACTION_COUNT;
};

struct MenuItem {

};

struct DescriptionDialog {
	std::string title = "";
	std::string effect = "";
	std::string description = "";
	std::string stats = "";
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
	BIND = STUN + 1,
	ATK_BUFF = BIND + 1,
	DEF_BUFF = ATK_BUFF + 1,
	SPEED_BUFF = DEF_BUFF + 1,
	FANG_POISON = SPEED_BUFF + 1,
	INVINCIBLE = FANG_POISON + 1,
	BURR_DEBUFF = INVINCIBLE + 1,
	RANGE_BUFF = BURR_DEBUFF + 1,
	WINDBAG_CD = RANGE_BUFF + 1,
	MALEDICTION_CD = WINDBAG_CD + 1,
	PIERCE_DEF = MALEDICTION_CD + 1,
	PARRYING_STANCE = PIERCE_DEF + 1,
	EP_REGEN = PARRYING_STANCE + 1,
	ARCANE_FUNNEL = EP_REGEN + 1,
	PRIMAL_RAGE = ARCANE_FUNNEL + 1,
	FOCUSING = PRIMAL_RAGE + 1
};

struct StatusEffect {
	StatusEffect(float value, int turns, StatusType effect, bool percentage, bool apply_at_turn_start):
		value(value), turns_remaining(turns), effect(effect), percentage(percentage), apply_at_turn_start(apply_at_turn_start) {}
	float value;
	int turns_remaining;
	StatusType effect;
	bool percentage; // if true, the struct's value field is used as a percentage in calculations, otherwise it is applied as a flat buff after percentages are calculated
	bool apply_at_turn_start; // if true, the status is applied at the start of a turn (i.e. stuns, buffs for stat recalculation). Otherwise it is applied at turn end (i.e. Damage over time)
};

struct StatusContainer {
	std::vector<StatusEffect> statuses;
	// this groups the same effect types together, then flat buffs of an effect type will appear before percentage buffs
	// this is in reverse, because statuses is to be iterated backwards so that removing elements doesn't mess things up
	void sort_statuses_reverse() {
		std::sort(statuses.begin(), statuses.end(),
			[](StatusEffect a, StatusEffect b) {
				if (a.effect < b.effect) {
					return true;
				}
				else if (a.effect == b.effect) {
					if (!a.percentage && b.percentage) {
						return true;
					}
					return false;
				}
				else {
					return false;
				}
			}
		);
	}
};

struct KnockBack {
	float remaining_distance = 0; 
	float angle = 0;
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
	CAVELING = PLANT_PROJECTILE + 1,
	BOSS = CAVELING + 1,
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
	NORMAL_POINTER = EXPLOSION_SPRITESHEET + 1,
	ATTACK_POINTER = NORMAL_POINTER + 1,
	MOVE_POINTER = ATTACK_POINTER + 1,
	MENU_CLOSE =  MOVE_POINTER + 1,
	COLLECTION_PANEL = MENU_CLOSE + 1,
	DESCRIPTION_DIALOG = COLLECTION_PANEL + 1,
	ARTIFACT_PLACEHOLDER = DESCRIPTION_DIALOG + 1,
	COLLECTION_SCROLL_ARROW = ARTIFACT_PLACEHOLDER + 1,
	TEXTURE_COUNT = COLLECTION_SCROLL_ARROW + 1
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
	ARTIFACT_ICONS = UI + 1,
	TEXT = ARTIFACT_ICONS + 1,
	DIALOG = TEXT + 1,
	DIALOG_TEXT = DIALOG + 1,
	UI_TOP = DIALOG_TEXT + 1,
	CURSOR = UI_TOP + 1,
	DEBUG_LAYER = CURSOR + 1,
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

// stylized poiner
struct Pointer {

};

// Artifact name map
const std::map <ARTIFACT, std::string>artifact_names = {
	{ARTIFACT::POISON_FANG, "Discarded Fang"},
	{ARTIFACT::GLAD_HOPLON, "Gladiator Hoplon"},
	{ARTIFACT::BLADE_POLISH, "Blade Polish Kit"},
	{ARTIFACT::HQ_FLETCHING, "High-Quality Fletching"},
	{ARTIFACT::MESSENGER_CAP, "Messenger's Cap"},
	{ARTIFACT::WARM_CLOAK, "Warm Cloak"},
	{ARTIFACT::THUNDER_TWIG, "Thundering Twig"},
	{ARTIFACT::LUCKY_CHIP, "Lucky Chip"},
	{ARTIFACT::GUIDE_HEALBUFF, "Guide to Healthy Eating"},
	{ARTIFACT::THICK_TOME, "Unnecessarily Thick Tome"},
	{ARTIFACT::GOLIATH_BELT, "Goliath's Belt"},
	{ARTIFACT::BLOOD_RUBY, "Blood Ruby"},
	{ARTIFACT::WINDBAG, "Bag of Wind"},
	{ARTIFACT::KB_MALLET, "Rubber Mallet"},
	{ARTIFACT::ARCANE_SPECS, "Arcane Spectacles"},
	{ARTIFACT::SCOUT_STRIDE, "Scouting Striders"},
	{ARTIFACT::ART_CONSERVE, "The Art of Conservation"},
	{ARTIFACT::ARCANE_FUNNEL, "Arcane Funnel"},
	{ARTIFACT::FUNGIFIER, "Fungifier"},
	{ARTIFACT::BURRBAG, "Burrbag"},
	{ARTIFACT::SMOKE_POWDER, "Smoke Powder"},
	{ARTIFACT::LIVELY_BULB, "Lively Bulb"},
	{ARTIFACT::MALEDICTION, "Malediction"},
	{ARTIFACT::CHIMERARM, "Chimera's Arm"}
};

// Artifact description map
const std::map <ARTIFACT, std::string>artifact_descriptions = {
	{ARTIFACT::POISON_FANG, "Even without the creature this fang originally belonged to, you still feel uneasy knowing there might be more of them out there."},
	{ARTIFACT::GLAD_HOPLON, "You won't believe it! This man bested TWO lions! It's a miracle how he's still alive! Just what is his shield made of?!"},
	{ARTIFACT::BLADE_POLISH, "test description"},
	{ARTIFACT::HQ_FLETCHING, "test description"},
	{ARTIFACT::MESSENGER_CAP, "Wait, are you sure this came from a messenger?"},
	{ARTIFACT::WARM_CLOAK, "Hiking up the mountain, boot-deep in snow, with chilling winds rushing past�why does this imaginary scene feel so real?"},
	{ARTIFACT::THUNDER_TWIG, "test description"},
	{ARTIFACT::LUCKY_CHIP, "�Why do you have this? Doesn�t that belong to the [REDACTED]? Well, I guess it�s too late to give it back to them now��"},
	{ARTIFACT::GUIDE_HEALBUFF, "�YOU EAT GOOD, YOU BECOME STRONG LIKE ME.�"},
	{ARTIFACT::THICK_TOME, "It is said that someone escaped death when a projectile aimed at them was stopped by a tome like this. Perhaps you may encounter a similar fortune by keeping it on your person."},
	{ARTIFACT::GOLIATH_BELT, "test description"},
	{ARTIFACT::BLOOD_RUBY, "test description"},
	{ARTIFACT::WINDBAG, "How? How does a simple pouch hold such a strong gust of wind? How is this possible?"},
	{ARTIFACT::KB_MALLET, "�Don�t worry, those aren�t real moles.�"},
	{ARTIFACT::ARCANE_SPECS, "It�s engraved with the words �Property of Professor Hammond�. Putting them on somehow lets you see further into the darkness than usual."},
	{ARTIFACT::SCOUT_STRIDE, "�Come get it today! Our newly patented boots that, when you sprint with them, lets you cover long distances far easier than ever before!�"},
	{ARTIFACT::ART_CONSERVE, "The ideas in this book were originally meant for saving energy so you won�t need to eat as frequently, but somebody left notes about applying some of these concepts in close-quarters combat. How intriguing�"},
	{ARTIFACT::ARCANE_FUNNEL, "It�s engraved with the words �Property of Professor Hammond�. It seems to be absorbing energy from fallen monsters. Don�t think about it too much, lest you wish to pity the poor creatures you�ve slain during your time here."},
	{ARTIFACT::FUNGIFIER, "�What do you mean it wasn�t fungible?!�"},
	{ARTIFACT::BURRBAG, "�Who even collects these?�"},
	{ARTIFACT::SMOKE_POWDER, "�Come get it today! Our newly patented powder that, when thrown on the ground, produces a cloud of smoke that lets you slip out of sight far easier than ever before!�"},
	{ARTIFACT::LIVELY_BULB, "You may have unintentionally allowed this plant to think you�re its parent. You also may have named it �Bobby�."},
	{ARTIFACT::MALEDICTION, "�Your suffering�I want to savour it!�"},
	{ARTIFACT::CHIMERARM, "A disfigured limb belonging to a monster of unknown origin. It seems to be wrapped in a strange aura that warps nearby weapons in an inexplicable way. You hear a strange voice from the back of your head saying that you can use it to create an armament of unparalleled power."}
};

// Artifact texture map
const std::map <ARTIFACT, TEXTURE_ASSET_ID>artifact_textures = {
	{ARTIFACT::POISON_FANG, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::GLAD_HOPLON, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::BLADE_POLISH, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::HQ_FLETCHING, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::MESSENGER_CAP, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::WARM_CLOAK, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::THUNDER_TWIG, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::LUCKY_CHIP, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::GUIDE_HEALBUFF, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::THICK_TOME, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::GOLIATH_BELT, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::BLOOD_RUBY, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::WINDBAG, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::KB_MALLET, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::ARCANE_SPECS, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::SCOUT_STRIDE, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::ART_CONSERVE, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::ARCANE_FUNNEL, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::FUNGIFIER, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::BURRBAG, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::SMOKE_POWDER, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::LIVELY_BULB, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::MALEDICTION, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER},
	{ARTIFACT::CHIMERARM, TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER}
};

// Artifact effect map
const std::map <ARTIFACT, std::string>artifact_effects = {
	{ARTIFACT::POISON_FANG, "Attacks have a 30% chance to inflict 15% (+10% per stack) ATK Poison DoT for 5 turns."},
	{ARTIFACT::GLAD_HOPLON, "Take 15% (+10% multiplicative per stack) reduced damage from enemies that you attacked directly until your next turn."},
	{ARTIFACT::BLADE_POLISH, "Melee attacks deal 20% (+20% per stack) additional damage."},
	{ARTIFACT::HQ_FLETCHING, "Ranged attacks deal 20% (+20% per stack) additional damage."},
	{ARTIFACT::MESSENGER_CAP, "10% (+5% per stack) of your base ATK stat is added onto your Speed stat."},
	{ARTIFACT::WARM_CLOAK, "10% (+5% per stack) of your base ATK stat is added onto your DEF stat."},
	{ARTIFACT::THUNDER_TWIG, "Attacks have a 15% (+15% per stack) chance to summon a lightning bolt that deals 60% ATK damage in a small AoE."},
	{ARTIFACT::LUCKY_CHIP, "7% (+7% additive) chance for your attack to deal 777% damage. 7 % (+7 % additive) chance to reduce incoming damage by 777. Lowest damage taken per attack is 1."},
	{ARTIFACT::GUIDE_HEALBUFF, "Health-restoring items and interactables grant a 30% (+30% per stack) ATK buff for 5 turns."},
	{ARTIFACT::THICK_TOME, "Upon taking lethal damage, survive with 1 HP and gain 3 turns of invincibility. This artifact is consumed when this effect activates."},
	{ARTIFACT::GOLIATH_BELT, "test effects"},
	{ARTIFACT::BLOOD_RUBY, "When HP is below 40%, increases ATK by 20% (+20% per stack)."},
	{ARTIFACT::WINDBAG, "Upon taking damage that puts you below 25% (+5% per stack) HP, release an AoE that knocks back nearby enemies by 300 units, and stuns them for 3 (+1 per stack) turns. Has a 15 turn cooldown."},
	{ARTIFACT::KB_MALLET, "When attacking an enemy within melee range, knock back struck enemies by 100 units (+ 50 units per stack)."},
	{ARTIFACT::ARCANE_SPECS, "Gain 50 (+50 per stack) units of sight range."},
	{ARTIFACT::SCOUT_STRIDE, "Consume 10% (+10% multiplicative per stack) less EP when moving."},
	{ARTIFACT::ART_CONSERVE, "Consume 10% (+10% multiplicative per stack) less EP when attacking."},
	{ARTIFACT::ARCANE_FUNNEL, "Upon defeating an enemy, gain a buff that doubles your MP regeneration for 1 (+1 per stack) turns."},
	{ARTIFACT::FUNGIFIER, "Upon defeating an enemy, an explosive mushroom is dropped at their location. When an enemy steps on the mushroom, or after 3 turns, the mushroom explodes, dealing 130% (+130% per stack) ATK in damage in a small AoE."},
	{ARTIFACT::BURRBAG, "At the start of each turn, leave a patch of burrs on the ground that last for 5 turns or until activated 1 (+1 per stack) times. Enemies that step over the burrs will take 40% ATK in damage and can move only 50% of their regular distance on their next turn."},
	{ARTIFACT::SMOKE_POWDER, "Upon picking up an item, release a cloud of smoke that halves the aggro range of enemies within 150 (+100 per stack) units for 1 turn."},
	{ARTIFACT::LIVELY_BULB, "Whenever you perform a Normal Attack, fire 1 (+1 per stack) seed projectile that deals 90% ATK damage towards the lowest HP enemy within your sight range."},
	{ARTIFACT::MALEDICTION, "When you are attacked, all enemies in sight range will be affected with a curse that reduces their ATK by 40% for 3 turns. Has a 10 (-1 per stack) turn cooldown."},
	{ARTIFACT::CHIMERARM, "Your current weapon, and newly generated weapons will have +4 ATK (+4 ATK per stack), and its 2nd Attack Skill will become a random attack skill from any weapon type."}
};