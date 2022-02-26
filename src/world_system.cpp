// Header
#include "world_system.hpp"
#include "world_init.hpp"
// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include<fstream>

#include "physics_system.hpp"
#include "combat_system.hpp"

// Game configuration
const size_t MAX_EAGLES = 15;
const size_t MAX_BUG = 5;
const size_t EAGLE_DELAY_MS = 2000 * 3;
const size_t BUG_DELAY_MS = 5000 * 3;
// decalre gamestates
//GameStates game_state = GameStates::CUTSCENE;

// Create the bug world
WorldSystem::WorldSystem()
	: points(0)
	, next_eagle_spawn(0.f)
	, next_bug_spawn(0.f) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (chicken_dead_sound != nullptr)
		Mix_FreeChunk(chicken_dead_sound);
	if (chicken_eat_sound != nullptr)
		Mix_FreeChunk(chicken_eat_sound);
	if (fire_explosion_sound != nullptr)
		Mix_FreeChunk(fire_explosion_sound);
	if (error_sound != nullptr)
		Mix_FreeChunk(error_sound);
	if (footstep_sound != nullptr)
		Mix_FreeChunk(footstep_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// bool current_game_state; (previious)
// In start menu (CHANGE TO INT TO SEE IF IT WORKS)
// CUTSCENE IS null
GameStates current_game_state;
GameStates previous_game_state = current_game_state;


// fog stats
float fog_radius = 450.f;
float fog_resolution = 2000.f;

// ep range stats
float ep_resolution = 2000.f;

// move audio timer
float move_audio_timer_ms = 200.f;

// hide guard buttons
bool hideGuardButton = false;

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Adrift In Somnium", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto mouse_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse(_0, _1, _2); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetMouseButtonCallback(window, mouse_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	// Set the game to start on the menu screen
	previous_game_state = current_game_state;
	//printf("Previous Game State : Game state = MAIN_MENU");
	//printf()
	current_game_state = GameStates::MAIN_MENU;
	printf("previous state in Now %d \n", static_cast<int>(previous_game_state));

	// set previous_game_state to current_game_state
	previous_game_state = current_game_state;
	printf("ACTION: SET THE GAME TO START : Game state = MAIN_MENU\n");
	printf("Current current_game_state Game state %d \n", static_cast<int>(current_game_state));
	printf("previous state in Now %d \n", static_cast<int>(previous_game_state));
	

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}
	// Music and volumes
	Mix_VolumeMusic(10);
	background_music = Mix_LoadMUS(audio_path("bgm/caves0.wav").c_str());

	// Sounds and volumes
	chicken_dead_sound = Mix_LoadWAV(audio_path("chicken_dead.wav").c_str());
	chicken_eat_sound = Mix_LoadWAV(audio_path("chicken_eat.wav").c_str());
	fire_explosion_sound = Mix_LoadWAV(audio_path("feedback/fire_explosion.wav").c_str());
	Mix_VolumeChunk(fire_explosion_sound, 13);
	error_sound = Mix_LoadWAV(audio_path("feedback/error.wav").c_str());
	Mix_VolumeChunk(error_sound, 13);
	footstep_sound = Mix_LoadWAV(audio_path("feedback/footstep.wav").c_str());
	Mix_VolumeChunk(footstep_sound, 14);

	if (background_music == nullptr || chicken_dead_sound == nullptr || chicken_eat_sound == nullptr 
		|| fire_explosion_sound == nullptr || error_sound == nullptr || footstep_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("bgm/caves0.wav").c_str(),
			audio_path("chicken_dead.wav").c_str(),
			audio_path("chicken_eat.wav").c_str(),
			audio_path("feedback/fire_explosion.wav").c_str(),
			audio_path("feedback/error.wav").c_str(),
			audio_path("feedback/footstep.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Adrift In Somnium: Alpha Build";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	    Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
			if(!registry.players.has(motions_registry.entities[i])) // don't remove the player
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	// if not in menu do turn order logic (!current_game_state)
	if (current_game_state < GameStates::CUTSCENE && current_game_state >= GameStates::GAME_START) {
		doTurnOrderLogic();
	}

	

	// perform in-motion behaviour
	if (get_is_player_turn() && player_move_click) {
		for (Entity player : registry.players.entities) {
			Motion player_motion = registry.motions.get(player);
			Stats stats = registry.stats.get(player);
			if (player_motion.in_motion) {
				// handle footstep sound
				if (move_audio_timer_ms <= 0) {
					// play the footstep sound
					Mix_PlayChannel(-1, footstep_sound, 0);
					move_audio_timer_ms = 200.f;
				}
				else {
					move_audio_timer_ms -= 20.f;
				}
				// update the fog of war if the player is moving
				remove_fog_of_war();
				create_fog_of_war();

				// remove old ep range
				for (Entity epr : registry.epRange.entities) {
					registry.remove_all_components_of(epr);
				}
				// update ep range
				create_ep_range(stats.ep, player_motion.movement_speed, player_motion.position);
			}
			else {
				// if in movement mode, show the new ep range
				if (current_game_state == GameStates::MOVEMENT_MENU) {
					// remove old ep range
					for (Entity epr : registry.epRange.entities) {
						registry.remove_all_components_of(epr);
					}
					create_ep_range(stats.ep, player_motion.movement_speed, player_motion.position);
				}
				player_move_click = false;
			}
		}
	}

	// If started, remove menu entities, and spawn game entities
	//if(!current_game_state) (
	//current_game_state > GameStates::CUTSCENE || current_game_state <GameStates::SPLASH_SCREEN
	if (current_game_state == GameStates::GAME_START || current_game_state == GameStates::BATTLE_MENU) {
		// remove all menu entities
		for (Entity e : registry.menuItems.entities) {
			registry.remove_all_components_of(e);
		}

		// bring back all of the buttons
		createMoveButton(renderer, { window_width_px - 1400.f, window_height_px - 50.f });
		createAttackButton(renderer, { window_width_px - 1000.f, window_height_px - 50.f });
		createGuardButton(renderer, { window_width_px - 600.f, window_height_px - 50.f }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD);
		createItemButton(renderer, { window_width_px - 200.f, window_height_px - 50.f });

		// hide all the visulaiztion tools
		for (Entity mvo : registry.modeVisualizationObjects.entities) {
			registry.remove_all_components_of(mvo);
		}

		// destory ep range
		for (Entity ep : registry.epRange.entities) {
			registry.remove_all_components_of(ep);
		}

		hideGuardButton = false;
	}
	// Update HP/MP/EP bars and movement
	// Check for player death
	for (Entity player : registry.players.entities) {
		Player& p = registry.players.get(player);
		
		// get player stats
		float& maxep = registry.stats.get(player).maxep;
		float& hp = registry.stats.get(player).hp;
		float& mp = registry.stats.get(player).mp;
		float& ep = registry.stats.get(player).ep;

		// Check if player has died
		if (hp <= 0 && !registry.deathTimers.has(player)) {
			registry.deathTimers.emplace(player);
			logText("You have died!");
			player_move_click = false;
			break;
		}

		// change guard button to end turn if ep is not full
		if (ep < maxep && !hideGuardButton) {
			// remove guard button
			for (Entity gb : registry.guardButtons.entities) {
				registry.remove_all_components_of(gb);
			}
			// add end turn button
			createGuardButton(renderer, { window_width_px - 600.f, window_height_px - 50.f }, BUTTON_ACTION_ID::ACTIONS_END_TURN, TEXTURE_ASSET_ID::ACTIONS_END_TURN);
		}
		else if (!hideGuardButton) {
			// remove guard button
			for (Entity gb : registry.guardButtons.entities) {
				registry.remove_all_components_of(gb);
			}
			// add end turn button
			createGuardButton(renderer, { window_width_px - 600.f, window_height_px - 50.f }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD);
		}

		// update player motion
		Motion& player_motion = registry.motions.get(player);
		if (player_motion.in_motion) {
			if (ep <= 0) {
				handle_end_player_turn(player);
			}
			else { 
				float ep_rate = 1.f;
				ep -= 0.03f * ep_rate * elapsed_ms_since_last_update; 
			}
		}
		
		// update Stat Bars and visibility
		for (Entity entity : registry.motions.entities) {
			if (!registry.renderRequests.has(entity)) {
				continue;
			}
			Motion& motion_struct = registry.motions.get(entity);
			RenderRequest& render_struct = registry.renderRequests.get(entity);
			
			switch (render_struct.used_texture) {
			case TEXTURE_ASSET_ID::HPFILL:
				motion_struct.scale = { (hp / 100.f) * STAT_BB_WIDTH, STAT_BB_HEIGHT };
				motion_struct.position[0] = 150.f - 150.f*(1.f - (hp / 100.f));	// original pos (full bar) - (1-multiplier)
				break;
			case TEXTURE_ASSET_ID::MPFILL:
				motion_struct.scale = { (mp / 100.f) * STAT_BB_WIDTH, STAT_BB_HEIGHT };
				motion_struct.position[0] = 150.f - 150.f*(1.f - (mp / 100.f));	// original pos (full bar) - (1-multiplier)
				break;
			case TEXTURE_ASSET_ID::EPFILL:
				motion_struct.scale = { (ep / 100.f) * STAT_BB_WIDTH, STAT_BB_HEIGHT };
				motion_struct.position[0] = 150.f - 150.f*(1.f - (ep / 100.f));	// original pos (full bar) - (1-multiplier)
				break;
			}

			// Hide certain entities that are outside of the player's sight range
			// don't hide walls, signs, stairs, doors
			if (!registry.hidables.has(entity))
				continue;

			float distance_to_player = 
				sqrt(pow((motion_struct.position.x - player_motion.position.x), 2) 
				+ pow((motion_struct.position.y - player_motion.position.y), 2));
			
			if (distance_to_player > fog_radius) {
				if (!registry.hidden.has(entity)) {
					registry.hidden.emplace(entity);
				}
			}
			else {
				if (registry.hidden.has(entity)) {
					registry.hidden.remove(entity);
				}
			}
		}

		// Update the camera to follow the player
		Camera& camera = registry.cameras.get(active_camera_entity);
		camera.position = player_motion.position - vec2(window_width_px/2, window_height_px/2);
	}

	// Check for enemy death
	for (Entity& enemy : registry.enemies.entities) {
		if (registry.stats.get(enemy).hp <= 0 && !registry.squishTimers.has(enemy)) {
			std::string log_text = "The enemy ";
			log_text = log_text.append(registry.stats.get(enemy).name.append(" is defeated!"));
			logText(log_text);
			SquishTimer& squish = registry.squishTimers.emplace(enemy);
			squish.orig_scale = registry.motions.get(enemy).scale;
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE EGG SPAWN HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Processing the chicken state
	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

    float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if(counter.counter_ms < min_counter_ms){
		    min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
            restart_game();
			return true;
		}
	}
	// reduce window brightness if any of the present chickens is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	// Text Timers
	for (Entity entity : registry.textTimers.entities) {
		// progress timer
		TextTimer& counter = registry.textTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if(counter.counter_ms < min_counter_ms){
		    min_counter_ms = counter.counter_ms;
		}

		// remove text once the text timer has expired
		if (counter.counter_ms < 0) {
			registry.remove_all_components_of(entity);
		}
	}

	// Wobble Timers
	for (Entity entity : registry.wobbleTimers.entities) {
		// prioritize squish over wobble
		if (registry.squishTimers.has(entity)) { break; }
		// progress timer
		WobbleTimer& counter = registry.wobbleTimers.get(entity);
		//float x_scale = (counter.orig_scale.x / 4) * sin(counter.counter_ms/100) + counter.orig_scale.x;
		//float y_scale = (counter.orig_scale.x / 4) * sin(counter.counter_ms/100 + M_PI) + counter.orig_scale.x;
		float x_scale = (pow(counter.counter_ms, 2) / 800000) * cos(counter.counter_ms / 50) + counter.orig_scale.x;
		float y_scale = (pow(counter.counter_ms, 2) / 800000) * cos(counter.counter_ms / 50 + M_PI) + counter.orig_scale.x;
		registry.motions.get(entity).scale = {x_scale, y_scale};
		counter.counter_ms -= elapsed_ms_since_last_update;

		// remove entity once the timer has expired
		if (counter.counter_ms < 0) {
			registry.motions.get(entity).scale = counter.orig_scale;
			registry.wobbleTimers.remove(entity);
		}
	}

	// Squish Timers
	for (Entity entity : registry.squishTimers.entities) {
		// progress timer
		SquishTimer& counter = registry.squishTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		float x_scale = registry.motions.get(entity).scale.x;
		float y_scale = registry.motions.get(entity).scale.y / 1.04f;
		registry.motions.get(entity).scale = { x_scale, y_scale };
		registry.motions.get(entity).position.y += (y_scale * 1.04 - y_scale)/2;

		// remove entity once the timer has expired
		if (counter.counter_ms < 0) {
			registry.remove_all_components_of(entity);
		}
	}

	// update animations 
	for (int i = 0; i < registry.animations.size(); i++) {
		Entity e = registry.animations.entities[i];
		AnimationData& anim = registry.animations.get(e);
		anim.animation_time_ms += elapsed_ms_since_last_update;
		if (anim.animation_time_ms > anim.frametime_ms * anim.frame_indices.size() - 1) {
			if (!anim.loop) {
				anim.animation_time_ms = anim.frametime_ms * anim.frame_indices.size() - 1;
				if (anim.delete_on_finish) {
					registry.remove_all_components_of(e);
				}
				continue;
			}
			anim.animation_time_ms -= anim.frametime_ms * anim.frame_indices.size() - 1;
		}
		anim.current_frame = std::min(anim.animation_time_ms / anim.frametime_ms, int(anim.frame_indices.size()) - 1);
	}

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;
	
	// Remove all entities that we created
	// All that have a motion, we could also iterate over all bug, eagles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());
	
	while (registry.texts.entities.size() > 0)
		registry.remove_all_components_of(registry.texts.entities.back());

	while (registry.cameras.entities.size() > 0)
		registry.remove_all_components_of(registry.cameras.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Create the map/level/background
	background = createBackground(renderer, vec2(window_width_px/2,window_height_px/2));

	// Add a camera entity
	active_camera_entity = createCamera({0, 0});

	//// Create a new chicken
	//player_chicken = createChicken(renderer, { window_width_px/2, window_height_px - 200 });
	//registry.colors.insert(player_chicken, {1, 0.8f, 0.8f});

	// !! TODO A3: Enable static eggs on the ground
	// Create eggs on the floor for reference
	/*
	for (uint i = 0; i < 20; i++) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		float radius = 30 * (uniform_dist(rng) + 0.3f); // range 0.3 .. 1.3
		Entity egg = createEgg({ uniform_dist(rng) * w, h - uniform_dist(rng) * 20 },
			         { radius, radius });
		float brightness = uniform_dist(rng) * 0.5 + 0.5;
		registry.colors.insert(egg, { brightness, brightness, brightness});
	}
	*/

	// restart the game on the menu screen
	//current_game_state = true;
	set_gamestate(GameStates::MAIN_MENU);

	/*if (current_game_state != GameStates::MAIN_MENU) {
		//current_game_state = GameStates::MAIN_MENU;
		std::cout << "ACTION: RESTART THE GAME ON THE MENU SCREEN : Game state = MAIN_MENU" << std::endl;
		//printf("ACTION: RESTART THE GAME ON THE MENU SCREEN : Game state = MAIN_MENU");
	}*/
	//current_game_state = GameStates::MAIN_MENU;
	//printf("ACTION: RESTART THE GAME ON THE MENU SCREEN : Game state = MAIN_MENU");

	// For testing textures
	//createPlayer(renderer, {50.f, 250.f});
	//createEnemy(renderer, {50.f, 350.f});
	//createBoss(renderer, {50.f, 450.f});
	//createArtifact(renderer, {50.f, 550.f});
	//createConsumable(renderer, {50.f, 650.f});
	//createEquipable(renderer, {150.f, 250.f});
	//createChest(renderer, {150.f, 350.f});
	//createDoor(renderer, {150.f, 450.f});
	//createSign(renderer, {150.f, 550.f});
	//createStair(renderer, {150.f, 650.f});
	createMenuStart(renderer, { window_width_px / 2, 500.f });
	createMenuQuit(renderer, { window_width_px / 2, 850.f });
	createMenuTitle(renderer, { window_width_px / 2, 200.f });

	// testing text
	// createText(renderer, vec2(200.f, 200.f), "abcdefghijklmnopqrstuvwxyz", 1.5f, vec3(1.0f, 0.0f, 0.0f));
	// createText(renderer, vec2(200.f, 300.f), "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 1.5f, vec3(1.0f, 0.0f, 0.0f));
	// createText(renderer, vec2(200.f, 400.f), "0123456789", 1.5f, vec3(1.0f, 0.0f, 0.0f));
	// createText(renderer, vec2(200.f, 500.f), ",./:;'()[]", 1.5f, vec3(1.0f, 0.0f, 0.0f));
	// createText(renderer, vec2(0.f, 50.f), "test,. '123", 1.5f, vec3(1.0f));
	// createText(renderer, vec2(200.f, 500.f), ".'", 2.f, vec3(1.0f));
}

void WorldSystem::handle_end_player_turn(Entity player) {
	Motion& player_motion = registry.motions.get(player);
	Player& p = registry.players.get(player);
	player_motion.velocity = { 0.f, 0.f };
	player_motion.in_motion = false;
	p.attacked = false;
	set_is_player_turn(false);
	player_move_click = false;
	logText("It is now the enemies' turn!");
	// set player's doing_turn to false
	registry.queueables.get(player).doing_turn = false;
	set_gamestate(GameStates::ENEMY_TURN);
}

// spawn the game entities
void WorldSystem::spawn_game_entities() {

	SpawnData spawnData = createTiles(renderer, "map1_random.tmx");

	// create all non-menu game objects
	// spawn the player and enemy in random locations
	spawn_player_random_location(spawnData.playerSpawns);
	spawn_enemies_random_location(spawnData.enemySpawns, spawnData.minEnemies, spawnData.maxEnemies);
	spawn_items_random_location(spawnData.itemSpawns, spawnData.minItems, spawnData.maxItems);
  
	/*
	createBoss(renderer, { 250.f, 450.f });
	createArtifact(renderer, { 250.f, 550.f });
	createConsumable(renderer, { 250.f, 650.f });
	createEquipable(renderer, { 350.f, 250.f });
	createChest(renderer, { 350.f, 350.f });
	createDoor(renderer, { 350.f, 450.f });
	createSign(renderer, { 350.f, 550.f });
	createStair(renderer, { 350.f, 650.f });
	*/

	// setup turn order system
	turnOrderSystem.setUpTurnOrder();
	// start first turn
	turnOrderSystem.getNextTurn();
	/*
	for (uint i = 0; WALL_BB_WIDTH / 2 + WALL_BB_WIDTH * i < window_width_px; i++) {
		createWall(renderer, { WALL_BB_WIDTH / 2 + WALL_BB_WIDTH * i, WALL_BB_HEIGHT / 2 });
		createWall(renderer, { WALL_BB_WIDTH / 2 + WALL_BB_WIDTH * i, window_height_px - WALL_BB_HEIGHT / 2 });
	}
	for (uint i = 1; WALL_BB_HEIGHT / 2 + WALL_BB_HEIGHT * i < window_height_px - WALL_BB_HEIGHT; i++) {
		createWall(renderer, { WALL_BB_WIDTH / 2, WALL_BB_HEIGHT / 2 + WALL_BB_HEIGHT * i });
		createWall(renderer, { window_width_px - WALL_BB_WIDTH / 2, WALL_BB_HEIGHT / 2 + WALL_BB_HEIGHT * i });
	}
	*/
	
	float statbarsX = 150.f;
	float statbarsY = 35.f;
	createHPFill(renderer, { statbarsX, statbarsY });
	createHPBar(renderer,  { statbarsX, statbarsY });
	createMPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createMPBar(renderer,  { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createEPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	createEPBar(renderer,  { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	create_fog_of_war();
}

// render ep range around the given position
void WorldSystem::create_ep_range(float remaining_ep, float speed, vec2 pos) {
	float ep_radius = remaining_ep * speed * 0.03 + ((110.f * remaining_ep) / 100);

	Entity ep = createEpRange({ pos.x , pos.y }, ep_resolution, ep_radius, { window_width_px, window_height_px });
	registry.colors.insert(ep, { 0.2, 0.2, 8.7 });
}

// render fog of war around the player
void WorldSystem::create_fog_of_war() {	
	for (Entity player : registry.players.entities) {
		// get player position
		Motion player_motion = registry.motions.get(player);
		float playerX = player_motion.position.x;
		float playerY = player_motion.position.y;

		Entity fog = createFog({ playerX, playerY }, fog_resolution, fog_radius, { window_width_px, window_height_px });
		registry.colors.insert(fog, { 0.2, 0.2, 0.2 });
	}
}

// remove all fog entities
void WorldSystem::remove_fog_of_war() {
	for (Entity e : registry.fog.entities) {
		registry.remove_all_components_of(e);
	}

}

// spawn player entity in random location
void WorldSystem::spawn_player_random_location(std::vector<vec2>& playerSpawns) {
	std::random_shuffle(playerSpawns.begin(), playerSpawns.end());
	if (playerSpawns.size() > 0) {
		createPlayer(renderer, { playerSpawns[0].x, playerSpawns[0].y});
		return;
	}
	// default spawn location in case we don't have player spawns
	createPlayer(renderer, { 0, 0 });

}

// spawn enemy entities in random locations
void WorldSystem::spawn_enemies_random_location(std::vector<vec2>& enemySpawns, int min, int max) {
	std::random_shuffle(enemySpawns.begin(), enemySpawns.end());
	if (enemySpawns.size() > 0) {
		int numberToSpawn = std::min(irandRange(min, max + 1), int(enemySpawns.size()));
		for (int i = 0; i < numberToSpawn; i++) {
			createEnemy(renderer, { enemySpawns[i].x, enemySpawns[i].y });
		}
	}
}

// spawn item entities in random locations
void WorldSystem::spawn_items_random_location(std::vector<vec2>& itemSpawns, int min, int max) {
	std::random_shuffle(itemSpawns.begin(), itemSpawns.end());
	if (itemSpawns.size() > 0) {
		int numberToSpawn = std::min(irandRange(min, max + 1), int(itemSpawns.size()));
		Entity& player = registry.players.entities[0];
		Motion& motion = registry.motions.get(player);
		int range = 0;
		if (registry.stats.has(player)) {
			range = registry.stats.get(player).range;
		}

		int spawned = 0;
		int i = 0;
		while (spawned < numberToSpawn && i < itemSpawns.size()) {
			// temporary, later we can also randomize the item types
			if (range > 0) {
				if (dist_to(motion.position, itemSpawns[i]) <= range) {
					i++;
					continue;
				}
			}
			createCampfire(renderer, { itemSpawns[i].x, itemSpawns[i].y });
			spawned++;
			i++;
		}
	}
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE CHICKEN MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// LOGGING TEXT TEST
	if (action == GLFW_PRESS && key == GLFW_KEY_P) {
		for (Entity& player : registry.players.entities) {
			for (Entity& enemy : registry.slimeEnemies.entities) {
				int test = irandRange(100,200);
				std::string log_message = deal_damage(enemy, player, test);

				logText(log_message);
				printf("testing combat with 10 ATK and %d multiplier\n", test);
			}
		}
	}

	// SAVING THE GAME
	if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
		saveSystem.saveGameState(turnOrderSystem.getTurnOrder());
		logText("Game state saved!");
	}

	// LOADING THE GAME
	if (action == GLFW_RELEASE && key == GLFW_KEY_L && get_is_player_turn() ) {
		// if save data exists reset the game
		if (saveSystem.saveDataExists()) {
			// remove entities to load in entities
			removeForLoad();
			// get saved game data
			json gameData = saveSystem.getSaveData();
			// load the entities in
			std::queue<Entity> entities = loadFromData(gameData);
			turnOrderSystem.loadTurnOrder(entities);
			saveSystem.readJsonFile(); // LOAD REST OF DATA FOR ARTIFACT etc.
		}

		logText("Game state loaded!");
		remove_fog_of_war();
		create_fog_of_war();
	}

	// simulating a new room
	if (action == GLFW_RELEASE && key == GLFW_KEY_N && get_is_player_turn()) {
		// remove all entities for new room
		removeForNewRoom();
		// save game (should be just player stuff)
		saveSystem.saveGameState(turnOrderSystem.getTurnOrder());
		// remove player
		for (Entity e : registry.players.entities) {
			registry.remove_all_components_of(e);
		}
		// make new map
		SpawnData spawnData = createTiles(renderer, "map1_random.tmx");
		// load the player back
		json gameData = saveSystem.getSaveData();
		std::queue<Entity> queue = loadFromData(gameData);
		turnOrderSystem.loadTurnOrder(queue);
		// get the player and set its position
		for (Entity e : registry.players.entities) {
			std::random_shuffle(spawnData.playerSpawns.begin(), spawnData.playerSpawns.end());
			Motion& motion = registry.motions.get(e);
			Stats& stats = registry.stats.get(e);
			// set random position
			motion.position = { spawnData.playerSpawns[0].x, spawnData.playerSpawns[0].y };
			// set everything else in motion to default
			motion.angle = 0.f;
			motion.velocity = { 0.f, 0.f };
			motion.in_motion = false;
			motion.movement_speed = 200;
			motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });

			// Refill Player EP
			stats.ep = stats.maxep;

		}
		remove_fog_of_war();
		create_fog_of_war();
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_T) {
		printf("GAME STATE LOG START ============\n");
		printf("Previous game state is: %i\n", static_cast<int>(previous_game_state));
		printf("Current game state is: %i\n", static_cast<int>(current_game_state));
		printf("GAME STATE LOG END ============\n\n\n");
	}

	// Debugging
	if (key == GLFW_KEY_D) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);
}

// On mouse click callback
void WorldSystem::on_mouse(int button, int action, int mod) {

	double xpos, ypos;
	//getting cursor position
	glfwGetCursorPos(window, &xpos, &ypos);
	//printf("Cursor Position at (%f, %f)\n", xpos, ypos);

	// get cursor position relative to world
	Camera camera = registry.cameras.get(active_camera_entity);
	vec2 world_pos = {xpos + camera.position.x, ypos + camera.position.y};
	//printf("World Position at (%f, %f)\n", world_pos.x, world_pos.y);

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {

		///////////////////////////
		// logic for button presses
		///////////////////////////

		for (Entity e : registry.buttons.entities) {
			if (!registry.motions.has(e)) {
				continue;
			}
			Motion m = registry.motions.get(e);
			int buttonX = m.position[0];
			int buttonY = m.position[1];
			// if mouse is interating with a button
			if ((xpos <= (buttonX + m.scale[0] / 2) && xpos >= (buttonX - m.scale[0] / 2)) &&
				(ypos >= (buttonY - m.scale[1] / 2) && ypos <= (buttonY + m.scale[1] / 2))) {
				if (!registry.buttons.has(e)) {
					continue;
				}
				// perform action based on button ENUM
				BUTTON_ACTION_ID action_taken = registry.buttons.get(e).action_taken;

				switch (action_taken) {

					case BUTTON_ACTION_ID::MENU_START: 
						set_gamestate(GameStates::GAME_START);
						spawn_game_entities();
						// spawn the actions bar
						// createActionsBar(renderer, { window_width_px / 2, window_height_px - 100.f });
						createMoveButton(renderer, { window_width_px - 1400.f, window_height_px - 50.f });
						createAttackButton(renderer, { window_width_px - 1000.f, window_height_px - 50.f });
						createGuardButton(renderer, { window_width_px - 600.f, window_height_px - 50.f }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD);
						createItemButton(renderer, { window_width_px - 200.f, window_height_px - 50.f });

						// spawn the collection and pause buttons
						createPauseButton(renderer, { window_width_px - 80.f, 50.f });
						createCollectionButton(renderer, { window_width_px - 160.f, 50.f });

						for (int i = registry.renderRequests.size() - 1; i >= 0; i--) {
							if (registry.renderRequests.components[i].used_layer == RENDER_LAYER_ID::BG) {
								registry.remove_all_components_of(registry.renderRequests.entities[i]);
							}
						}

						//spawn_game_entities(); 
						is_player_turn = true; 
						break;
					case BUTTON_ACTION_ID::MENU_QUIT: glfwSetWindowShouldClose(window, true); break;
					case BUTTON_ACTION_ID::ACTIONS_ATTACK:
						if (current_game_state != GameStates::ENEMY_TURN) {
							// set player action to attack
							for (Entity p : registry.players.entities) {
								Player& player = registry.players.get(p);
								player.action = PLAYER_ACTION::ATTACKING;

								// hide all action buttons
								for (Entity ab : registry.actionButtons.entities) {
									registry.remove_all_components_of(ab);
								}
								hideGuardButton = true;

								// set game state to attack menu
								set_gamestate(GameStates::ATTACK_MENU);

								// create back button and attack mode text
								createBackButton(renderer, { 100.f , window_height_px - 60.f });
								createAttackModeText(renderer, { window_width_px / 2 , window_height_px - 60.f });
							}
						}
						break;
					case BUTTON_ACTION_ID::ACTIONS_MOVE:
						if (current_game_state != GameStates::ENEMY_TURN) {
							// set player action to move
							for (Entity p : registry.players.entities) {
								Player& player = registry.players.get(p);
								Stats stats = registry.stats.get(p);
								player.action = PLAYER_ACTION::MOVING;

								// hide all action buttons
								for (Entity ab : registry.actionButtons.entities) {
									registry.remove_all_components_of(ab);
								}
								hideGuardButton = true;

								// show ep range
								Motion motion = registry.motions.get(p);
								create_ep_range(stats.ep, motion.movement_speed, motion.position);

								// set game state to move menu
								set_gamestate(GameStates::MOVEMENT_MENU);

								// create back button and move mode text
								createBackButton(renderer, { 100.f , window_height_px - 60.f });
								createMoveModeText(renderer, { window_width_px / 2 , window_height_px - 60.f });
							}
						}
						break;
					case BUTTON_ACTION_ID::PAUSE:
						// TODO: pause enimies if it is their turn
						
						// inMenu = true;
						set_gamestate(GameStates::PAUSE_MENU);
						// render quit button
						createMenuQuit(renderer, { window_width_px / 2, window_height_px / 2 + 90});

						// render cancel button
						createCancelButton(renderer, { window_width_px / 2, window_height_px / 2 - 90.f });
						
						break;
					case BUTTON_ACTION_ID::ACTIONS_CANCEL:
						// inMenu = false;
						set_gamestate(GameStates::BATTLE_MENU);
						break;
					case BUTTON_ACTION_ID::COLLECTION:
						// TODO: add real functionality for this
						logText("Collection Menu to be implemented later!");
						break;
					case BUTTON_ACTION_ID::ACTIONS_BACK:
						// set gamestate back to normal
						set_gamestate(GameStates::BATTLE_MENU);

						break;
					case BUTTON_ACTION_ID::ACTIONS_ITEM:
						// TODO: add real functionality for this
						logText("Items Menu to be implemented later!");

						// hide all action buttons
						for (Entity ab : registry.actionButtons.entities) {
							registry.remove_all_components_of(ab);
						}
						hideGuardButton = true;

						// set game state to move menu
						set_gamestate(GameStates::ITEM_MENU);

						// create back button and move mode text
						createBackButton(renderer, { 100.f , window_height_px - 60.f });
						break;
				}
			}
		}

		///////////////////////////
		// logic for guard button presses
		///////////////////////////
		if (current_game_state != GameStates::ENEMY_TURN) {
			for (Entity e : registry.guardButtons.entities) {
				if (!registry.motions.has(e)) {
					continue;
				}
				Motion m = registry.motions.get(e);
				int buttonX = m.position[0];
				int buttonY = m.position[1];
				// if mouse is interating with a button
				if ((xpos <= (buttonX + m.scale[0] / 2) && xpos >= (buttonX - m.scale[0] / 2)) &&
					(ypos >= (buttonY - m.scale[1] / 2) && ypos <= (buttonY + m.scale[1] / 2))) {
					if (!registry.guardButtons.has(e)) {
						continue;
					}
					// perform action based on button ENUM
					BUTTON_ACTION_ID action = registry.guardButtons.get(e).action;

					switch (action) {
					case BUTTON_ACTION_ID::ACTIONS_GUARD:
						logText("You brace yourself...");
						for (Entity player : registry.players.entities) {
							registry.stats.get(player).guard = true;
							handle_end_player_turn(player);
						}
						break;
					case BUTTON_ACTION_ID::ACTIONS_END_TURN:
						for (Entity player : registry.players.entities) {
							handle_end_player_turn(player);
						}
						break;
					}
				}
			}
		}

		///////////////////////////
		// logic for player actions
		///////////////////////////

		// ensure it is the player's turn and they are not currently moving
		if (get_is_player_turn() && !player_move_click && ypos < window_height_px - 200.f && ypos > 80.f) {
			for (Entity e : registry.players.entities) {
				Player& player = registry.players.get(e);
				Motion& player_motion = registry.motions.get(e);
				Stats& player_stats = registry.stats.get(e);

				switch (current_game_state) {
				case GameStates::ATTACK_MENU:
					// ensure player has clicked on an enemy
					for (Entity en : registry.enemies.entities) {
						// super simple bounding box for now
						Motion m = registry.motions.get(en);
						int enemyX = m.position[0];
						int enemyY = m.position[1];
						
						if ((world_pos.x <= (enemyX + m.scale[0] / 2) && world_pos.x >= (enemyX - m.scale[0] / 2)) &&
							(world_pos.y >= (enemyY - m.scale[1] / 2) && world_pos.y <= (enemyY + m.scale[1] / 2))) {
								
							// only attack if the player hasn't attacked that turn
							if (!player.attacked) {

								// only attack if have enough ep and is close enough
								if (player_stats.ep >= 0.5 * player_stats.maxep && dist_to(player_motion.position, m.position) <= 100.f) {

									// show explosion animation
									createExplosion(renderer, { enemyX, enemyY });

									// play attack sound
									Mix_PlayChannel(-1, fire_explosion_sound, 0);

									logText(deal_damage(e, en, 100.f));

									// wobble the enemy lol
									if (!registry.wobbleTimers.has(en)){
										WobbleTimer& wobble = registry.wobbleTimers.emplace(en);
										wobble.orig_scale = m.scale;
									}
						
									// lower ep
									player_stats.ep -= 0.5 * player_stats.maxep;
									player.attacked = true;
								}
								else if (player_stats.ep < 0.5 * player_stats.maxep) {
									logText("Not enough EP to attack!");
									// play error sound
									Mix_PlayChannel(-1, error_sound, 0);
								}
								else {
									logText("Target too far away!");
									// play error sound
									Mix_PlayChannel(-1, error_sound, 0);
								}
							}
							else {
								logText("You already attacked this turn!");
								// play error sound
								Mix_PlayChannel(-1, error_sound, 0);
							}
						}
					}
					break;
				case GameStates::MOVEMENT_MENU:
					for (Entity& player : registry.players.entities) {
						Motion& motion_struct = registry.motions.get(player);

						// set velocity to the direction of the cursor, at a magnitude of player_velocity
						float speed = motion_struct.movement_speed;
						float angle = atan2(world_pos.y - motion_struct.position.y, world_pos.x - motion_struct.position.x);
						float x_component = cos(angle) * speed;
						float y_component = sin(angle) * speed;
						motion_struct.velocity = { x_component, y_component };
						//motion_struct.angle = angle + (0.5 * M_PI);
						motion_struct.destination = { world_pos.x, world_pos.y };
						motion_struct.in_motion = true;
						player_move_click = true;

					}
					break;
				}

			}
		}
	}


	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE && get_is_player_turn() && !player_move_click) {
		for (Entity& player : registry.players.entities) {
			Motion& motion_struct = registry.motions.get(player);

			// set velocity to the direction of the cursor, at a magnitude of player_velocity
			float speed = motion_struct.movement_speed;
			float angle = atan2(world_pos.y - motion_struct.position.y, world_pos.x - motion_struct.position.x);
			float x_component = cos(angle) * speed;
			float y_component = sin(angle) * speed;
			motion_struct.velocity = { x_component, y_component};
			//motion_struct.angle = angle + (0.5 * M_PI);
			motion_struct.destination = { world_pos.x, world_pos.y };
			motion_struct.in_motion = true;
			player_move_click = true;

		}
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE CHICKEN ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the chicken's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	(vec2)mouse_position; // dummy to avoid compiler warning
}

void WorldSystem::set_is_player_turn(bool val) {
	is_player_turn = val;
}

bool WorldSystem::get_is_player_turn() {
	return is_player_turn;
}

void WorldSystem::set_is_ai_turn(bool val) {
	is_ai_turn = val;
}

bool WorldSystem::get_is_ai_turn() {
	return is_ai_turn;
}

void WorldSystem::start_player_turn() {
	for (Entity player : registry.players.entities) {

		// get player stats
		float& maxep = registry.stats.get(player).maxep;
		float& hp = registry.stats.get(player).hp;
		float& mp = registry.stats.get(player).mp;
		float& ep = registry.stats.get(player).ep;

		if (registry.stats.get(player).guard) {
			//ep = maxep * 1.5f;
			registry.stats.get(player).guard = false;
		}
		else {
			ep = maxep;
		}

	}
}

void WorldSystem::removeForLoad() {
	// remove player for loading
	for (Entity player : registry.players.entities) {
		registry.remove_all_components_of(player);
	}

	// remove enemies
	for (Entity enemy : registry.enemies.entities) {
		registry.remove_all_components_of(enemy);
	}
}

void WorldSystem::removeForNewRoom() {
	// remove enemies
	for (Entity enemy : registry.enemies.entities) {
		registry.remove_all_components_of(enemy);
	}

	// remove solids
	for (Entity solids : registry.solid.entities) {
		registry.remove_all_components_of(solids);
	}

	// remove tileUVs
	for (Entity tileUV : registry.tileUVs.entities) {
		registry.remove_all_components_of(tileUV);
	}

	// remove animations
	for (Entity animation : registry.animations.entities) {
		registry.remove_all_components_of(animation);
	}
}

std::queue<Entity> WorldSystem::loadFromData(json data) {
	// load player
	json entityList = data["entities"];
	std::queue<Entity> entities;
	for (auto entity : entityList) {
		Entity e;
		if (entity["type"] == "player") {
			e = loadPlayer(entity);
		}
		else {
			e = loadEnemy(entity);
		}
		entities.push(e);
	}

	return entities;
}

Entity WorldSystem::loadPlayer(json playerData) {
	// create a player from the save data
	// get player motion
	Motion motion = loadMotion(playerData["motion"]);
	
	// create player
	Entity e = createPlayer(renderer, motion);

	// get player component stuff
	registry.players.get(e).attacked = playerData["player"]["attacked"];

	// get queueable stuff
	registry.queueables.get(e).doing_turn = playerData["queueable"]["doingTurn"];

	// get stats
	json stats = playerData["stats"];
	registry.stats.get(e).ep = stats["ep"];
	registry.stats.get(e).maxep = stats["maxEP"];
	registry.stats.get(e).hp = stats["hp"];
	registry.stats.get(e).maxep = stats["maxHP"];
	registry.stats.get(e).mp = stats["mp"];
	registry.stats.get(e).maxmp = stats["maxMP"];
	registry.stats.get(e).atk = stats["atk"];
	registry.stats.get(e).def = stats["def"];
	registry.stats.get(e).speed = stats["speed"];
	registry.stats.get(e).range = stats["range"];
	registry.stats.get(e).chase = stats["chase"];
	
	return e;
}

Entity WorldSystem::loadEnemy(json enemyData) {
	Entity e;
	if (enemyData["type"] == "slime") {
		e = loadSlime(enemyData);
	}
	return e;
}

Entity WorldSystem::loadSlime(json slimeData) {
	// get slime's motion
	Motion motion = loadMotion(slimeData["motion"]);

	// create slime
	Entity e = createEnemy(renderer, motion);

	// set slimeEnemy data
	json slimeEnemy = slimeData["slime"];
	registry.slimeEnemies.get(e).state = slimeEnemy["state"];

	// get queueable stuff
	json queueable = slimeData["queueable"];
	registry.queueables.get(e).doing_turn = queueable["doingTurn"];

	// get stats
	json stats = slimeData["stats"];
	registry.stats.get(e).ep = stats["ep"];
	registry.stats.get(e).maxep = stats["maxEP"];
	registry.stats.get(e).hp = stats["hp"];
	registry.stats.get(e).maxep = stats["maxHP"];
	registry.stats.get(e).mp = stats["mp"];
	registry.stats.get(e).maxmp = stats["maxMP"];
	registry.stats.get(e).atk = stats["atk"];
	registry.stats.get(e).def = stats["def"];
	registry.stats.get(e).speed = stats["speed"];
	registry.stats.get(e).range = stats["range"];
	registry.stats.get(e).chase = stats["chase"];

	return e;
}

Motion WorldSystem::loadMotion(json motionData) {
	Motion m;
	m.angle = motionData["angle"];
	m.destination = { motionData["destination_x"], motionData["destination_y"] };
	m.in_motion = motionData["in_motion"];
	m.movement_speed = motionData["movement_speed"];
	m.position = { motionData["position_x"], motionData["position_y"] };
	m.velocity = { motionData["velocity_x"], motionData["velocity_y"] };
	return m;
}

void WorldSystem::logText(std::string msg) {
	// (note: if we want to use createText in other applications, we can create a logged text entity)
	// shift existing logged text upwards

	for (Entity e : registry.textTimers.entities) {
		Text& text = registry.texts.get(e);
		text.position[1] -= 50.f;
	}

	// vec2 defaultPos = vec2((2.0f * window_width_px) * (1.f/20.f), (2.0f * window_height_px) * (7.f/10.f));
	vec2 defaultPos = vec2(50.f, (2.0f * window_height_px) * (7.f/10.f));
	vec3 textColor = vec3(1.0f, 1.0f, 1.0f); // white

	Entity e = createText(renderer, defaultPos, msg, 1.5f, textColor);
	registry.textTimers.emplace(e);
}

void WorldSystem::doTurnOrderLogic() {
	Entity currentTurnEntity = turnOrderSystem.getCurrentTurnEntity();

	// if current entity is not doing turn and stopped moving, get the next turn entity
	if (!registry.queueables.get(currentTurnEntity).doing_turn && !registry.motions.get(currentTurnEntity).in_motion) {
		// if player just finished their turn, set is player turn to false
		if (registry.players.has(currentTurnEntity)) {
			set_is_player_turn(false);
		}
		// perform end-of-movement attacks for enemies
		else {
			set_enemy_state_attack(currentTurnEntity);
			aiSystem.step(currentTurnEntity, this, renderer);
		}

		// get next turn
		currentTurnEntity = turnOrderSystem.getNextTurn();

		// if the current entity is the player, call start_player_turn()
		if (registry.players.has(currentTurnEntity)) {
			set_is_player_turn(true);
			start_player_turn();
			logText("It is now your turn!");
			set_gamestate(GameStates::BATTLE_MENU);
		}
	}

	// if current turn entity is enemy and is still doing_turn call ai.step();
	if (!registry.players.has(currentTurnEntity) && registry.queueables.get(currentTurnEntity).doing_turn) {
		aiSystem.step(currentTurnEntity, this, renderer);

		// now that ai did its step, set doing turn to false
		registry.queueables.get(currentTurnEntity).doing_turn = false;
	}
}

// Set attack state for enemies who attack after moving
void set_enemy_state_attack(Entity enemy) {
	if (registry.slimeEnemies.has(enemy)) {
		registry.slimeEnemies.get(enemy).state = ENEMY_STATE::ATTACK;
	}
}

// Set attack state for enemies who attack after moving
void set_gamestate(GameStates state) {
	previous_game_state = current_game_state;
	current_game_state = state;
}