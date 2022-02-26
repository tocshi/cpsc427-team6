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
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

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

	// music references
	Mix_Music* background_music;
	Mix_Chunk* chicken_dead_sound;
	Mix_Chunk* chicken_eat_sound;
	Mix_Chunk* fire_explosion_sound;
	Mix_Chunk* error_sound;
	Mix_Chunk* footstep_sound;

	// log text
	void logText(std::string msg);

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse(int button, int action, int mod);
	void on_mouse_move(vec2 pos);

	// restart level
	void restart_game();

	// spawn entities
	void spawn_game_entities();

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

	// load entities from saved data
	void loadFromData(json data);

	// load player from data
	void loadPlayer(json playerData);
  
	// load enemies from data
	void loadEnemies(json enemyData);

	// load a slime from data
	void loadSlime(json slimeData);

	// load motion data
	Motion loadMotion(json motionData);
  
  // do turn order logic
	void doTurnOrderLogic();

	// OpenGL window handle
	GLFWwindow* window;

	// Number of bug eaten by the chicken, displayed in the window title
	unsigned int points;

	// Game state
	RenderSystem* renderer;
	float current_speed;
	float next_eagle_spawn;
	float next_bug_spawn;

	Entity active_camera_entity;

	Entity player_chicken;
	Entity background;
	bool is_player_turn = true;
	bool player_move_click = false;
	bool is_ai_turn = false;

	SaveSystem saveSystem;
	TurnOrderSystem turnOrderSystem;
	AISystem aiSystem;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};

// Set attack state for enemies that attack after moving
void set_enemy_state_attack(Entity enemy);

// set gamestate
void set_gamestate(GameStates state);