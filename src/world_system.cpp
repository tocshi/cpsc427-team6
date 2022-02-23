// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include<fstream>

#include "physics_system.hpp"

// Game configuration
const size_t MAX_EAGLES = 15;
const size_t MAX_BUG = 5;
const size_t EAGLE_DELAY_MS = 2000 * 3;
const size_t BUG_DELAY_MS = 5000 * 3;


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

// In start menu
bool inMenu;

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
	inMenu = true;

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

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	chicken_dead_sound = Mix_LoadWAV(audio_path("chicken_dead.wav").c_str());
	chicken_eat_sound = Mix_LoadWAV(audio_path("chicken_eat.wav").c_str());

	if (background_music == nullptr || chicken_dead_sound == nullptr || chicken_eat_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("chicken_dead.wav").c_str(),
			audio_path("chicken_eat.wav").c_str());
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
	title_ss << "Points: " << points;
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

	// perform in-motion behaviour
	if (get_is_player_turn() && player_right_click) {
		for (Entity player : registry.players.entities) {
			Motion player_motion = registry.motions.get(player);
			if (player_motion.in_motion) {
				// update the fog of war if the player is moving
				remove_fog_of_war();
				create_fog_of_war(500.f);
				
			}
			else {
				player_right_click = false;
			}
		}
	}

	// if all ai have moved, start player turn
	if (!get_is_player_turn() && get_is_ai_turn()) {
		bool all_moved = true;
		for (Entity ai : registry.slimeEnemies.entities) {
			Motion ai_motion = registry.motions.get(ai);
			if (ai_motion.in_motion) {
				all_moved = false;
			}
		}
		if (all_moved) {
			set_is_ai_turn(false);
			set_is_player_turn(true);

			// perform start-of-turn actions for player
			start_player_turn();
			
		}
		// save the data to the json file
	}

	// If started, remove menu entities, and spawn game entities
	if (!inMenu) {
		// remove all menu entities
		for (Entity e : registry.menuItems.entities) {
			registry.remove_all_components_of(e);
		}
	}

	// Update HP/MP/EP bars and movement
	for (Entity player : registry.players.entities) {
		
		// get player stats
		float& maxEP = registry.players.get(player).maxEP;
		float& hp = registry.players.get(player).hp;
		float& mp = registry.players.get(player).mp;
		float& ep = registry.players.get(player).ep;
		

		// KAITI

		// update player motion
		Motion& player_motion = registry.motions.get(player);
		if (player_motion.in_motion) {
			if (ep <= 0) { 
				player_motion.velocity = { 0.f, 0.f };
				player_motion.in_motion = false; 
				set_is_player_turn(false);
				player_right_click = false;
			}
			else { 
				float ep_rate = 1.f;
				ep -= 2.f * ep_rate; 
				//printf("%s \n", "we are saving data DOES IT WORK AHH CPSC");
				//printf("we are saving data DOES IT WORK AHH CPSC ");
				//printf("%f",ep);
				// UPDATE playerEP
				registry.players.get(player).ep = ep;
				printf("%f", registry.players.get(player).ep);
			}	
		}

		for (Entity entity : registry.motions.entities) {
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
		}
		// Update the camera to follow the player
		Camera& camera = registry.cameras.get(active_camera_entity);
		camera.position = player_motion.position - vec2(window_width_px/2, window_height_px/2);
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

	// !!! TODO A1: update LightUp timers and remove if time drops below zero, similar to the death counter
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
	inMenu = true;

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
}

// spawn the game entities
void WorldSystem::spawn_game_entities() {

	// create all non-menu game objects
	// spawn the player and enemy in random locations
	spawn_player_random_location();
	spawn_enemy_random_location();
  
	createBoss(renderer, { 250.f, 450.f });
	createArtifact(renderer, { 250.f, 550.f });
	createConsumable(renderer, { 250.f, 650.f });
	createEquipable(renderer, { 350.f, 250.f });
	createChest(renderer, { 350.f, 350.f });
	createDoor(renderer, { 350.f, 450.f });
	createSign(renderer, { 350.f, 550.f });
	createStair(renderer, { 350.f, 650.f });
	for (uint i = 0; WALL_BB_WIDTH / 2 + WALL_BB_WIDTH * i < window_width_px; i++) {
		createWall(renderer, { WALL_BB_WIDTH / 2 + WALL_BB_WIDTH * i, WALL_BB_HEIGHT / 2 });
		createWall(renderer, { WALL_BB_WIDTH / 2 + WALL_BB_WIDTH * i, window_height_px - WALL_BB_HEIGHT / 2 });
	}
	for (uint i = 1; WALL_BB_HEIGHT / 2 + WALL_BB_HEIGHT * i < window_height_px - WALL_BB_HEIGHT; i++) {
		createWall(renderer, { WALL_BB_WIDTH / 2, WALL_BB_HEIGHT / 2 + WALL_BB_HEIGHT * i });
		createWall(renderer, { window_width_px - WALL_BB_WIDTH / 2, WALL_BB_HEIGHT / 2 + WALL_BB_HEIGHT * i });
	}
	
	float statbarsX = 150.f;
	float statbarsY = 740.f;
	createHPFill(renderer, { statbarsX, statbarsY });
	createHPBar(renderer,  { statbarsX, statbarsY });
	createMPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createMPBar(renderer,  { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createEPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	createEPBar(renderer,  { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	create_fog_of_war(500.f);
}

// render fog of war around the player past a given radius
void WorldSystem::create_fog_of_war(float radius) {	
	// render fog everywhere except in visible circle around the player
	for (int x = 0; x <= window_width_px; x+=50) {
		for (int y = 0; y <= window_height_px; y += 50) {
			// if the point is not witin the visible circle, render fog there
			for (Entity player : registry.players.entities) {
				// get player position
				Motion player_motion = registry.motions.get(player);
				float playerX = player_motion.position.x;
				float playerY = player_motion.position.y;

				// check if position is within the radius of the players position
				double absX = abs(x - playerX);
				double absY = abs(y - playerY);
				double r = (double)radius;

				// only create fog entities if they are not within the circle
				if ((absX > r || absY > r) || !((absX * absX + absY * absY) <= r * r)) {
					createFog(renderer, { x, y });
				}
			}
		}
	}
}

// make a function that is passing data from the old room states to the new room like Player EP/ HP/ MP stats 
// fog or war states are also passed to new room 
// HP EP MP BAR need to be passed with there current states 
// Float stat bars, walls 
// spwan enemy in random location 
// Door, sign , stair everythin all the placeholders too 
// 1. generate new room , so check if door is open, player not dead so hp!=0 
// check if there is a new room
// goto new room
// carry same player entity / stat


// remove all fog entities
void WorldSystem::remove_fog_of_war() {
	for (Entity e : registry.fog.entities) {
		registry.remove_all_components_of(e);
	}

}

// spawn player entity in random location
void WorldSystem::spawn_player_random_location() {
	printf("%d", rand());
	int randX = rand() % ((window_width_px - 200 + 1) + 200);
	int randY = rand() % ((window_height_px - 200 + 1) + 200);

	if (randX < 200) {
		randX += 200;
	}
	else if (randX >= window_width_px - 200) {
		randX -= 200;
	}

	if (randY < 200) {
		randY += 200;
	}
	else if (randY >= window_height_px - 200) {
		randY -= 200;
	}

	createPlayer(renderer, { (float)randX, (float)randY } );
}

// spawn enemy entity in random location
void WorldSystem::spawn_enemy_random_location() {
	printf("%d", rand());
	int randX = rand()%((window_width_px - 200 + 1) + 200);
	int randY = rand()%((window_height_px - 200 + 1) + 200);

	if (randX < 200) {
		randX += 200;
	}
	else if (randX >= window_width_px - 200) {
		randX -= 200;
	}

	if (randY < 200) {
		randY += 200;
	}
	else if (randY >= window_height_px - 200) {
		randY -= 200;
	}

	createEnemy(renderer, { (float)randX, (float)randY });
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// For now, we are only interested in collisions that involve the player
		if (registry.players.has(entity)) {
			//Player& player = registry.players.get(entity);

			// Checking Player - Deadly collisions
			if (registry.deadlys.has(entity_other)) {
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity)) {
					// Scream, reset timer, and make the chicken sink
					registry.deathTimers.emplace(entity);
					Mix_PlayChannel(-1, chicken_dead_sound, 0);

					// !!! TODO A1: change the chicken orientation and color on death
				}
			}
			// Checking Player - Eatable collisions
			else if (registry.eatables.has(entity_other)) {
				if (!registry.deathTimers.has(entity)) {
					// chew, count points, and set the LightUp timer
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, chicken_eat_sound, 0);
					++points;

					// !!! TODO A1: create a new struct called LightUp in components.hpp and add an instance to the chicken entity by modifying the ECS registry
				}
			}
		}
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

	// SAVING THE GAME
	if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
		save_data();
		printf("SAVING KEY PRESSED");
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
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
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		// Clicking the start button on the menu screen
		for (Entity e : registry.buttons.entities) {
			Motion m = registry.motions.get(e);
			int buttonX = m.position[0];
			int buttonY = m.position[1];
			// if mouse is interating with a button
			if ((xpos <= (buttonX + m.scale[0] / 2) && xpos >= (buttonX - m.scale[0] / 2)) && 
				(ypos >= (buttonY - m.scale[1] / 2) && ypos <= (buttonY + m.scale[1] / 2))) {
				// perform action based on button ENUM
				BUTTON_ACTION_ID action_taken = registry.buttons.get(e).action_taken;

				switch (action_taken) {
					case BUTTON_ACTION_ID::MENU_START: inMenu = false; spawn_game_entities(); is_player_turn = true; break;
					case BUTTON_ACTION_ID::MENU_QUIT: glfwSetWindowShouldClose(window, true); break;
				}
			}
		}
	}


	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE && get_is_player_turn() && !player_right_click) {
		for (Entity& player : registry.players.entities) {
			Motion& motion_struct = registry.motions.get(player);

			// set velocity to the direction of the cursor, at a magnitude of player_velocity
			float speed = motion_struct.movement_speed;
			float angle = atan2(world_pos.y - motion_struct.position.y, world_pos.x - motion_struct.position.x);
			float x_component = cos(angle) * speed;
			float y_component = sin(angle) * speed;
			motion_struct.velocity = { x_component, y_component};
			motion_struct.angle = angle + (0.5 * M_PI);
			motion_struct.destination = { world_pos.x, world_pos.y };
			motion_struct.in_motion = true;
			player_right_click = true;

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
		float& maxEP = registry.players.get(player).maxEP;
		float& hp = registry.players.get(player).hp;
		float& mp = registry.players.get(player).mp;
		float& ep = registry.players.get(player).ep;

		ep = maxEP;

	}
}