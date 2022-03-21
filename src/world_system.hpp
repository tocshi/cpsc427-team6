#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "save_system.hpp"
#include "turn_order_system.hpp"
#include "ai_system.hpp"
#include "game_state.hpp"
#include "room_system.hpp"

#include <../ext/json/single_include/nlohmann/json.hpp>

using json = nlohmann::json;


// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init (RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// free music 
	void destroyMusic();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

	bool get_is_player_turn();

	void set_is_player_turn(bool val);

	bool get_is_ai_turn();

	void set_is_ai_turn(bool val);

	void start_player_turn();

	// Game state
	Entity player_main;

	// Tutorial flags
	bool tutorial = true;
	bool firstSign = false;
	bool movementSelected = false;
	bool epDepleted = false;
	bool secondSign = false;
	bool slimeDefeated = false;
	bool interactedCampfire = false;
	bool thirdSign = false;

	Entity tutorial_sign_1;
	Entity tutorial_sign_2;
	Entity tutorial_sign_3;
	Entity tutorial_slime;
	Entity tutorial_campfire;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* fire_explosion_sound;
	Mix_Chunk* error_sound;
	Mix_Chunk* footstep_sound;
	Mix_Chunk* door_sound;
	Mix_Chunk* switch_sound;
	Mix_Chunk* chest_sound;

	Mix_Music* menu_music;
	Mix_Music* cutscene_music;
	// Game state
	RenderSystem* renderer;
	float current_speed;

	Entity active_camera_entity;

	Entity background;
	Entity background_front;
	Entity background_mid;
	Entity background_back;
	bool is_player_turn = true;
	bool player_move_click = false;
	bool is_ai_turn = false;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	Entity turnUI;

	// log text
	void logText(std::string msg);

	void spawn_doors_random_location(int quantity);
	void spawn_switches_random_location(int quantity);

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse(int button, int action, int mod);
	void on_mouse_move(vec2 pos);
	
	// start of cut scene 
	void cut_scene_start();
	
	// restart level
	void restart_game();

	// spawn entities (tutorial)
	void spawn_tutorial_entities();

	// spawn entities
	void spawn_game_entities();

	// create ep range
	void create_ep_range(float remaining_ep, float speed, vec2 pos);

	// create fog of war
	void create_fog_of_war();

	// remove all fog entities
	void remove_fog_of_war();

	// spawn player in random location
	void spawn_player_random_location(std::vector<vec2>& playerSpawns);

	// spawn enemies in random location
	void spawn_enemies_random_location(std::vector<vec2>& enemySpawns, int min, int max);

	// spawn items in random location
	void spawn_items_random_location(std::vector<vec2>& itemSpawns, int min, int max);

	// remove entities for loading
	void removeForLoad();

	// remove entities for new room
	void removeForNewRoom();

	// load entities from saved data
	void loadFromData(json data);

	// load player from data
	Entity loadPlayer(json playerData);
  
	// load enemies from data
	Entity loadEnemy(json enemyData);

	// load motion data
	void loadMotion(Entity e, json motionData);

	// load stats data
	void loadStats(Entity e, json statsData);

	// load queueable data
	void loadQueueable(Entity e, json queueableData);

	// load enemy component
	void loadEnemyComponent(Entity e, json enemyCompData, Inventory inv);

	// load player component
	void loadPlayerComponent(Entity e, json playerCompData, Inventory inv);

	// load inventory
	Inventory loadInventory(Entity e, json inventoryData);

	// load statuses
	void loadStatuses(Entity e, json statusData);

	// load tiles
	void loadTiles(json tileData);

	// load collidables
	void loadCollidables(json collidableData);

	// load interactables
	void loadInteractables(json interactablesList);

	// load a sign
	void loadSign(Entity e, json signData);

	// load a chest
	void loadChest(Entity e, json chestData);

	// load an item pickup
	void loadEquipmentEntity(Entity e, json equipData, json spritesheetData);

	// load artifact data
	void loadArtifact(Entity e, json artifactData);

	// load a door
	void loadDoor(Entity e);

	// load a switch
	void loadSwitch(Entity e, json switchData);
  
	// do turn order logic
	void doTurnOrderLogic();

	// handle end of player's turn
	void handle_end_player_turn(Entity player);

	// item action
	void itemAction();

	// attack action
	void attackAction();

	// move action
	void moveAction();

	// cacnel action
	void cancelAction();

	// back action
	void backAction();

	// action button helper
	void handleActionButtonPress();
	// generate and setup a new room
	void generateNewRoom(Floors floor, bool repeat_allowed);

	// update game background
	void updateGameBackground();

	// udate turn UI
	void update_turn_ui();

	// update tutorial flags
	void WorldSystem::updateTutorial();

	// OpenGL window handle
	GLFWwindow* window;

	SaveSystem saveSystem;
	TurnOrderSystem turnOrderSystem;
	AISystem aiSystem;
	RoomSystem roomSystem;

	SpawnData spawnData;
	
	int countCutScene = 0;
};

// Set attack state for enemies that attack after moving
void set_enemy_state_attack(Entity enemy);

// set gamestate
void set_gamestate(GameStates state);

// check if entity has a status effect;
bool has_status(Entity e, StatusType status);

// Remove a number of a status effect type from entity
void remove_status(Entity e, StatusType status, int number = 1);

extern WorldSystem world;