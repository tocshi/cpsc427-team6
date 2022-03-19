#include "world_system.hpp"
#include "world_init.hpp"
// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>

#include "physics_system.hpp"
#include "combat_system.hpp"

// Game configuration
// decalre gamestates
//GameStates game_state = GameStates::CUTSCENE;

// Create the world
WorldSystem::WorldSystem()
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

void WorldSystem::destroyMusic() {

	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (fire_explosion_sound != nullptr)
		Mix_FreeChunk(fire_explosion_sound);
	if (error_sound != nullptr)
		Mix_FreeChunk(error_sound);
	if (footstep_sound != nullptr)
		Mix_FreeChunk(footstep_sound);
	if (menu_music != nullptr)
		Mix_FreeMusic(menu_music);
	if (cutscene_music != nullptr)
		Mix_FreeMusic(cutscene_music);
	Mix_CloseAudio();

}

WorldSystem::~WorldSystem() {
	// Destroy music components
	destroyMusic();
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

// In start menu (CHANGE TO INT TO SEE IF IT WORKS)
// CUTSCENE IS null
GameStates current_game_state;
GameStates previous_game_state = current_game_state;


// fog stats
float fog_radius = 300.f;
float fog_resolution = 2000.f;

// ep range stats
float ep_resolution = 2000.f;

// move audio timer
float move_audio_timer_ms = 200.f;

// button toggles
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
	//current_game_state = GameStates::CUTSCENE;

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
	menu_music = Mix_LoadMUS(audio_path("bgm/menu0.wav").c_str());
	cutscene_music= Mix_LoadMUS(audio_path("bgm/dream0.wav").c_str());



	// Sounds and volumes
	fire_explosion_sound = Mix_LoadWAV(audio_path("feedback/fire_explosion.wav").c_str());
	Mix_VolumeChunk(fire_explosion_sound, 13);
	error_sound = Mix_LoadWAV(audio_path("feedback/error.wav").c_str());
	Mix_VolumeChunk(error_sound, 13);
	footstep_sound = Mix_LoadWAV(audio_path("feedback/footstep.wav").c_str());
	Mix_VolumeChunk(footstep_sound, 14);

	if (background_music == nullptr || fire_explosion_sound == nullptr 
		|| error_sound == nullptr || footstep_sound == nullptr|| menu_music == nullptr || cutscene_music == nullptr ) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("bgm/caves0.wav").c_str(),
			audio_path("bgm/menu0.wav").c_str(), //add
			audio_path("bgm/dream0.wav").c_str(), //add
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

	Mix_PlayMusic(cutscene_music, 1);
	fprintf(stderr, "Loaded music\n");
	printf("%d", countCutScene);
	//set_gamestate(GameStates::CUTSCENE);
	// call custscene func
	cut_scene_start();
	// Set all states to default  

   // restart_game();
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
		update_turn_ui();
		doTurnOrderLogic();
	}

	double mouseXpos, mouseYpos;
	//getting cursor position
	glfwGetCursorPos(window, &mouseXpos, &mouseYpos);
	//printf("Cursor Position at (%f, %f)\n", xpos, ypos);

	// remove previous stylized pointer
	for (Entity pointer : registry.pointers.entities) {
		registry.remove_all_components_of(pointer);
	}
	// render stylized pointers
	if (mouseYpos > window_height_px - 200.f || mouseYpos < 100.f) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		createPointer(renderer, vec2(mouseXpos + POINTER_BB_WIDTH / 2, mouseYpos + POINTER_BB_HEIGHT / 2), TEXTURE_ASSET_ID::NORMAL_POINTER);
	}
	else if (current_game_state == GameStates::MOVEMENT_MENU) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		createPointer(renderer, vec2(mouseXpos, mouseYpos - POINTER_BB_HEIGHT / 2), TEXTURE_ASSET_ID::MOVE_POINTER);
	}
	else if (current_game_state == GameStates::ATTACK_MENU) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		createPointer(renderer, vec2(mouseXpos + POINTER_BB_WIDTH / 2, mouseYpos + POINTER_BB_HEIGHT / 2), TEXTURE_ASSET_ID::ATTACK_POINTER);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		createPointer(renderer, vec2(mouseXpos + POINTER_BB_WIDTH / 2, mouseYpos + POINTER_BB_HEIGHT / 2), TEXTURE_ASSET_ID::NORMAL_POINTER);
	}

	// perform in-motion behaviour
	if (get_is_player_turn() && player_move_click) {
		Motion player_motion = registry.motions.get(player_main);
		Stats stats = registry.stats.get(player_main);
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
			if (current_game_state == GameStates::MOVEMENT_MENU) {
				create_ep_range(stats.ep, player_motion.movement_speed, player_motion.position);
			}
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

	// If started, remove menu entities, and spawn game entities
	//if(!current_game_state) (
	//current_game_state > GameStates::CUTSCENE || current_game_state <GameStates::SPLASH_SCREEN
	if (current_game_state == GameStates::GAME_START || current_game_state == GameStates::BATTLE_MENU) {
		// remove all menu entities
		for (Entity e : registry.menuItems.entities) {
			registry.remove_all_components_of(e);
		}

		if (registry.actionButtons.entities.size() < 4) {
			// bring back all of the buttons
			createMoveButton(renderer, { window_width_px - 1400.f, window_height_px - 50.f });
			createAttackButton(renderer, { window_width_px - 1000.f, window_height_px - 50.f });
			createGuardButton(renderer, { window_width_px - 600.f, window_height_px - 50.f }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD);
			createItemButton(renderer, { window_width_px - 200.f, window_height_px - 50.f });
		}
		
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
		float& hp = registry.stats.get(player).hp;
		float& mp = registry.stats.get(player).mp;
		float& ep = registry.stats.get(player).ep;
		float& maxhp = registry.stats.get(player).maxhp;
		float& maxmp = registry.stats.get(player).maxmp;
		float& maxep = registry.stats.get(player).maxep;

		// Check if player has died
		if (hp <= 0 && !registry.deathTimers.has(player)) {
			registry.deathTimers.emplace(player);
			logText("You have died!");
			player_move_click = false;
			break;
		}

		// change guard button to end turn if ep is not full
		if ((p.attacked || p.moved) && !hideGuardButton) {
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
		Stats& player_stats = registry.stats.get(player);
		if (player_motion.in_motion) {
			if (ep <= 0) {
				player_motion.destination = player_motion.position;
				set_gamestate(GameStates::BATTLE_MENU);
			}
			else { 
				ep -= 0.06f * player_stats.epratemove * elapsed_ms_since_last_update; 
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
				motion_struct.scale = { min(STAT_BB_WIDTH, (hp / maxhp) * STAT_BB_WIDTH), STAT_BB_HEIGHT };
				motion_struct.position[0] = 150.f - 150.f*(1.f - min(1.f, (hp / maxhp)));	// original pos (full bar) - (1-multiplier)
				break;
			case TEXTURE_ASSET_ID::MPFILL:
				motion_struct.scale = { min(STAT_BB_WIDTH, (mp / maxmp) * STAT_BB_WIDTH), STAT_BB_HEIGHT };
				motion_struct.position[0] = 150.f - 150.f*(1.f - min(1.f, (mp / maxmp)));	// original pos (full bar) - (1-multiplier)
				break;
			case TEXTURE_ASSET_ID::EPFILL:
				motion_struct.scale = { min(STAT_BB_WIDTH, (ep / maxep) * STAT_BB_WIDTH), STAT_BB_HEIGHT };
				motion_struct.position[0] = 150.f - 150.f*(1.f - min(1.f, (ep / maxep)));	// original pos (full bar) - (1-multiplier)
				break;
			}

			// Hide certain entities that are outside of the player's sight range
			// don't hide walls, signs, stairs, doors
			if (!registry.hidables.has(entity))
				continue;

			float distance_to_player = 
				sqrt(pow((motion_struct.position.x - player_motion.position.x), 2) 
				+ pow((motion_struct.position.y - player_motion.position.y), 2));
			
			if (distance_to_player > registry.stats.get(player_main).range) {
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

			// remove from turn queue
			turnOrderSystem.removeFromQueue(enemy);
		}
	}

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

	// Projectile Timers
	for (Entity entity : registry.projectileTimers.entities) {
		// progress timer
		ProjectileTimer& counter = registry.projectileTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if(counter.counter_ms < min_counter_ms){
		    min_counter_ms = counter.counter_ms;
		}

		// remove text once the text timer has expired
		if (counter.counter_ms < 0) {
			registry.motions.get(counter.owner).in_motion = false;
			registry.remove_all_components_of(entity);
		}
	}

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
	for (Entity e : registry.animations.entities) {
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

	// update timed log text from signs
	for (Entity e : registry.signs.entities) {
		Sign& sign = registry.signs.get(e);
		if (sign.playing) {
			sign.counter_ms += elapsed_ms_since_last_update;
			for (int i = sign.next_message; i < sign.messages.size(); i++) {
				if (sign.counter_ms < sign.messages[i].second) {
					sign.next_message = i;
					break;
				}
				logText(sign.messages[i].first);
				sign.next_message = i;
			}
			if (sign.counter_ms > sign.messages[sign.messages.size()-1].second) {
				sign.counter_ms = 0;
				sign.next_message = 0;
				sign.playing = false;
			}
		}
	}

	return true;
}

// cutscene at the start of the game  --- -
void WorldSystem::cut_scene_start() {

	//create entity for background pic
	// set game state to cutscene 
	player_move_click = false;
	
	registry.list_all_components();
	printf("CUT SCENE STARTING \n");

	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	while (registry.texts.entities.size() > 0)
		registry.remove_all_components_of(registry.texts.entities.back());

	while (registry.cameras.entities.size() > 0)
		registry.remove_all_components_of(registry.cameras.entities.back());

	// Add a camera entity
	active_camera_entity = createCamera({ 0, 0 });

	registry.list_all_components();

	set_gamestate(GameStates::CUTSCENE);
	//create cut scene 

	// check when the left mouse is clicked move to next picture 

	// on left click change scene to new one (x2)
	// checks how many times left click was one with countCutScene & makes sure the game state is CutScene 
	if (current_game_state == GameStates::CUTSCENE && countCutScene == 0) {
			createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
			printf("%d the cutscene 1 and cutscene count is \n", countCutScene);
			printf("Cut Scene\n");
	}
	if (current_game_state == GameStates::CUTSCENE && countCutScene == 1) {
			createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE2);
			printf("cutScene 2\n");
			printf("%d the cutscene 2 and cutscene count is \n", countCutScene);
	}
	
	if (current_game_state == GameStates::CUTSCENE && countCutScene == 2) {
			createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE3);
			printf("cutScene 3 \n");
			printf("%d the cutscene 3 and cutscene count is \n", countCutScene);
	}

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

	// restart the game on the menu screen
	//current_game_state = true;
	set_gamestate(GameStates::MAIN_MENU);

	if (current_game_state == GameStates::MAIN_MENU) {
		Mix_PlayMusic(menu_music, 1);
	}

	/*if (current_game_state != GameStates::MAIN_MENU) {
		//current_game_state = GameStates::MAIN_MENU;
		std::cout << "ACTION: RESTART THE GAME ON THE MENU SCREEN : Game state = MAIN_MENU" << std::endl;
		//printf("ACTION: RESTART THE GAME ON THE MENU SCREEN : Game state = MAIN_MENU");
	}*/
	//current_game_state = GameStates::MAIN_MENU;
	//printf("ACTION: RESTART THE GAME ON THE MENU SCREEN : Game state = MAIN_MENU");

	createMenuStart(renderer, { window_width_px / 2, 500.f });
	createMenuQuit(renderer, { window_width_px / 2, 850.f });
	createMenuTitle(renderer, { window_width_px / 2, 200.f });
}

void WorldSystem::handle_end_player_turn(Entity player) {
	Motion& player_motion = registry.motions.get(player);
	Player& p = registry.players.get(player);
	player_motion.velocity = { 0.f, 0.f };
	player_motion.in_motion = false;
	p.attacked = false;
	p.moved = false;

	set_is_player_turn(false);
	player_move_click = false;
	logText("It is now the enemies' turn!");
	// set player's doing_turn to false
	registry.queueables.get(player).doing_turn = false;
	set_gamestate(GameStates::ENEMY_TURN);
}

// spawn the game entities
void WorldSystem::spawn_game_entities() {

	// Switch between debug and regular room
	std::string next_map = roomSystem.getRandomRoom(Floors::FLOOR1, true);
	//std::string next_map = roomSystem.getRandomRoom(Floors::DEBUG, true);
	SpawnData spawnData = createTiles(renderer, next_map);

	// create all non-menu game objects
	// spawn the player and enemy in random locations
	spawn_player_random_location(spawnData.playerSpawns);
	spawn_enemies_random_location(spawnData.enemySpawns, spawnData.minEnemies, spawnData.maxEnemies);
	spawn_items_random_location(spawnData.itemSpawns, spawnData.minItems, spawnData.maxItems);
  
	Entity player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);

	std::vector<std::pair<std::string, int>> messages = {
		{"Welcome to Adrift in Somnium!", 0},
		{"Left click the buttons at the bottom to switch between actions.", 2000},
		{"In Move mode, you can click to move as long as you have EP.", 6000},
		{"EP is the yellow bar at the top of the screen, which gets expended as you move and attack.", 10000},
		{"In Attack mode, you can click on an enemy close to you to deal damage at the cost of half your EP.", 15000},
		{"Use your attacks wisely. You can only attack once per turn.", 20000},
		{"After your EP hits 0 or you click on End Turn, the enemies will have a turn to move and attack you.", 24000},
		{"Good luck, nameless adventurer.", 30000}};

	createSign(
		renderer, 
		{ player_motion.position.x - 64, player_motion.position.y - 64 },
		messages);

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
	turnUI = createTurnUI(renderer, { window_width_px*(3.f/4.f), window_height_px*(1.f/16.f)});
}

// render ep range around the given position
void WorldSystem::create_ep_range(float remaining_ep, float speed, vec2 pos) {
	float ep_radius = remaining_ep * speed * 0.015 + ((110.f * remaining_ep) / 100);

	Entity ep = createEpRange({ pos.x , pos.y }, ep_resolution, ep_radius, { window_width_px, window_height_px });
	registry.colors.insert(ep, { 0.2, 0.2, 8.7 });
}

// render fog of war around the player
void WorldSystem::create_fog_of_war() {	
		// get player position
	Motion player_motion = registry.motions.get(player_main);
	Stats player_stats = registry.stats.get(player_main);
	float playerX = player_motion.position.x;
	float playerY = player_motion.position.y;

	Entity fog = createFog({ playerX, playerY }, fog_resolution, player_stats.range, { window_width_px, window_height_px });
	registry.colors.insert(fog, { 0.2, 0.2, 0.2 });
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
		player_main = createPlayer(renderer, { playerSpawns[0].x, playerSpawns[0].y});
		return;
	}
	// default spawn location in case we don't have player spawns
	player_main = createPlayer(renderer, { 0, 0 });

}

// spawn enemy entities in random locations
void WorldSystem::spawn_enemies_random_location(std::vector<vec2>& enemySpawns, int min, int max) {
	std::random_shuffle(enemySpawns.begin(), enemySpawns.end());
	if (enemySpawns.size() > 0) {
		int numberToSpawn = std::min(irandRange(min, max + 1), int(enemySpawns.size()));
		for (int i = 0; i < numberToSpawn; i++) {
			// Spawn either a slime or PlantShooter or caveling
			int roll = irand(4);
			if (roll < 1) {
				createCaveling(renderer, { enemySpawns[i].x, enemySpawns[i].y });
			}
			else if (roll < 2) {
				createPlantShooter(renderer, { enemySpawns[i].x, enemySpawns[i].y });
			}
			else {
				createEnemy(renderer, { enemySpawns[i].x, enemySpawns[i].y });
			}
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
			float roll = irand(100);
			if (roll < 30) {
				createChest(renderer, { itemSpawns[i].x, itemSpawns[i].y }, false);
			}
			else {
				createChest(renderer, { itemSpawns[i].x, itemSpawns[i].y }, true);
			}
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
	// DEBUG: HEAL PLAYER
	if (action == GLFW_RELEASE && key == GLFW_KEY_EQUAL) {
		Stats& stat = registry.stats.get(player_main);
		stat.hp = stat.maxhp;
	}


	if (action == GLFW_PRESS && key == GLFW_KEY_P) {
		auto& stats = registry.stats.get(player_main);
		printf("\nPLAYER STATS:\natk: %f\ndef: %f\nspeed: %f\nhp: %f\nmp: %f\n", stats.atk, stats.def, stats.speed, stats.maxhp, stats.maxmp);
	}

	// Esc to go to mainMenu only if previous game state is cutscene
	if (action == GLFW_PRESS && key== GLFW_KEY_ESCAPE && current_game_state == GameStates::CUTSCENE) {
		int w, h;
		//previous_game_state = GameStates::CUTSCENE;
		//logText("Escape Key Pressed");
		//set_gamestate(GameStates::MAIN_MENU);
		glfwGetWindowSize(window, &w, &h);
		restart_game();
		printf("\n escaped pressed \n");
	}

	// SAVING THE GAME
	if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
		saveSystem.saveGameState(turnOrderSystem.getTurnOrder());
		logText("Game state saved!");
	}

	// DEBUG: Testing artifact/stacking
	if (action == GLFW_RELEASE && key == GLFW_KEY_1) {
		int give = (int)ARTIFACT::KB_MALLET;
		for (Entity& p : registry.players.entities) {
			Inventory& inv = registry.inventories.get(p);
			inv.artifact[give]++;

			std::string name = artifact_names.at((ARTIFACT)give);
			std::cout << "Artifact given: " << name << " (" << inv.artifact[give] << ")" << std::endl;
		}
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_2) {
		int give = (int)ARTIFACT::WINDBAG;
		for (Entity& p : registry.players.entities) {
			Inventory& inv = registry.inventories.get(p);
			inv.artifact[give]++;

			std::string name = artifact_names.at((ARTIFACT)give);
			std::cout << "Artifact given: " << name << " (" << inv.artifact[give] << ")" << std::endl;
		}
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_Q) {
		for (Entity& p : registry.players.entities) {
			Motion m = registry.motions.get(p);
			Player player = registry.players.get(p);
			Equipment equip = createEquipment(EQUIPMENT::SHARP, player.floor);
			createEquipmentEntity(renderer, m.position, equip);
			printf("atk: %f\ndef: %f\nspeed: %f\nhp: %f\nmp: %f\n", equip.atk, equip.def, equip.speed, equip.hp, equip.mp);
			for (ATTACK a : equip.attacks) {
				std::cout << "Attack: " << (int)a << std::endl;
			}
		}
	}

	// LOADING THE GAME
	if (action == GLFW_RELEASE && key == GLFW_KEY_L && get_is_player_turn() ) {
		// if save data exists reset the game
		if (saveSystem.saveDataExists()) {
			// remove entities to load in entities
			removeForLoad();
			printf("Removed for load\n");
			// get saved game data
			json gameData = saveSystem.getSaveData();
			printf("getting gameData\n");
			// load the entities in
			loadFromData(gameData);
			printf("load game data?\n");
		}

		logText("Game state loaded!");
		remove_fog_of_war();
		create_fog_of_war();
	}

	// simulating a new room
	if (action == GLFW_RELEASE && key == GLFW_KEY_N && get_is_player_turn()) {
		// save game (should be just player stuff)
		json playerData = saveSystem.jsonifyPlayer(player_main);
		// remove all entities for new room
		removeForNewRoom();
		// remove player
		registry.remove_all_components_of(player_main);
		// make new map
		std::string next_map = roomSystem.getRandomRoom(roomSystem.current_floor, false);
		SpawnData spawnData = createTiles(renderer, next_map);
		// load the player back
		player_main = loadPlayer(playerData);
		// get the player and set its position
		std::random_shuffle(spawnData.playerSpawns.begin(), spawnData.playerSpawns.end());
		Motion& motion = registry.motions.get(player_main);
		Stats& stats = registry.stats.get(player_main);
		// set random position
		motion.position = { spawnData.playerSpawns[0].x, spawnData.playerSpawns[0].y };
		// set everything else in motion to default
		motion.angle = 0.f;
		motion.velocity = { 0.f, 0.f };
		motion.in_motion = false;
		motion.movement_speed = 400;
		motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });

		// Refill Player EP
		stats.ep = stats.maxep;

		spawn_enemies_random_location(spawnData.enemySpawns, spawnData.minEnemies, spawnData.maxEnemies);
		spawn_items_random_location(spawnData.itemSpawns, spawnData.minItems, spawnData.maxItems);

		remove_fog_of_war();
		create_fog_of_war();

		// setup turn order system
		turnOrderSystem.setUpTurnOrder();
		// start first turn
		turnOrderSystem.getNextTurn();

		saveSystem.saveGameState(turnOrderSystem.getTurnOrder());
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
	}

	// Resetting game // LOGGING TEXT TEST
	if (action == GLFW_RELEASE && key == GLFW_KEY_T) {
		printf("GAME STATE LOG START ============\n");
		printf("Previous game state is: %i\n", static_cast<int>(previous_game_state));
		printf("Current game state is: %i\n", static_cast<int>(current_game_state));
		printf("GAME STATE LOG END ============\n\n\n");
		turnOrderSystem.getTurnOrder();
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
	//printf("World Position at (%f, %f)\n", world_pos.x, world_pos.y)

		// check if left click works 
		
	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT && !player_move_click && current_game_state == GameStates::CUTSCENE) {
		countCutScene++;
		printf("start cutscenecount is %d \n", countCutScene);
		
		cut_scene_start();
		if (current_game_state == GameStates::CUTSCENE && countCutScene == 3) {
			set_gamestate(GameStates::MAIN_MENU);
			printf("set to main_menu game state \n");
			restart_game();
		}
		else {
			printf(" \n you just pressed left mouse button\n");
		}
	}

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
						if (current_game_state != GameStates::CUTSCENE || current_game_state != GameStates::MAIN_MENU) {
							Mix_PlayMusic(background_music, -1);
						}
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
						is_player_turn = true; 
						break;
					case BUTTON_ACTION_ID::MENU_QUIT: glfwSetWindowShouldClose(window, true); break;
					case BUTTON_ACTION_ID::ACTIONS_ATTACK:
						if (current_game_state != GameStates::ENEMY_TURN) {
							// set player action to attack
							Player& player = registry.players.get(player_main);
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
						break;
					case BUTTON_ACTION_ID::ACTIONS_MOVE:
						if (registry.stats.get(player_main).ep <= 0) {
							logText("Cannot move with 0 EP!");
							Mix_PlayChannel(-1, error_sound, 0);
							break;
						}
						if (current_game_state != GameStates::ENEMY_TURN) {
							// set player action to move
							Player& player = registry.players.get(player_main);
							Stats stats = registry.stats.get(player_main);
							player.action = PLAYER_ACTION::MOVING;

							// hide all action buttons
							for (Entity ab : registry.actionButtons.entities) {
								registry.remove_all_components_of(ab);
							}
							hideGuardButton = true;

							// show ep range
							Motion motion = registry.motions.get(player_main);
							create_ep_range(stats.ep, motion.movement_speed, motion.position);

							// set game state to move menu
							set_gamestate(GameStates::MOVEMENT_MENU);

							// create back button and move mode text
							createBackButton(renderer, { 100.f , window_height_px - 60.f });
							createMoveModeText(renderer, { window_width_px / 2 , window_height_px - 60.f });
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
						// if the button is pressed again while the menu is already open, close the menu
						if (current_game_state == GameStates::COLLECTION_MENU) {
							set_gamestate(GameStates::BATTLE_MENU);
						}
						else {
							// render the collection menu
							createCollectionMenu(renderer, vec2(window_width_px / 2, window_height_px / 2 - 40.f));
							set_gamestate(GameStates::COLLECTION_MENU);
						}
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
					case BUTTON_ACTION_ID::OPEN_DIALOG:
						// remove all other description dialog components
						for (Entity dd : registry.descriptionDialogs.entities) {
							registry.remove_all_components_of(dd);
						}

						// get which icon was clicked
						if (registry.artifactIcons.has(e)) {
							ARTIFACT artifact = registry.artifactIcons.get(e).artifact;
							createDescriptionDialog(renderer, vec2(window_width_px / 2, window_height_px / 2 - 50.f), artifact);
						}
						break;
					case BUTTON_ACTION_ID::CLOSE_DIALOG:
						// remove all description dialog components
						for (Entity dd : registry.descriptionDialogs.entities) {
							registry.remove_all_components_of(dd);
						}
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
						registry.stats.get(player_main).guard = true;
						handle_end_player_turn(player_main);
						break;
					case BUTTON_ACTION_ID::ACTIONS_END_TURN:
						handle_end_player_turn(player_main);
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
			if (player_main && current_game_state >= GameStates::GAME_START && current_game_state != GameStates::CUTSCENE) {
				Player& player = registry.players.get(player_main);
				Motion& player_motion = registry.motions.get(player_main);
				Stats& player_stats = registry.stats.get(player_main);

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
								if (player_stats.ep >= 50 && dist_to(player_motion.position, m.position) <= 100.f) {

									// show explosion animation
									createExplosion(renderer, { enemyX, enemyY });

									// play attack sound
									Mix_PlayChannel(-1, fire_explosion_sound, 0);

									logText(deal_damage(player_main, en, 100.f));

									// wobble the enemy lol
									if (!registry.wobbleTimers.has(en)){
										WobbleTimer& wobble = registry.wobbleTimers.emplace(en);
										wobble.orig_scale = m.scale;
									}
						
									// lower ep
									player_stats.ep -= 50;
									player.attacked = true;
								}
								else if (player_stats.ep < 50) {
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
					if (player_main) {
						Motion& motion_struct = registry.motions.get(player_main);

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
						player.moved = true;
					}
					break;
				default:
					for (Entity& entity : registry.interactables.entities) {
						Motion& motion = registry.motions.get(entity);
						Interactable& interactable = registry.interactables.get(entity);
						if (world_pos.x <= motion.position.x + abs(motion.scale.x / 2) &&
							world_pos.x >= motion.position.x - abs(motion.scale.x / 2) &&
							world_pos.y <= motion.position.y + abs(motion.scale.y / 2) &&
							world_pos.y >= motion.position.y - abs(motion.scale.y / 2)) {

							// Sign behaviour
							if (registry.signs.has(entity)) {
								Sign& sign = registry.signs.get(entity);
								sign.playing = true;
							}
							// Chest behaviour
							else if (interactable.type == INTERACT_TYPE::ARTIFACT_CHEST && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								
								// use gacha system to determine loot
								int pity = registry.players.get(player_main).gacha_pity;
								float chance_T4 = 5.f + 1 * pity;
								float chance_T3 = chance_T4 + 15.f + 0.75 * pity;
								float chance_T2 = chance_T3 + 30.f + 0.5 * pity;
								float roll = rand() % 100;
								int loot = 0;

								// need sizeof() for array element size (yes I know it's 4 but I would like to generalize just in case)
								if (roll < chance_T4) { 
									loot = artifact_T4[irand(sizeof(artifact_T4) / sizeof(artifact_T4[0]))];
									registry.players.get(player_main).gacha_pity = 0;
								}
								else if (roll < chance_T3) { 
									loot = artifact_T3[irand(sizeof(artifact_T3) / sizeof(artifact_T3[0]))];
									registry.players.get(player_main).gacha_pity++;
								}
								else if (roll < chance_T2) { 
									loot = artifact_T2[irand(sizeof(artifact_T2) / sizeof(artifact_T2[0]))];
									registry.players.get(player_main).gacha_pity++;
								}
								else                       { 
									loot = artifact_T1[irand(sizeof(artifact_T1) / sizeof(artifact_T1[0]))];
									registry.players.get(player_main).gacha_pity++;
								}
								
								createArtifact(renderer, motion.position, (ARTIFACT)loot);

								std::string name = artifact_names.at((ARTIFACT)loot);
								logText("You open the chest and find " + name + "!");
								
								// TODO: make a more graceful chest destruction kthx
								registry.remove_all_components_of(entity);
								break;
							}
							else if (interactable.type == INTERACT_TYPE::ITEM_CHEST && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								Player player = registry.players.get(player_main);
								EQUIPMENT type;

								// choose equipment
								if (ichoose(0, 1)) {
									type = EQUIPMENT::SHARP;
								}
								else {
									type = EQUIPMENT::ARMOUR;
								}

								Equipment equip = createEquipment(type, player.floor);
								createEquipmentEntity(renderer, motion.position, equip);

								logText("You open the chest and find some equipment!");

								// TODO: make a more graceful chest destruction kthx
								registry.remove_all_components_of(entity);
								break;
							}
							// Pickup item behaviour
							else if (interactable.type == INTERACT_TYPE::PICKUP && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								Inventory& inv = registry.inventories.get(player_main);
								Motion& player_motion = registry.motions.get(player_main);
								if (registry.artifacts.has(entity)) {
									ARTIFACT artifact = registry.artifacts.get(entity).type;
									inv.artifact[(int)artifact]++;
								}
								if (registry.equipment.has(entity)) {
									Equipment equipment = registry.equipment.get(entity);
									Equipment prev = equip_item(player_main, equipment);
									createEquipmentEntity(renderer, player_motion.position, prev);
								}
								registry.remove_all_components_of(entity);
								break;
							}
						}
					}
				}
			}
		}
	}


	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE && get_is_player_turn() && !player_move_click && current_game_state >= GameStates::GAME_START && current_game_state != GameStates::CUTSCENE) {
		Motion& motion_struct = registry.motions.get(player_main);

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

void WorldSystem::on_mouse_move(vec2 mouse_position) {
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
	// get player stats
	float& maxep = registry.stats.get(player_main).maxep;
	float& ep = registry.stats.get(player_main).ep;

	if (registry.stats.get(player_main).guard) {
		ep = maxep * 1.5f;
		registry.stats.get(player_main).guard = false;
	}
	else {
		ep = maxep;
	}

	// reset enemies' hit_by_player status
	for (Entity& enemy : registry.enemies.entities) {
		auto& enemy_struct = registry.enemies.get(enemy);

		enemy_struct.hit_by_player = false;
	}
}

void WorldSystem::removeForLoad() {
	// empty queue
	turnOrderSystem.emptyQueue();

	// remove player for loading
	registry.remove_all_components_of(player_main);

	// remove enemies
	for (Entity enemy : registry.enemies.entities) {
		registry.remove_all_components_of(enemy);
	}

	
	// remove collidables
	for (Entity collidable : registry.collidables.entities) {
		registry.remove_all_components_of(collidable);
	}

	// remove interactables
	for (Entity interactable : registry.interactables.entities) {
		registry.remove_all_components_of(interactable);
	}

	// remove tiles
	for (Entity tileUV : registry.tileUVs.entities) {
		registry.remove_all_components_of(tileUV);
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

void WorldSystem::loadFromData(json data) {
	// load player
	json entityList = data["entities"];
	json collidablesList = data["map"]["collidables"];
	json interactablesList = data["map"]["interactables"];
	json tilesList = data["map"]["tiles"];

	// load enemies
	std::queue<Entity> entities;
	for (auto& entity : entityList) {
		Entity e;
		if (entity["type"] == "player") {
			printf("type is player successful... loading player\n");
			e = loadPlayer(entity);
			player_main = e;
		}
		else {
			printf(" type is enemy ... loading enemy\n");
			e = loadEnemy(entity);
			printf("loading enemy done\n ");
		}
		entities.push(e);
	}
	// put entities into turn order system
	turnOrderSystem.loadTurnOrder(entities);
	// load collidables
	loadCollidables(collidablesList);
	// load interactables
	loadInteractables(interactablesList);
	// load tiles
	loadTiles(tilesList);
}

Entity WorldSystem::loadPlayer(json playerData) {
	// create a player from the save data
	// create player
	Entity e = createPlayer(renderer, { 0, 0 });
	player_main = e;

	// load motion
	loadMotion(e, playerData["motion"]);

	// get queueable stuff
	loadQueueable(e, playerData["queueable"]);

	// load stats
	loadStats(e, playerData["stats"]);

	// get inventory
	Inventory inv = loadInventory(e, playerData["inventory"]);

	// get player component stuff
	loadPlayerComponent(e, playerData["player"], inv);

	// load player statuses
	loadStatuses(e, playerData["statuses"]);
	
	return e;
}

Entity WorldSystem::loadEnemy(json enemyData) {
	Entity e;
	if (enemyData["enemy"]["type"] == ENEMY_TYPE::SLIME) {
		e = createEnemy(renderer, { 0, 0 });
	}
	else if (enemyData["enemy"]["type"] == ENEMY_TYPE::PLANT_SHOOTER) {
		e = createPlantShooter(renderer, { 0, 0 });
	}
	else if (enemyData["enemy"]["type"] == ENEMY_TYPE::CAVELING) {
		e = createCaveling(renderer, { 0, 0 });
	}
	else if (enemyData["enemy"]["type"] == ENEMY_TYPE::KING_SLIME) {
		e = createBoss(renderer, { 0, 0 });
	}
	// load motion
	loadMotion(e, enemyData["motion"]);
	// load queueable
	loadQueueable(e, enemyData["queueable"]);
	// load stats
	loadStats(e, enemyData["stats"]);
	// load inventory
	Inventory inv = loadInventory(e, enemyData["inventory"]);
	// load enemy component
	loadEnemyComponent(e, enemyData["enemy"], inv);
	// load enemy statuses
	loadStatuses(e, enemyData["statuses"]);
	return e;
}

void WorldSystem::loadMotion(Entity e, json motionData) {
	Motion& m = registry.motions.get(e);
	m.angle = motionData["angle"];
	m.destination = { motionData["destination_x"], motionData["destination_y"] };
	m.in_motion = motionData["in_motion"];
	m.movement_speed = motionData["movement_speed"];
	m.position = { motionData["position_x"], motionData["position_y"] };
	m.velocity = { motionData["velocity_x"], motionData["velocity_y"] };
	m.scale = { motionData["scale"]["x"], motionData["scale"]["y"] };
}

void WorldSystem::loadStats(Entity e, json stats) {
	registry.stats.get(e).hp = stats["hp"];
	registry.stats.get(e).maxep = stats["maxHP"];

	registry.stats.get(e).mp = stats["mp"];
	registry.stats.get(e).maxmp = stats["maxMP"];
	registry.stats.get(e).mpregen = stats["mpregen"];

	registry.stats.get(e).ep = stats["ep"];
	registry.stats.get(e).maxep = stats["maxEP"];

	registry.stats.get(e).epratemove = stats["epratemove"];
	registry.stats.get(e).eprateatk = stats["eprateatk"];
	
	registry.stats.get(e).atk = stats["atk"];
	registry.stats.get(e).def = stats["def"];
	registry.stats.get(e).speed = stats["speed"];
	registry.stats.get(e).range = stats["range"];
	registry.stats.get(e).chase = stats["chase"];

	registry.stats.get(e).guard = stats["guard"];
}

void WorldSystem::loadQueueable(Entity e, json queueableData) {
	registry.queueables.get(e).doing_turn = queueableData["doingTurn"];
}

void WorldSystem::loadEnemyComponent(Entity e, json enemyCompData, Inventory inv) {
	registry.enemies.get(e).hit_by_player = enemyCompData["hit_by_player"];
	registry.enemies.get(e).state = enemyCompData["state"];
	registry.enemies.get(e).type = enemyCompData["type"];
	registry.enemies.get(e).inv = inv;
}

void WorldSystem::loadPlayerComponent(Entity e, json playerCompData, Inventory inv) {
	registry.players.get(e).attacked = playerCompData["attacked"];
	registry.players.get(e).gacha_pity = playerCompData["gacha_pity"];
	registry.players.get(e).floor = playerCompData["floor"];
	registry.players.get(e).room = playerCompData["room"];
	registry.players.get(e).total_rooms = playerCompData["total_rooms"];
	registry.players.get(e).inv = inv;
}

Inventory WorldSystem::loadInventory(Entity e, json inventoryData) {
	Inventory& inv = registry.inventories.get(e);
	// get artifacts
	int artifact[static_cast<int>(ARTIFACT::ARTIFACT_COUNT)];
	int i = 0;
	for (auto& artifact : inventoryData["artifact"]) {
		inv.artifact[i] = artifact;
		i++;
	}

	// get consumables
	i = 0;
	for (auto& consumable : inventoryData["consumable"]) {
		inv.consumable[i] = consumable;
		i++;
	}
	
	// load weapon
	json weaponJson = inventoryData["equipped"]["weapon"];
	Equipment weapon;
	weapon.atk = weaponJson["atk"];
	weapon.def = weaponJson["def"];
	weapon.ep = weaponJson["ep"];
	weapon.hp = weaponJson["hp"];
	weapon.mp = weaponJson["mp"];
	weapon.range = weaponJson["range"];
	weapon.speed = weaponJson["speed"];
	weapon.type = weaponJson["type"];
	i = 0;
	for (auto& attack : weaponJson["attacks"]) {
		weapon.attacks[i] = attack;
		i++;
	}
	inv.equipped[0] = weapon;

	// get armour
	json armourJson = inventoryData["equipped"]["armour"];
	Equipment armour;
	armour.atk = armourJson["atk"];
	armour.def = armourJson["def"];
	armour.ep = armourJson["ep"];
	armour.hp = armourJson["hp"];
	armour.mp = armourJson["mp"];
	armour.range = armourJson["range"];
	armour.speed = armourJson["speed"];
	armour.type = armourJson["type"];
	i = 0;
	for (auto& attack : armourJson["attacks"]) {
		armour.attacks[i] = attack;
		i++;
	}
	inv.equipped[1] = armour;

	return inv;
}

void WorldSystem::loadStatuses(Entity e, json statuses) {
	StatusContainer& statusContainer = registry.statuses.get(e);
	for (auto& status : statuses) {
		float value = status["value"];
		int turns_remaining = status["turns_remaining"];
		StatusType effect = status["effect"];
		bool percentage = status["percentage"];
		bool apply_at_turn_start = status["apply_at_turn_start"];
		statusContainer.statuses.push_back(StatusEffect(value, turns_remaining, effect, percentage, apply_at_turn_start));
	}
}

void WorldSystem::loadTiles(json tileList) {
	for (auto& tile : tileList) {
		Entity e = Entity();

		Motion& motion = registry.motions.emplace(e);
		motion.position = { tile["motion"]["position_x"], tile["motion"]["position_y"] };
		motion.scale = { tile["motion"]["scale"]["x"], tile["motion"]["scale"]["y"]};

		json uvData = tile["tileUV"];
		TileUV& tileUV = registry.tileUVs.emplace(e);
		tileUV.layer = uvData["layer"];
		tileUV.tileID = uvData["tileID"];
		tileUV.uv_end = { uvData["uv_end"]["x"], uvData["uv_end"]["y"] };
		tileUV.uv_start = { uvData["uv_start"]["x"], uvData["uv_start"]["y"] };

		RenderRequest renderRequest = {
		static_cast<TEXTURE_ASSET_ID>(tile["renderRequest"]["used_texture"]),
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::TILEMAP,
		static_cast<RENDER_LAYER_ID>(tile["renderRequest"]["used_layer"])
		};
		registry.renderRequests.insert(e, renderRequest);
	}
}

void WorldSystem::loadCollidables(json collidablesList) {
	for (auto& collidable : collidablesList) {
		Entity entity = Entity();
		json mData = collidable["motion"];
		Motion& motion = registry.motions.emplace(entity);
		motion.scale = { mData["scale"]["x"], mData["scale"]["y"]};
		motion.position = { mData["position_x"], mData["position_y"] };
		registry.solid.emplace(entity);
		registry.collidables.emplace(entity);
	}
}

void WorldSystem::loadInteractables(json interactablesList) {
	for (auto& interactable : interactablesList) {
		Entity e = Entity();

		registry.motions.emplace(e);
		loadMotion(e, interactable["motion"]);

		Interactable& interact_component = registry.interactables.emplace(e);
		interact_component.type = (INTERACT_TYPE)interactable["type"];

		switch ((int)interactable["type"]) {
		case 0: // chest
			loadChest(e);
		case 1: // door
			break;
		case 2: // stairs
			break;
		case 3: // sign
			loadSign(e, interactable["sign"]);
		default:
			break;
		}
	}
}

void WorldSystem::loadSign(Entity e, json signData) {
	Sign& sign = registry.signs.emplace(e);
	std::vector<std::pair<std::string, int>> msgs = std::vector<std::pair<std::string, int>>();
	for (auto& msgData : signData["messages"]) {
		msgs.push_back({ msgData["message"], msgData["number"] });
	}
	sign.messages = msgs;

	AnimationData& anim = registry.animations.emplace(e);
	anim.spritesheet_texture = TEXTURE_ASSET_ID::SIGN_GLOW_SPRITESHEET;
	anim.frametime_ms = 200;
	anim.frame_indices = { 0, 1, 2, 3, 4, 5, 6, 7 };
	anim.spritesheet_columns = 8;
	anim.spritesheet_rows = 1;
	anim.spritesheet_width = 256;
	anim.spritesheet_height = 32;
	anim.frame_size = { anim.spritesheet_width / anim.spritesheet_columns, anim.spritesheet_height / anim.spritesheet_rows };

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(e, &mesh);

	registry.renderRequests.insert(
		e,
		{ TEXTURE_ASSET_ID::SIGN_GLOW_SPRITESHEET,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::ANIMATION,
		RENDER_LAYER_ID::SPRITE
		});
}

void WorldSystem::loadChest(Entity e) {
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(e, &mesh);

	registry.renderRequests.insert(
		e,
		{ TEXTURE_ASSET_ID::CHEST,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(e);
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
	TextTimer& timer = registry.textTimers.emplace(e);
	timer.counter_ms = 8000;
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
			aiSystem.step(currentTurnEntity);
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
		aiSystem.step(currentTurnEntity);

		// now that ai did its step, set doing turn to false
		registry.queueables.get(currentTurnEntity).doing_turn = false;
	}
}

// Set attack state for enemies who attack after moving
void set_enemy_state_attack(Entity enemy) {
	if (registry.enemies.get(enemy).type == ENEMY_TYPE::SLIME ||
		registry.enemies.get(enemy).type == ENEMY_TYPE::PLANT_SHOOTER ||
		registry.enemies.get(enemy).type == ENEMY_TYPE::CAVELING) {
		registry.enemies.get(enemy).state = ENEMY_STATE::ATTACK;
	}
}

// Set game state
void set_gamestate(GameStates state) {
	previous_game_state = current_game_state;
	current_game_state = state;
}

// Check if entity has a status effect;
bool has_status(Entity e, StatusType status) {
	if (!registry.statuses.has(e)) { return false; }

	StatusContainer statuses = registry.statuses.get(e);
	for (StatusEffect s : statuses.statuses) {
		if (s.effect == status) {
			return true;
		}
	}
	return false;
}

// Remove a number of a status effect type from entity
void remove_status(Entity e, StatusType status, int number) {
	if (!registry.statuses.has(e)) { return; }

	int index = 0;
	StatusContainer statuses = registry.statuses.get(e);
	for (StatusEffect s : statuses.statuses) {
		if (s.effect == status && number > 0) {
			statuses.statuses.erase(statuses.statuses.begin() + index);
			number--;
			index++;
		}
	}
	return;
}

void WorldSystem::update_turn_ui() {
	// clear icon registry
	for (Entity e : registry.icons.entities) {
		registry.remove_all_components_of(e);
	}

	Motion& turn_ui_motion = registry.motions.get(turnUI);
	vec2 position = turn_ui_motion.position;
	vec2 startPos = vec2(turn_ui_motion.scale[0]/2.f, 0.f);
	vec2 offset = vec2(0.f);

	// get queue
	std::queue<Entity> queue = turnOrderSystem.getTurnOrder();

	for (int count = 0; !queue.empty() && count < 5; queue.pop()) {
		Entity e = queue.front();
		if (!registry.hidden.has(e)) {
			offset[0] = 48.f*count + 32.f;
			TEXTURE_ASSET_ID texture_id = registry.renderRequests.get(e).used_texture;
			createIcon(renderer, position - startPos + offset, texture_id);
			count++;
		}
	}
	return;
}