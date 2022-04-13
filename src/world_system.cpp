#include "world_system.hpp"
#include "world_init.hpp"
// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

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
	if (boss0_music != nullptr)
		Mix_FreeMusic(boss0_music);
	if (ruins_music != nullptr)
		Mix_FreeMusic(ruins_music);
	if (boss1_music != nullptr)
		Mix_FreeMusic(boss1_music);
	if (door_sound != nullptr)
		Mix_FreeChunk(door_sound);
	if (switch_sound != nullptr)
		Mix_FreeChunk(switch_sound);
	if (chest_sound != nullptr)
		Mix_FreeChunk(chest_sound);
	if (slime_move != nullptr)
		Mix_FreeChunk(slime_move);
	if (slime_death != nullptr)
		Mix_FreeChunk(slime_death);
	if (caveling_move != nullptr)
		Mix_FreeChunk(caveling_move);
	if (caveling_death != nullptr)
		Mix_FreeChunk(caveling_death);
	if (whoosh != nullptr)
		Mix_FreeChunk(whoosh);
	if (sword_end != nullptr)
		Mix_FreeChunk(sword_end);
	if (sword_parry != nullptr)
		Mix_FreeChunk(sword_parry);
	if (sword_pierce != nullptr)
		Mix_FreeChunk(sword_pierce);
	if (sword_slash != nullptr)
		Mix_FreeChunk(sword_slash);
	if (special_sound != nullptr)
		Mix_FreeChunk(special_sound);
	if (ui_click != nullptr)
		Mix_FreeChunk(ui_click);
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
float fog_radius = 300.f * ui_scale;
float fog_resolution = 2000.f * ui_scale;

// ep range stats
float ep_resolution = 2000.f * ui_scale;

// move audio timer
float move_audio_timer_ms = 200.f;

// enemy move audio timer
float enemy_move_audio_time_ms = 200.f;

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
	cutscene_music = Mix_LoadMUS(audio_path("bgm/dream0.wav").c_str());
	boss0_music = Mix_LoadMUS(audio_path("bgm/boss0.wav").c_str());
	ruins_music = Mix_LoadMUS(audio_path("bgm/ruins0.wav").c_str());
	boss1_music = Mix_LoadMUS(audio_path("bgm/boss1.wav").c_str());

	// Sounds and volumes
	fire_explosion_sound = Mix_LoadWAV(audio_path("feedback/fire_explosion.wav").c_str());
	Mix_VolumeChunk(fire_explosion_sound, 13);
	error_sound = Mix_LoadWAV(audio_path("feedback/error.wav").c_str());
	Mix_VolumeChunk(error_sound, 13);
	footstep_sound = Mix_LoadWAV(audio_path("feedback/footstep.wav").c_str());
	Mix_VolumeChunk(footstep_sound, 42);
	door_sound = Mix_LoadWAV(audio_path("feedback/door_open.wav").c_str());
	Mix_VolumeChunk(door_sound, 32);
	switch_sound = Mix_LoadWAV(audio_path("feedback/switch_click.wav").c_str());
	Mix_VolumeChunk(switch_sound, 32);
	chest_sound = Mix_LoadWAV(audio_path("feedback/chest_open.wav").c_str());
	Mix_VolumeChunk(chest_sound, 32);
	special_sound = Mix_LoadWAV(audio_path("sfx/special.wav").c_str());
	Mix_VolumeChunk(special_sound, 24);
	whoosh = Mix_LoadWAV(audio_path("sfx/whoosh.wav").c_str());
	Mix_VolumeChunk(whoosh, 24);
	sword_end = Mix_LoadWAV(audio_path("sfx/sword_end.wav").c_str());
	Mix_VolumeChunk(sword_end, 24);
	sword_parry = Mix_LoadWAV(audio_path("sfx/sword_parry.wav").c_str());
	Mix_VolumeChunk(sword_parry, 24);
	sword_pierce = Mix_LoadWAV(audio_path("sfx/sword_pierce.wav").c_str());
	Mix_VolumeChunk(sword_pierce, 24);
	sword_slash = Mix_LoadWAV(audio_path("sfx/sword_slash.wav").c_str());
	Mix_VolumeChunk(sword_slash, 24);
	slime_move = Mix_LoadWAV(audio_path("feedback/slime_move.wav").c_str());
	Mix_VolumeChunk(slime_move, 20);
	slime_death = Mix_LoadWAV(audio_path("feedback/slime_death.wav").c_str());
	Mix_VolumeChunk(slime_death, 24);
	caveling_move = Mix_LoadWAV(audio_path("feedback/caveling_move.wav").c_str());
	Mix_VolumeChunk(caveling_move, 14);
	caveling_death = Mix_LoadWAV(audio_path("feedback/caveling_death.wav").c_str());
	Mix_VolumeChunk(caveling_death, 30);
	ui_click = Mix_LoadWAV(audio_path("feedback/ui_click.wav").c_str());
	Mix_VolumeChunk(ui_click, 32);
	kingslime_attack = Mix_LoadWAV(audio_path("sfx/slimeattack.wav").c_str());
	Mix_VolumeChunk(kingslime_attack, 24);
	kingslime_jump = Mix_LoadWAV(audio_path("sfx/slimejump.wav").c_str());
	Mix_VolumeChunk(kingslime_jump, 24);
	kingslime_summon = Mix_LoadWAV(audio_path("sfx/slimesummon.wav").c_str());
	Mix_VolumeChunk(kingslime_summon, 24);

	if (background_music == nullptr || fire_explosion_sound == nullptr
		|| error_sound == nullptr || footstep_sound == nullptr
		|| menu_music == nullptr || cutscene_music == nullptr
		|| door_sound == nullptr || switch_sound == nullptr
		|| chest_sound == nullptr || slime_move == nullptr 
		|| slime_death == nullptr || caveling_death == nullptr
		|| caveling_move == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("bgm/caves0.wav").c_str(),
			audio_path("bgm/menu0.wav").c_str(),
			audio_path("bgm/dream0.wav").c_str(),
			audio_path("feedback/fire_explosion.wav").c_str(),
			audio_path("feedback/error.wav").c_str(),
			audio_path("feedback/footstep.wav").c_str(),
			audio_path("feedback/door_open.wav").c_str(),
			audio_path("feedback/switch_click.wav").c_str(),
			audio_path("feedback/chest_open.wav").c_str(),
			audio_path("feedback/slime_move.wav").c_str(),
			audio_path("feedback/slime_death.wav").c_str(),
			audio_path("feedback/caveling_death.wav").c_str(),
			audio_path("feedback/caveling_move.wav").c_str()
		);
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely

	playMusic(Music::CUTSCENE);
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
	title_ss << "Adrift In Somnium";
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

	// remove pickups that have been interacted
	for (int i = (int)registry.interactables.components.size() - 1; i >= 0; --i) {
		Interactable& interactable = registry.interactables.components[i];
		Entity entity = registry.interactables.entities[i];
		if (interactable.type == INTERACT_TYPE::PICKUP && interactable.interacted) {
			Inventory& inv = registry.inventories.get(player_main);
			Motion& player_motion = registry.motions.get(player_main);
			if (registry.artifacts.has(entity)) {
				ARTIFACT artifact = registry.artifacts.get(entity).type;
				inv.artifact[(int)artifact]++;
				reset_stats(player_main);
				calc_stats(player_main);
				remove_fog_of_war();
				create_fog_of_war();
			}
			if (registry.equipment.has(entity)) {
				Equipment equipment = registry.equipment.get(entity);
				Equipment prev = equip_item(player_main, equipment);
				createEquipmentEntity(renderer, player_motion.position, prev);
			}
			if (current_game_state == GameStates::ITEM_MENU) {
				// re-render the itemCards
				for (Entity ic : registry.itemCards.entities) {
					registry.remove_all_components_of(ic);
				}
				createItemMenu(renderer, { window_width_px - 125.f, 200.f }, inv);
			}
			if (registry.consumables.has(entity)) {
				Consumable consumable = registry.consumables.get(entity);
				Stats stats = registry.stats.get(player_main);
				Stats basestats = registry.basestats.get(player_main);
				StatusEffect regen = StatusEffect(stats.maxhp * 0.06, 5, StatusType::HP_REGEN, false, true);
				switch (consumable.type) {
				case CONSUMABLE::REDPOT:
					break;
				case CONSUMABLE::BLUPOT:
					break;
				case CONSUMABLE::YELPOT:
					break;
				case CONSUMABLE::INSTANT:
					if (has_status(player_main, StatusType::HP_REGEN)) { remove_status(player_main, StatusType::HP_REGEN); }
					apply_status(player_main, regen);

					// Guide to Healthy Eating
					if (inv.artifact[(int)ARTIFACT::GUIDE_HEALBUFF] > 0) {
						StatusEffect buff = StatusEffect(0.2 * inv.artifact[(int)ARTIFACT::GUIDE_HEALBUFF], 5, StatusType::ATK_BUFF, true, true);
						if (has_status(player_main, StatusType::ATK_BUFF)) { remove_status(player_main, StatusType::ATK_BUFF); }
						apply_status(player_main, buff);
					}
					break;
				default:
					break;
				}
			}
			registry.remove_all_components_of(registry.interactables.entities[i]);
		}
	}

	// if not in menu do turn order logic (!current_game_state)
	if (current_game_state < GameStates::CUTSCENE && current_game_state >= GameStates::GAME_START) {
		if (tutorial) {
			updateTutorial();
		}
		update_turn_ui();
		doTurnOrderLogic();
		remove_fog_of_war();
		create_fog_of_war();
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
	if (mouseXpos > window_width_px - 200.f || mouseYpos < 100.f) {
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

	// handle enemy move sounds
	if (current_game_state == GameStates::ENEMY_TURN) {
		if (enemy_move_audio_time_ms <= 0) {
			for (Entity e : registry.enemies.entities) {
				Motion motion = registry.motions.get(e);
				if (motion.in_motion) {
					playEnemyMoveSound(registry.enemies.get(e).type);
				}
			}
			enemy_move_audio_time_ms = 200.f;
		}
		else {
			enemy_move_audio_time_ms -= elapsed_ms_since_last_update;
		}
	}

	// update per-enemy hp bar positions
	for (int i = 0; i < registry.enemyHPBars.size(); i++) {
		Entity enemy = registry.enemyHPBars.entities[i];
		EnemyHPBar& hpbar = registry.enemyHPBars.components[i];
		if (!registry.motions.has(hpbar.hpBacking) || !registry.motions.has(hpbar.hpFill)) {
			continue;
		}
		Stats& stats = registry.stats.get(enemy);

		Motion& enemy_motion = registry.motions.get(enemy);
		Motion& hpbacking_motion = registry.motions.get(hpbar.hpBacking);
		Motion& hpfill_motion = registry.motions.get(hpbar.hpFill);

		hpbacking_motion.position = enemy_motion.position + vec2(0, ENEMY_HP_BAR_OFFSET);
		hpfill_motion.scale.x = hpbacking_motion.scale.x * max(0.f, (stats.hp / stats.maxhp));
		hpfill_motion.position = hpbacking_motion.position - vec2((hpbacking_motion.scale.x - hpfill_motion.scale.x) / 2, 0);
	}

	// update the boss hp bar
	for (int i = 0; i < registry.bossHPBars.size(); i++) {
		Entity boss = registry.bossHPBars.entities[i];
		BossHPBar& hpbar = registry.bossHPBars.components[i];
		if (!registry.motions.has(hpbar.hpBacking) || !registry.motions.has(hpbar.hpFill)) {
			continue;
		}
		Stats& stats = registry.stats.get(boss);

		Motion& hpbacking_motion = registry.motions.get(hpbar.hpBacking);
		Motion& hpfill_motion = registry.motions.get(hpbar.hpFill);

		hpfill_motion.scale.x = hpbacking_motion.scale.x * max(0.f, (stats.hp / stats.maxhp));
		hpfill_motion.position = hpbacking_motion.position - vec2((hpbacking_motion.scale.x - hpfill_motion.scale.x) / 2, 0);
	}

	// update per-enemy shadows
	for (int i = 0; i < registry.shadowContainers.size(); i++) {
		Entity enemy = registry.shadowContainers.entities[i];
		ShadowContainer& shadow_container = registry.shadowContainers.components[i];
		if (!registry.motions.has(shadow_container.shadow_entity)) {
			continue;
		}
		Motion& shadow_motion = registry.motions.get(shadow_container.shadow_entity);
		Motion& player_motion = registry.motions.get(player_main);
		Motion& enemy_motion = registry.motions.get(enemy);

		float angle = atan2(enemy_motion.position.y - player_motion.position.y, enemy_motion.position.x - player_motion.position.x);
		float distance = dist_to(enemy_motion.position, player_motion.position);
		float length_scale = 1;
		if (distance < 64) {
			length_scale = distance / 64.f;
		}
		else {
			length_scale = 1 + min(distance, 300.f) / 300.f;
		}
		shadow_motion.scale = vec2(enemy_motion.scale.x * length_scale, enemy_motion.scale.y);
		shadow_motion.angle = angle;
		shadow_motion.position = dirdist_extrapolate(enemy_motion.position, angle, shadow_motion.scale.x/2 - enemy_motion.scale.x/4);
		// shift the shadow a little if enemy is a plantshooter
		if (registry.enemies.has(enemy) && registry.enemies.get(enemy).type == ENEMY_TYPE::PLANT_SHOOTER) {
			shadow_motion.position += vec2(0, 0.25 * enemy_motion.scale.y);
		}
	}

	for (Entity p : registry.players.entities) {
		Player player = registry.players.get(p);
		Motion player_motion = registry.motions.get(player_main);
		Stats stats = registry.stats.get(player_main);

		// update FoW if moving
		if (registry.knockbacks.has(p) || player_motion.in_motion) {
			// update the fog of war if the player is being knocked back
			remove_fog_of_war();
			create_fog_of_war();
		}

		// render the attack range
		if (current_game_state == GameStates::ATTACK_MENU) {
			// render the attack range based on the currently prepared attack
			if (player.using_attack == ATTACK::NONE || player.using_attack == ATTACK::ROUNDSLASH || player.using_attack == ATTACK::SAPPING_STRIKE
				|| player.using_attack == ATTACK::PIERCING_THRUST || player.using_attack == ATTACK::TERMINUS_VERITAS) {
				float attack_range = 50.f + player_motion.scale.x / 2;
				switch (player.using_attack) {
				case ATTACK::ROUNDSLASH:
					attack_range = 150.f;
					break;
				case ATTACK::PIERCING_THRUST:
					attack_range = 250.f;
					break;
				case ATTACK::TERMINUS_VERITAS:
					attack_range = stats.range;
					break;
				}
				create_attack_range(attack_range, player_motion.position);
			}
			else {
				remove_attack_range();
			}
		}
		else {
			remove_attack_range();
		}

		// perform in-motion behaviour
		if (get_is_player_turn() && player_move_click) {
			if (player_motion.in_motion) {
				// handle footstep sound
				if (move_audio_timer_ms <= 0) {
					// play the footstep sound
					Mix_PlayChannel(-1, footstep_sound, 0);
					move_audio_timer_ms = 200.f;
				}
				else {
					move_audio_timer_ms -= elapsed_ms_since_last_update;
				}

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

		if (registry.keyIcons.entities.size() < 4) {
			if (current_game_state == GameStates::ATTACK_MENU && !player.prepared) {
				// need to move everything down one in attack menu
				// TODO: un-hard-code these
				createKeyIcon(renderer, { window_width_px - 70.f, 140.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_1);
				createKeyIcon(renderer, { window_width_px - 70.f, 260.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_2);
				createKeyIcon(renderer, { window_width_px - 70.f, 380.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_3);
				createKeyIcon(renderer, { window_width_px - 70.f, 500.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_4);
				createKeyIcon(renderer, { window_width_px - 70.f, 620.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_5);
			}
			else if (current_game_state == GameStates::BATTLE_MENU) {
				if (tutorial) {
					if (tutorial_flags & SIGN_1 && registry.keyIcons.entities.size() < 1) { createKeyIcon(renderer, { window_width_px - 60.f, 300.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_2); }
					if (tutorial_flags & EP_DEPLETED && registry.keyIcons.entities.size() < 2) { createKeyIcon(renderer, { window_width_px - 60.f, 450.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_3); }
					if (tutorial_flags & SIGN_2 && registry.keyIcons.entities.size() < 3) { createKeyIcon(renderer, { window_width_px - 60.f, 150.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_1); }
					if (tutorial_flags & SIGN_4 && registry.keyIcons.entities.size() < 4) { createKeyIcon(renderer, { window_width_px - 60.f, 600.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_4); }
				} else {
					createKeyIcon(renderer, { window_width_px - 60.f, 150.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_1);
					createKeyIcon(renderer, { window_width_px - 60.f, 300.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_2);
					createKeyIcon(renderer, { window_width_px - 60.f, 450.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_3);
					createKeyIcon(renderer, { window_width_px - 60.f, 600.f * ui_scale}, TEXTURE_ASSET_ID::KEY_ICON_4);
				}
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

		if (registry.actionButtons.entities.size() < 4) {
			// bring back all of the buttons
			// sequentially add buttons depending on flags
			if (tutorial) {
				if (tutorial_flags & SIGN_1 && registry.actionButtons.entities.size() < 1) { createMoveButton(renderer, { window_width_px - 125.f, 350.f * ui_scale }); }
				if (tutorial_flags & EP_DEPLETED && registry.actionButtons.entities.size() < 2) { createGuardButton(renderer, { window_width_px - 125.f, 500.f * ui_scale }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD); }
				if (tutorial_flags & SIGN_2 && registry.actionButtons.entities.size() < 3) { createAttackButton(renderer, { window_width_px - 125.f, 200.f * ui_scale}); }
				if (tutorial_flags & SIGN_4 && registry.actionButtons.entities.size() < 4) { createItemButton(renderer, { window_width_px - 125.f, 650.f * ui_scale }); }
			} else {
				createAttackButton(renderer, { window_width_px - 125.f, 200.f * ui_scale});
				createMoveButton(renderer, { window_width_px - 125.f, 350.f * ui_scale });
				createGuardButton(renderer, { window_width_px - 125.f, 500.f * ui_scale }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD);
				createItemButton(renderer, { window_width_px - 125.f, 650.f * ui_scale });
			}
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
	// Check for player death / walking
	for (Entity player : registry.players.entities) {
		Player& p = registry.players.get(player);
		
		// get player stats
		float& hp = registry.stats.get(player).hp;
		float& mp = registry.stats.get(player).mp;
		float& ep = registry.stats.get(player).ep;
		float& maxhp = registry.stats.get(player).maxhp;
		float& maxmp = registry.stats.get(player).maxmp;
		float& maxep = registry.stats.get(player).maxep;

		// stop walking
		if (!registry.motions.get(player_main).in_motion && registry.animations.has(player_main)) {
			registry.animations.remove(player_main);
			registry.renderRequests.get(player_main).used_texture = TEXTURE_ASSET_ID::PLAYER;
			registry.renderRequests.get(player_main).used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
		}

		// Check if player has died
		if (hp <= 0 && !registry.deathTimers.has(player)) {
			registry.deathTimers.emplace(player);
			logText("You have died!");
			player_move_click = false;
			break;
		}

		// change guard button to end turn if ep is not full
		if ((p.attacked || p.moved || ep <= 50) && !hideGuardButton) {
			// remove guard button
			for (Entity gb : registry.guardButtons.entities) {
				registry.remove_all_components_of(gb);
			}
			// add end turn button
			if (tutorial) {
				if (tutorial_flags & EP_DEPLETED) { createGuardButton(renderer, { window_width_px - 125.f, 500.f * ui_scale }, BUTTON_ACTION_ID::ACTIONS_END_TURN, TEXTURE_ASSET_ID::ACTIONS_END_TURN); }
			} else {
				createGuardButton(renderer, { window_width_px - 125.f, 500.f * ui_scale }, BUTTON_ACTION_ID::ACTIONS_END_TURN, TEXTURE_ASSET_ID::ACTIONS_END_TURN);
			}
		}
		else if (!hideGuardButton) {
			// remove guard button
			for (Entity gb : registry.guardButtons.entities) {
				registry.remove_all_components_of(gb);
			}
			// add end turn button
			if (tutorial) {
				if (tutorial_flags & EP_DEPLETED) { createGuardButton(renderer, { window_width_px - 125.f, 500.f * ui_scale }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD); }
			} else {
				createGuardButton(renderer, { window_width_px - 125.f, 500.f * ui_scale }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD);
			}
		}

		// update player motion
		Motion& player_motion = registry.motions.get(player);
		Stats& player_stats = registry.stats.get(player);
		if (player_motion.in_motion) {
			if (ep <= 0) {
				if (registry.textboxes.size() == 0) {
					player_motion.destination = player_motion.position;
					set_gamestate(GameStates::BATTLE_MENU);
				}
			}
			else { 
				ep -= 0.06f * player_stats.epratemove * elapsed_ms_since_last_update; 
				ep = max(0.f, ep);
			}
		}

		// update stats text
		std::string currHpString = std::to_string((int)hp);
		std::string maxHpString = std::to_string((int)maxhp);

		std::string currMpString = std::to_string((int)mp);
		std::string maxMpString = std::to_string((int)maxmp);

		std::string currEpString = std::to_string((int)ep);
		std::string maxEpString = std::to_string((int)maxep);

		// remove previous stats text
		for (Entity st : registry.statsText.entities) {
			registry.remove_all_components_of(st);
		}

		float statbarsX = window_width_px * 0.14;
		float statbarsY = window_height_px * 1.7;

		createStatsText(renderer, { statbarsX, statbarsY }, currHpString + " / " + maxHpString, 1.2f, vec3(1.0f));
		createStatsText(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 }, currMpString + " / " + maxMpString, 1.2f, vec3(1.0f));
		createStatsText(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT * 4 }, currEpString + " / " + maxEpString, 1.2f, vec3(1.0f));
		
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
				motion_struct.position[0] = 150.f - 150.f*(1.f - min(1.f, ui_scale*(hp / maxhp)));	// original pos (full bar) - (1-multiplier)
				break;
			case TEXTURE_ASSET_ID::MPFILL:
				motion_struct.scale = { min(STAT_BB_WIDTH, (mp / maxmp) * STAT_BB_WIDTH), STAT_BB_HEIGHT };
				motion_struct.position[0] = 150.f - 150.f*(1.f - min(1.f, ui_scale*(mp / maxmp)));	// original pos (full bar) - (1-multiplier)
				break;
			case TEXTURE_ASSET_ID::EPFILL:
				motion_struct.scale = { min(STAT_BB_WIDTH, (ep / maxep) * STAT_BB_WIDTH), STAT_BB_HEIGHT };
				motion_struct.position[0] = 150.f - 150.f*(1.f - min(1.f, ui_scale*(ep / maxep)));	// original pos (full bar) - (1-multiplier)
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

		// Check traps for collisions/activations
		for (Entity t : registry.traps.entities) {
			if (!registry.traps.has(t)) { continue; }
			Trap& trap = registry.traps.get(t);
			if (trap.triggers <= 0) { continue; }
			Motion& trap_motion = registry.motions.get(t);

			if (registry.players.has(trap.owner)) {
				for (Entity e : registry.enemies.entities) {
					// hacky way of having enemies not die when spawning on top of a trap
					if (registry.iFrameTimers.has(e)) { continue; }
					Motion& enemy_motion = registry.motions.get(e);
					Stats& enemy_stats = registry.stats.get(e);
					if (enemy_stats.hp <= 0 || trap.triggers <= 0) { continue; }
					else if (collides_circle(trap_motion, enemy_motion)) {
						trigger_trap(t, e);
						break;
					}
				}
			}
			else {
				// internal trap cooldown
				if (registry.iFrameTimers.has(player)) { continue; }
				Motion& player_motion = registry.motions.get(player);
				Stats& player_stats = registry.stats.get(player);
				if (player_stats.hp <= 0 || trap.triggers <= 0) { continue; }
				else if (collides_circle(trap_motion, player_motion)) {
					ExpandTimer iframe = registry.iFrameTimers.emplace(player);
					iframe.counter_ms = 50;
					trigger_trap(t, player);
					break;
				}
			}
		}
	}

	// Check for enemy death
	for (Entity& enemy : registry.enemies.entities) {
		if (registry.stats.get(enemy).hp <= 0 && !registry.squishTimers.has(enemy)) {
			SquishTimer& squish = registry.squishTimers.emplace(enemy);
			squish.orig_scale = registry.motions.get(enemy).scale;

			// special behaviour if orb
			if (registry.renderRequests.get(enemy).used_texture == TEXTURE_ASSET_ID::ORB) {
				squish.counter_ms = 50;
			}
			else {
				std::string log_text = "The enemy ";
				log_text = log_text.append(registry.stats.get(enemy).name.append(" is defeated!"));
				logText(log_text);
			}

			playEnemyDeathSound(registry.enemies.get(enemy).type);
      
			int roll = irand(3);

			registry.queueables.get(enemy).doing_turn = false;
			if (current_game_state == GameStates::ENEMY_TURN) {
				registry.motions.get(enemy).in_motion = false;
				registry.motions.get(enemy).velocity = { 0, 0 };
				doTurnOrderLogic();
			}
			
			// remove from solids
			if (registry.solid.has(enemy)) {
				registry.solid.remove(enemy);
			}
			if (!tutorial && registry.enemies.get(enemy).type != ENEMY_TYPE::LIVING_PEBBLE) {
				// TEMP: drop healing item from enemy with 1/3 chance
				if (roll == 0 && !tutorial) {
					createConsumable(renderer, registry.motions.get(enemy).position + vec2(16, 16), CONSUMABLE::INSTANT);
				}
				roomSystem.updateObjective(ObjectiveType::KILL_ENEMIES, 1);
			}
			if (registry.bosses.has(enemy)) {
				roomSystem.updateObjective(ObjectiveType::DEFEAT_BOSS, 1);
				Entity bossdoor = createDoor(renderer, { registry.motions.get(enemy).position.x, registry.motions.get(enemy).position.y - 64.f }, true);
				//roomSystem.setNextFloor(Floors((int)roomSystem.current_floor + 1));
				registry.players.get(player_main).floor++;
				createCampfire(renderer, { registry.motions.get(enemy).position.x, registry.motions.get(enemy).position.y + 64.f });
				registry.enemies.get(enemy).state = ENEMY_STATE::DEATH;
				aiSystem.step(enemy);

				// Final Boss Death
				if (registry.enemies.get(enemy).type == ENEMY_TYPE::REFLEXION) {
					registry.renderRequests.get(bossdoor).used_texture == TEXTURE_ASSET_ID::ENDLIGHT;
				}
			}
			if (registry.shadowContainers.has(enemy)) {
				ShadowContainer& shadow_container = registry.shadowContainers.get(enemy);
				registry.remove_all_components_of(shadow_container.shadow_entity);
				registry.shadowContainers.remove(enemy);
			}
			// remove from enemy list so aoes don't crash the game trying to deal damage to a dead enemy
			if (registry.enemies.has(enemy)) {
				// remove from turn queue
				turnOrderSystem.removeFromQueue(enemy);
				registry.enemies.remove(enemy);
			}

			
		}
	}

	// Processing the chicken state
	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

    float min_death_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if(counter.counter_ms < min_death_counter_ms){
		    min_death_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
            restart_game();
			return true;
		}
	}

	float min_room_counter_ms = 750.f;
	for (Entity entity : registry.roomTransitions.entities) {
		// progress timer
		RoomTransitionTimer& counter = registry.roomTransitions.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < min_room_counter_ms) {
			min_room_counter_ms = counter.counter_ms;
		}

		if (counter.counter_ms < 0) {
			registry.roomTransitions.remove(entity);
			generateNewRoom(counter.floor, counter.repeat_allowed);
			return true;
		}
	}

	// reduce window brightness if any of the present chickens is dying
	screen.darken_screen_factor = max(1 - min_death_counter_ms / 3000, 1 - min_room_counter_ms / 750);

	float min_fadeout_counter_ms = 500.f;
	for (Entity entity : registry.fadeTransitionTimers.entities) {
		// progress timer
		FadeTransitionTimer& counter = registry.fadeTransitionTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < min_fadeout_counter_ms) {
			min_fadeout_counter_ms = counter.counter_ms;
		}
		
		if (counter.counter_ms <= 0) {
			registry.loadingTimers.emplace(entity);
			switch (counter.type) {
			case (TRANSITION_TYPE::MAIN_TO_GAME):
				if (tutorial) {
					start_tutorial();
					spawn_tutorial_entities();
				}
				else {
					start_game();
					roomSystem.current_floor = Floors::FLOOR1;
					spawn_game_entities();
					roomSystem.setRandomObjective();
				}
				break;
			case (TRANSITION_TYPE::CUTSCENE_TO_MAIN):
			case (TRANSITION_TYPE::CREDITS_TO_MAIN):
				set_gamestate(GameStates::MAIN_MENU);
				restart_game();
				break;
			case (TRANSITION_TYPE::MAIN_TO_CREDITS):
				enter_credits();
				break;
			case (TRANSITION_TYPE::CUTSCENE_SWITCH):
				countCutScene++;
				cutSceneSystem.updateDialogue(renderer, countCutScene);
				break;
			case (TRANSITION_TYPE::CONTINUE_TO_GAME):
				if (true) {
					start_game();
					// remove entities to load in entities
					removeForLoad();
					//printf("Removed for load\n");
					// get saved game data
					json gameData = saveSystem.getSaveData();
					//printf("getting gameData\n");
					// load the entities in
					loadFromData(gameData);
					Inventory test = registry.inventories.get(player_main);
					//printf("load game data?\n");
					logText("Game state loaded!");
					remove_fog_of_war();
					create_fog_of_war();
				}
				break;
			default:
				break;
			}
			registry.fadeTransitionTimers.remove(entity);
			break;
		}
	}
	screen.darken_screen_factor = max(screen.darken_screen_factor, 1 - min_fadeout_counter_ms / 500.f);

	float max_fadein_counter_ms = 0.f;
	for (Entity entity : registry.fadeins.entities) {
		// progress timer
		FadeInTimer& counter = registry.fadeins.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms > max_fadein_counter_ms) {
			max_fadein_counter_ms = counter.counter_ms;
		}

		if (counter.counter_ms < 0) {
			registry.fadeins.remove(entity);
		}
	}
	screen.darken_screen_factor = max(max_fadein_counter_ms/750, screen.darken_screen_factor);

	if (registry.loadingTimers.size() > 0) {
		screen.darken_screen_factor = 1;
	}

	for (Entity entity : registry.loadingTimers.entities) {
		LoadingTimer& counter = registry.loadingTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < 0) {
			registry.loadingTimers.remove(entity);
			if (registry.loadingTimers.size() == 0) {
				registry.fadeins.emplace(entity);
			}
		}
	}

	// incredibly hacky, but this is where I update attack indicators
	// that are supposed to update per step
	for (Entity e : registry.attackIndicators.entities) {
		Motion& motion = registry.motions.get(e);
		if (motion.movement_speed > 0) {
			// Don't you lecture me with your 30 dollar raycasting
			Motion player_motion = registry.motions.get(player_main);
			float length = 0;
			float dir = atan2(player_motion.position.y - motion.destination.y, player_motion.position.x - motion.destination.x);
			bool stop = false;
			while (!stop) {
				for (Entity i : registry.solid.entities) {
					if (collides_point_circle(dirdist_extrapolate(motion.destination, dir, length), registry.motions.get(i)) && !registry.bosses.has(i)) {
						stop = true;
					}
				}
				length += 10;
			}
			motion.position = dirdist_extrapolate(motion.destination, dir, length/2);
			motion.scale.x = length;
			motion.angle = dir;
		}
	}

	// Damage text timer
	// 0-150 ms: increase size to slightly more than 1
	// 150-200 ms: decrease to normal size and stay there
	// 200-1350 ms: stay at normal size
	// 1350-1500 ms: shrink to 0
	// 1500+ ms: delete
	for (Entity entity : registry.damageText.entities) {
		DamageTextTimer& counter = registry.damageText.get(entity);
		Motion& motion = registry.motions.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		if (counter.counter_ms <= 0) {
			registry.remove_all_components_of(entity);
			continue;
		}
		if (counter.counter_ms > 0 && counter.counter_ms <= 150) {
			motion.scale = { 1 - (150 - counter.counter_ms) / 150, 1 - (150 - counter.counter_ms) / 150 };
		}
		else if (counter.counter_ms > 150 && counter.counter_ms <= 1300) {
			motion.scale = { 1,1 };
		}
		else if (counter.counter_ms > 1300 && counter.counter_ms < 1350) {
			motion.scale = {1 + (counter.counter_ms - 1300)/200, 1 + (counter.counter_ms - 1300) / 200 };
		}
		else if (counter.counter_ms > 1350) {
			motion.scale = {1.25 - (counter.counter_ms-1350)/200, 1.25 - (counter.counter_ms - 1350) / 200 };
		}
	}

	// Projectile Timers
	for (Entity entity : registry.projectileTimers.entities) {
		// progress timer
		ProjectileTimer& counter = registry.projectileTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		// remove projectile once the text timer has expired
		if (counter.counter_ms < 0) {
			registry.motions.get(counter.owner).in_motion = false;
			if (!registry.solid.has(counter.owner)) {
				registry.solid.emplace(counter.owner);
			}
			registry.remove_all_components_of(entity);
		}
	}

	// Text Timers
	for (Entity entity : registry.textTimers.entities) {
		// progress timer
		TextTimer& counter = registry.textTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

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
		float x_scale = (pow(counter.counter_ms, 2) / 800000) * cos(counter.counter_ms / 50) + counter.orig_scale.x;
		float y_scale = (pow(counter.counter_ms, 2) / 800000) * cos(counter.counter_ms / 50 + M_PI) + counter.orig_scale.x;
		registry.motions.get(entity).scale = {x_scale, y_scale};
		counter.counter_ms -= elapsed_ms_since_last_update;

		// remove entity once the timer has expired
		if (counter.counter_ms < 0) {
			registry.motions.get(entity).scale = counter.orig_scale;
			registry.wobbleTimers.remove(entity);

			if (registry.bosses.has(entity) && !registry.solid.has(entity)) {
				registry.solid.emplace(entity);
			}
		}
	}

	// Squish Timers
	for (Entity entity : registry.squishTimers.entities) {
		if (!registry.motions.has(entity)) { continue; }
		// progress timer
		SquishTimer& counter = registry.squishTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		float x_scale = registry.motions.get(entity).scale.x;
		float y_scale = registry.motions.get(entity).scale.y / 1.04f;
		registry.motions.get(entity).scale = { x_scale, y_scale };
		registry.motions.get(entity).position.y += (y_scale * 1.04 - y_scale)/2;

		// remove entity once the timer has expired
		if (counter.counter_ms < 0) {
			// delete HP bar
			if (registry.enemyHPBars.has(entity)) {
				EnemyHPBar& hpbar = registry.enemyHPBars.get(entity);
				registry.remove_all_components_of(hpbar.hpBacking);
				registry.remove_all_components_of(hpbar.hpFill);
			}
			if (registry.bossHPBars.has(entity)) {
				BossHPBar& hpbar = registry.bossHPBars.get(entity);
				registry.remove_all_components_of(hpbar.icon);
				registry.remove_all_components_of(hpbar.iconBacking);
				registry.remove_all_components_of(hpbar.hpBacking);
				registry.remove_all_components_of(hpbar.hpFill);
			}
			registry.remove_all_components_of(entity);
		}
	}

	// expand timers
	for (Entity entity : registry.expandTimers.entities) {
		// progress timer
		ExpandTimer& counter = registry.expandTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		Motion& motion = registry.motions.get(entity);
		motion.scale += elapsed_ms_since_last_update * (counter.target_scale / 300);

		// remove entity once the timer has expired
		if (counter.counter_ms < 0) {
			registry.remove_all_components_of(entity);
		}
	}

	// iframe timers for enemies to not get spawnkilled by traps and break turn ui
	for (Entity entity : registry.iFrameTimers.entities) {
		// progress timer
		ExpandTimer& counter = registry.iFrameTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;

		// remove entity once the timer has expired
		if (counter.counter_ms < 0) {
			registry.iFrameTimers.remove(entity);
		}
	}

	// particle effects
	for (Entity entity : registry.particleContainers.entities) {
		ParticleContainer& container = registry.particleContainers.get(entity);
		for (ParticleEmitter& emitter : container.emitters) {
			emitter.counter_ms -= elapsed_ms_since_last_update;

			if (emitter.counter_ms < 0) {
				emitter.counter_ms = emitter.min_interval_ms + uniform_dist(rng) * (emitter.max_interval_ms - emitter.min_interval_ms);
				Motion& motion = registry.motions.get(entity);
				createParticle(motion.position, emitter);
			}
		}
	}
	
	for (int i = registry.particles.size() - 1; i >= 0; i--) {
		Entity entity = registry.particles.entities[i];
		Particle& particle = registry.particles.components[i];
		particle.counter_ms -= elapsed_ms_since_last_update;

		switch (particle.type) {
		case (PARTICLE_TYPE::ATK_UP):
		case (PARTICLE_TYPE::ATK_DOWN):
		case (PARTICLE_TYPE::DEF_UP):
		case (PARTICLE_TYPE::DEF_DOWN):
		case (PARTICLE_TYPE::RANGE_UP):
		case (PARTICLE_TYPE::RANGE_DOWN):
			if (registry.colors.has(entity)) {
				vec4& color = registry.colors.get(entity);
				color.a -= (0.75 * elapsed_ms_since_last_update / 1500.f);
			}
			break;
		case (PARTICLE_TYPE::SLIMED):
			if (registry.motions.has(entity)) {
				Motion& motion = registry.motions.get(entity);
				motion.velocity.y += 0.4 * elapsed_ms_since_last_update; // gravity-like effect
			}
			break;
		case (PARTICLE_TYPE::STUN):
			if (registry.colors.has(entity)) {
				vec4& color = registry.colors.get(entity);
				color.a -= (0.8 * elapsed_ms_since_last_update / 1000.f);
			}
			break;
		case (PARTICLE_TYPE::INVINCIBLE):
			if (registry.colors.has(entity)) {
				vec4& color = registry.colors.get(entity);
				color.a -= elapsed_ms_since_last_update / 1000.f;
			}
			if (registry.motions.has(entity)) {
				Motion& motion = registry.motions.get(entity);
				motion.scale.x += 16 * elapsed_ms_since_last_update / 2000.f;
				motion.scale.y += 16 * elapsed_ms_since_last_update / 2000.f;
			}
			break;
		case (PARTICLE_TYPE::HP_REGEN):
			if (registry.colors.has(entity)) {
				vec4& color = registry.colors.get(entity);
				color.a -= elapsed_ms_since_last_update / 1000.f;
			}
			if (particle.counter_ms > 333) {
				Motion& motion = registry.motions.get(entity);
				motion.scale.x += 8 * elapsed_ms_since_last_update / 667.f;
				motion.scale.y += 8 * elapsed_ms_since_last_update / 667.f;
			}
			else {
				Motion& motion = registry.motions.get(entity);
				motion.scale.x -= 16 * elapsed_ms_since_last_update / 333.f;
				motion.scale.y -= 16 * elapsed_ms_since_last_update / 333.f;
			}
			break;
		default:
			break;
		}

		if (particle.counter_ms < 0) {
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

	// update game background (only on player turn)
	if (current_game_state >= GameStates::GAME_START && current_game_state != GameStates::CUTSCENE) {
		updateGameBackground();
	}
	// update chest textures if flagged
	for (Entity& entity : registry.chests.entities) {
		Chest& chest = registry.chests.get(entity);
		if (chest.needs_retexture) {
			if (registry.renderRequests.has(entity)) {
				RenderRequest& rr = registry.renderRequests.get(entity);
				if (chest.isArtifact) {
					if (chest.opened) {
						rr.used_texture = TEXTURE_ASSET_ID::CHEST_ARTIFACT_OPEN;

						// Smoke Powder effect
						if (registry.inventories.get(player_main).artifact[(int)ARTIFACT::SMOKE_POWDER] > 0) {
							float range = 125 + 75 * registry.inventories.get(player_main).artifact[(int)ARTIFACT::SMOKE_POWDER];
							for (Entity e : registry.enemies.entities) {
								if (dist_to(registry.motions.get(e).position, registry.motions.get(player_main).position) > range) { continue; }
								StatusEffect debuff = StatusEffect(-0.5, 1, StatusType::RANGE_BUFF, true, true);
								apply_status(e, debuff);
							}

							Entity smoke = createBigSlash(world.renderer, registry.motions.get(player_main).position, 0, range);
							registry.renderRequests.get(smoke).used_texture = TEXTURE_ASSET_ID::SMOKE;
							registry.expandTimers.get(smoke).counter_ms = 1000;
						}
					}
					else {
						rr.used_texture = TEXTURE_ASSET_ID::CHEST_ARTIFACT_CLOSED;
					}
				}
				else {
					if (chest.opened) {
						rr.used_texture = TEXTURE_ASSET_ID::CHEST_ITEM_OPEN;

						// Smoke Powder effect
						if (registry.inventories.get(player_main).artifact[(int)ARTIFACT::SMOKE_POWDER] > 0) {
							float range = 125 + 75 * registry.inventories.get(player_main).artifact[(int)ARTIFACT::SMOKE_POWDER];
							for (Entity e : registry.enemies.entities) {
								if (dist_to(registry.motions.get(e).position, registry.motions.get(player_main).position) > range) { continue; }
								StatusEffect debuff = StatusEffect(-0.5, 1, StatusType::RANGE_BUFF, true, true);
								apply_status(e, debuff);
							}

							Entity smoke = createBigSlash(world.renderer, registry.motions.get(player_main).position, 0, range);
							registry.renderRequests.get(smoke).used_texture = TEXTURE_ASSET_ID::SMOKE;
							registry.expandTimers.get(smoke).counter_ms = 1000;
						}
					}
					else {
						rr.used_texture = TEXTURE_ASSET_ID::CHEST_ITEM_CLOSED;
					}
				}
				chest.needs_retexture = false;
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
	registry.remove_all_components_of(active_camera_entity);
	active_camera_entity = createCamera({ 0, 0 });

	registry.list_all_components();

	set_gamestate(GameStates::CUTSCENE);
	//create cut scene 

	// check when the left mouse is clicked move to next picture 

	// on left click change scene to new one (x2)
	// checks how many times left click was one with countCutScene & makes sure the game state is CutScene 
	if (current_game_state == GameStates::CUTSCENE) {
		cutSceneSystem.scene_transition(renderer, countCutScene);
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

	if (current_game_state == GameStates::MAIN_MENU && current_music != Music::MENU) {
		playMusic(Music::MENU);
	}

	/*if (current_game_state != GameStates::MAIN_MENU) {
		//current_game_state = GameStates::MAIN_MENU;
		std::cout << "ACTION: RESTART THE GAME ON THE MENU SCREEN : Game state = MAIN_MENU" << std::endl;
		//printf("ACTION: RESTART THE GAME ON THE MENU SCREEN : Game state = MAIN_MENU");
	}*/
	//current_game_state = GameStates::MAIN_MENU;
	//printf("ACTION: RESTART THE GAME ON THE MENU SCREEN : Game state = MAIN_MENU");

	

	createMenuStart(renderer, { window_width_px / 6, 500.f * ui_scale });
	createMenuContinue(renderer, { window_width_px / 6, 650.f * ui_scale });
	createMenuQuit(renderer, { window_width_px / 6, 800.f * ui_scale });
	createMenuCredits(renderer, { window_width_px - 150.f, 850.f * ui_scale });
	createMenuTitle(renderer, { window_width_px / 2, window_height_px / 2 });

	if (saveSystem.saveDataExists()) {
		printf("%d size of inventory\n", registry.inventories.size());
		// width: window_width_px / 5, height = (600.f*ui_scale/2)
		update_background_collection(window_width_px / 5, (600.f*ui_scale / 2));
	}
}

void WorldSystem::handle_end_player_turn(Entity player) {
	Motion& player_motion = registry.motions.get(player);
	Player& p = registry.players.get(player);

	// Need to have this outside of handle_status_ticks or else it'll erase the wrong status
	if (has_status(player_main, StatusType::DISENGAGE_TRIGGER) && !p.attacked) {
		StatusEffect regen = StatusEffect(30, 0, StatusType::EP_REGEN, false, true);
		apply_status(player_main, regen);
	}

	player_motion.velocity = { 0.f, 0.f };
	player_motion.in_motion = false;
	p.attacked = false;
	p.moved = false;
	enemy_move_audio_time_ms = 0.f;

	set_is_player_turn(false);
	player_move_click = false;
	logText("It is now the enemies' turn!", {0.6, 0.6, 0.6});
	// set player's doing_turn to false
	registry.queueables.get(player).doing_turn = false;
	set_gamestate(GameStates::ENEMY_TURN);
}

// spawn tutorial entities
void WorldSystem::spawn_tutorial_entities() {
	std::string next_map = roomSystem.getRandomRoom(Floors::TUTORIAL, true);
	spawnData = createTiles(renderer, next_map);

	// create all non-menu game objects
	// spawn the player and enemy in random locations
	spawn_player_random_location(spawnData.playerSpawns);
	// spawn_enemies_random_location(spawnData.enemySpawns, spawnData.minEnemies, spawnData.maxEnemies);
	// spawn_items_random_location(spawnData.itemSpawns, spawnData.minItems, spawnData.maxItems);

	Entity player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);

	// spawn signs at appropriate locations
	std::vector<std::vector<std::string>> messages = {
		{
			"Welcome To ADRIFT IN SOMNIUM"
		},
		{
			"Click on the move icon or press [2] to continue."
		}
	};
	tutorial_sign_1 = createSign2(renderer, { player_motion.position.x - 64*ui_scale, player_motion.position.y - 64*ui_scale }, messages);
	registry.hidables.emplace(tutorial_sign_1);
	Entity m1 = createMouseAnimation(renderer, { player_motion.position.x - 64*ui_scale, player_motion.position.y - 2*64*ui_scale });

	std::vector<std::vector<std::string>> messages2 = {
		{
			"WATCH OUT!",
			"",
			"A weakened slime has appeared!"
		},
		{
			"Move towards the slime and",
			"click on the attack icon or press [1] to continue."
		}
	};
	tutorial_sign_2 = createSign2(renderer, { player_motion.position.x + 11*64*ui_scale, player_motion.position.y - 10*64*ui_scale }, messages2);
	registry.hidables.emplace(tutorial_sign_2);
	Entity m2 = createMouseAnimation(renderer, { player_motion.position.x + 11*64*ui_scale, player_motion.position.y - 11*64*ui_scale });

	std::vector<std::vector<std::string>> messages3 = {
		{
			"Speaking of interactables,",
			"You can find many chests."
		},
		{
			"There are 2 kinds of chests."
		},
		{
			"The chests with round corners contain weapons!",
			"Different weapons have different abilities.",
			"Be sure to experiment with different weapons!"
		},
		{
			"The chests with square corners contain artifacts!",
			"Artifacts can give you various beneficial effects!"
		},
		{
			"Open the chest on the right to move on."
		}
	};
	tutorial_sign_3 = createSign2(renderer, { player_motion.position.x + 10*64*ui_scale + 32*ui_scale, player_motion.position.y - 28*64*ui_scale }, messages3);
	registry.hidables.emplace(tutorial_sign_3);
	Entity m3 = createMouseAnimation(renderer, { player_motion.position.x + 10*64*ui_scale + 32*ui_scale, player_motion.position.y - 29*64*ui_scale });

	std::vector<std::vector<std::string>> messages4 = {
		{
			"Sometimes you can find potions that you can click to use.",
			"Healing potions will heal a portion of your HP over several turns."
		}
	};
	tutorial_sign_4 = createSign2(renderer, { player_motion.position.x, player_motion.position.y - 25*64*ui_scale }, messages4);
	registry.hidables.emplace(tutorial_sign_4);
	Entity m4 = createMouseAnimation(renderer, { player_motion.position.x, player_motion.position.y - 26*64*ui_scale });

	std::vector<std::vector<std::string>> messages5 = {
		{
			"To proceed through the rooms, you have to complete objectives.",
			"Your current objective will be shown in the top left!"
		},
		{
			"The Objective remaining shows you how much progress you have left",
			"Once the objective is complete, a door will open somewhere in the map"
		},
		{
			"When a door appears,",
			"find your way to the door and left click it to move to the next room!"
		}
	};
	tutorial_sign_5 = createSign2(renderer, { player_motion.position.x, player_motion.position.y - 37*64*ui_scale }, messages5);
	registry.hidables.emplace(tutorial_sign_5);
	Entity m5 = createMouseAnimation(renderer, { player_motion.position.x, player_motion.position.y - 38*64*ui_scale });

	// spawn chests at appropriate locations
	tutorial_chest_1 = createChest(renderer, { player_motion.position.x + 14*64*ui_scale, player_motion.position.y - 32*64*ui_scale }, false);
	createMouseAnimation(renderer, { registry.motions.get(tutorial_chest_1).position.x, registry.motions.get(tutorial_chest_1).position.y - 64 });
	tutorial_chest_2 = createChest(renderer, { player_motion.position.x + 8*64*ui_scale, player_motion.position.y - 32*64*ui_scale }, true);
	createMouseAnimation(renderer, { registry.motions.get(tutorial_chest_2).position.x, registry.motions.get(tutorial_chest_2).position.y - 64 });

	// spawn removable walls
	tutorial_wall_1 = createWall(renderer, { player_motion.position.x + 4*64*ui_scale + 32*ui_scale, player_motion.position.y - 13*64*ui_scale }, { 48*ui_scale, 6*64*ui_scale } );
	tutorial_wall_2 = createWall(renderer, { player_motion.position.x + 10*64*ui_scale + 32*ui_scale, player_motion.position.y - 17*64*ui_scale }, { 6*64*ui_scale, 48*ui_scale });
	tutorial_wall_3 = createWall(renderer, { player_motion.position.x + 13*64*ui_scale + 32*ui_scale, player_motion.position.y - 30*64*ui_scale }, { 6*64*ui_scale, 48*ui_scale });
	tutorial_wall_4 = createWall(renderer, { player_motion.position.x + 7*64*ui_scale + 32*ui_scale, player_motion.position.y - 30*64*ui_scale }, { 6*64*ui_scale, 48*ui_scale });
	tutorial_wall_5 = createWall(renderer, { player_motion.position.x + 4*64*ui_scale + 32*ui_scale, player_motion.position.y - 27*64*ui_scale }, { 48*ui_scale, 5*64*ui_scale });
	tutorial_wall_6 = createWall(renderer, { player_motion.position.x, player_motion.position.y - 34*64*ui_scale }, { 10*64*ui_scale, 48*ui_scale });

	// setup turn order system
	turnOrderSystem.setUpTurnOrder();
	// start first turn
	turnOrderSystem.getNextTurn();
	// fog of war
	remove_fog_of_war();
	create_fog_of_war();

	// roomSystem.setRandomObjective(); // hijack for tutorial objectives?
}

// spawn the game entities
void WorldSystem::spawn_game_entities() {

	// Switch between debug and regular room
	std::string next_map = roomSystem.getRandomRoom(Floors::FLOOR1, true);
	//std::string next_map = roomSystem.getRandomRoom(Floors::DEBUG, true);

	spawnData = createTiles(renderer, next_map);

	// create all non-menu game objects
	// spawn the player and enemy in random locations
	spawn_player_random_location(spawnData.playerSpawns);
	spawn_enemies_random_location(spawnData.enemySpawns, spawnData.minEnemies, spawnData.maxEnemies);
	spawn_items_random_location(spawnData.itemSpawns, spawnData.minItems, spawnData.maxItems);
  
	Entity player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);

	// setup turn order system
	turnOrderSystem.setUpTurnOrder();
	// start first turn
	turnOrderSystem.getNextTurn();

	float statbarsX = 150.f * ui_scale;
	float statbarsY = window_height_px - START_BB_HEIGHT - 55.f * ui_scale;
	createHPFill(renderer, { statbarsX, statbarsY });
	createHPBar(renderer,  { statbarsX, statbarsY });
	createMPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createMPBar(renderer,  { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createEPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	createEPBar(renderer,  { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	turnUI = createTurnUI(renderer, { window_width_px*(3.f/4.f), window_height_px*(1.f/16.f)});
	objectiveCounter = createObjectiveCounter(renderer, { 256 * ui_scale, window_height_px * (1.f / 16.f) + 32});
	objectiveDescText = createText(renderer, { 272 * ui_scale, window_height_px * (1.f / 16.f) + 76 * ui_scale }, "", 2.f, { 1.0, 1.0, 1.0 });
	objectiveNumberText = createText(renderer, { 272 * ui_scale, window_height_px * (1.f / 16.f) + 204 * ui_scale }, "", 2.f, { 1.0, 1.0, 1.0 });

	remove_fog_of_war();
	create_fog_of_war();

	// spawn chests beside player
	createChest(renderer, player_motion.position + vec2(-64, 64), false);
	createChest(renderer, player_motion.position + vec2(64, 64), true);
}

// render ep range around the given position
void WorldSystem::create_ep_range(float remaining_ep, float speed, vec2 pos) {
	Stats player_stats = registry.stats.get(player_main);
	float ep_radius = remaining_ep * (1 / player_stats.epratemove) * speed * 0.015 + ((110.f * remaining_ep * (1 / player_stats.epratemove)) / 100);

	Entity ep = createEpRange({ pos.x , pos.y }, ep_resolution, ep_radius, { window_width_px, window_height_px });
	registry.colors.insert(ep, { 0.2, 0.2, 1.f, 1.f });
}

// render attack range around the given position
void WorldSystem::create_attack_range(float range, vec2 pos) {
	// remove previously rendered attack_range
	remove_attack_range();

	Stats player_stats = registry.stats.get(player_main);
	float attack_radius = range;

	Entity ar = createAttackRange({ pos.x , pos.y }, ep_resolution, attack_radius, { window_width_px, window_height_px });
	registry.colors.insert(ar, { 0.0, 1.0, 1.0, 1.0 });
}

// remove all attack ranges
void WorldSystem::remove_attack_range() {
	for (Entity e : registry.attackRange.entities) {
		registry.remove_all_components_of(e);
	}
}

// render fog of war around the player
void WorldSystem::create_fog_of_war() {	
		// get player position
	Motion player_motion = registry.motions.get(player_main);
	Stats player_stats = registry.stats.get(player_main);
	float playerX = player_motion.position.x;
	float playerY = player_motion.position.y;

	Entity fog = createFog({ playerX, playerY }, fog_resolution, player_stats.range, { window_width_px, window_height_px });
	registry.colors.insert(fog, { 0.2, 0.2, 0.2, 1.f });
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
			int roll = irand(4);
			switch (roomSystem.current_floor) {
			case Floors::FLOOR1:
				// Spawn either a slime or PlantShooter or caveling
				if (roll < 1) {
					createCaveling(renderer, { enemySpawns[i].x, enemySpawns[i].y });
				}
				else if (roll < 2) {
					createPlantShooter(renderer, { enemySpawns[i].x, enemySpawns[i].y });
				}
				else {
					createEnemy(renderer, { enemySpawns[i].x, enemySpawns[i].y });
				}
				break;
			case Floors::BOSS1:
				createKingSlime(renderer, { enemySpawns[i].x, enemySpawns[i].y });
				break;
			case Floors::FLOOR2:
				roll = irand(9);
				if (roll < 1) {
					Entity summon = createEnemy(world.renderer, { enemySpawns[i].x, enemySpawns[i].y });
					Stats& summon_stats = registry.basestats.get(summon);
					summon_stats.maxhp = 56;
					summon_stats.atk = 20;
					registry.stats.get(summon).hp = summon_stats.maxhp;
					reset_stats(summon);
					calc_stats(summon);
				}
				else if (roll < 2) {
					Entity summon = createPlantShooter(world.renderer, { enemySpawns[i].x, enemySpawns[i].y });
					Stats& summon_stats = registry.basestats.get(summon);
					summon_stats.maxhp = 48;
					summon_stats.atk = 16;
					registry.stats.get(summon).hp = summon_stats.maxhp;
					reset_stats(summon);
					calc_stats(summon);
				}
				else if (roll < 3) {
					Entity summon = createCaveling(world.renderer, { enemySpawns[i].x, enemySpawns[i].y });
					Stats& summon_stats = registry.basestats.get(summon);
					summon_stats.maxhp = 38;
					summon_stats.atk = 12;
					registry.stats.get(summon).hp = summon_stats.maxhp;
					reset_stats(summon);
					calc_stats(summon);
				}
				else if (roll < 6) {
					createLivingRock(world.renderer, { enemySpawns[i].x, enemySpawns[i].y });
				}
				else {
					createApparition(world.renderer, { enemySpawns[i].x, enemySpawns[i].y });
				}
				break;
			case Floors::BOSS2:
				createReflexion(renderer, { enemySpawns[i].x, enemySpawns[i].y });
				break;
			}
		}
	}
}

// spawn item entities in random locations
void WorldSystem::spawn_items_random_location(std::vector<vec2>& itemSpawns, int min, int max) {
	std::random_shuffle(itemSpawns.begin(), itemSpawns.end());
	if (itemSpawns.size() > 0) {
		int numberToSpawn = std::min(irandRange(min, max + 1), int(itemSpawns.size()));
		Motion& motion = registry.motions.get(player_main);
		int range = 0;
		if (registry.stats.has(player_main)) {
			range = registry.stats.get(player_main).range;
		}

		int spawned = 0;
		int i = 0;
		while (spawned < numberToSpawn && i < itemSpawns.size()) {
			
			switch (roomSystem.current_floor) {
			case Floors::BOSS1:
			case Floors::BOSS2:
				createCampfire(renderer, { itemSpawns[i].x, itemSpawns[i].y });
			default:
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
			}
			spawned++;
			i++;
		}
	}
}

// spawn door entities in random locations
void WorldSystem::spawn_doors_random_location(int quantity, bool has_boss_doors) {
	std::random_shuffle(spawnData.playerSpawns.begin(), spawnData.playerSpawns.end());
	if (spawnData.playerSpawns.size() > 0) {
		Motion& motion = registry.motions.get(player_main);
		int range = 0;
		if (registry.stats.has(player_main)) {
			range = registry.stats.get(player_main).range;
		}

		int spawned = 0;
		int i = 0;
		int boss_doors_to_spawn = has_boss_doors ? irandRange(1, 3) : 0;
		while (spawned < quantity && i < spawnData.playerSpawns.size()) {
			// temporary, later we can also randomize the item types
			if (range > 0) {
				if (dist_to(motion.position, spawnData.playerSpawns[i]) <= range) {
					i++;
					continue;
				}
			}
			if (boss_doors_to_spawn > 0) {
				createDoor(renderer, { spawnData.playerSpawns[i].x, spawnData.playerSpawns[i].y }, true);
				boss_doors_to_spawn--;
			}
			else {
				createDoor(renderer, { spawnData.playerSpawns[i].x, spawnData.playerSpawns[i].y }, false);
			}
			
			spawned++;
			i++;
		}
	}
}

void WorldSystem::spawn_switches_random_location(int quantity) {
	std::random_shuffle(spawnData.enemySpawns.begin(), spawnData.enemySpawns.end());
	if (spawnData.enemySpawns.size() > 0) {
		for (int i = 0; i < quantity; i++) {
			createSwitch(renderer, { spawnData.enemySpawns[i].x, spawnData.enemySpawns[i].y });
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
	// no interactions when being knocked back
	if (registry.knockbacks.has(player_main)) { return; }

	// DEBUG: TRAP
	if (action == GLFW_RELEASE && key == GLFW_KEY_Q) {
		createTrap(world.renderer, player_main, registry.motions.get(player_main).position, {64, 64}, 400, 4, 5, TEXTURE_ASSET_ID::BURRS);
	}

	// DEBUG: HEAL PLAYER
	if (action == GLFW_RELEASE && key == GLFW_KEY_EQUAL) {
		Stats& stat = registry.stats.get(player_main);
		stat.hp = stat.maxhp;
		stat.mp = stat.maxmp;
		stat.ep = stat.maxep;
		registry.players.get(player_main).attacked = false;
	}

	// DEBUG: Testing artifact/stacking
	if (action == GLFW_RELEASE && key == GLFW_KEY_9) {
		int give = (int)ARTIFACT::BURRBAG;
		for (Entity& p : registry.players.entities) {
			Inventory& inv = registry.inventories.get(p);
			inv.artifact[give]++;

			std::string name = artifact_names.at((ARTIFACT)give);
			std::cout << "Artifact given: " << name << " (" << inv.artifact[give] << ")" << std::endl;
			reset_stats(p);
			calc_stats(p);
		}
	}

	// DEBUG: Testing artifact/stacking
	if (action == GLFW_RELEASE && key == GLFW_KEY_0) {
		int give = (int)ARTIFACT::FUNGIFIER;
		for (Entity& p : registry.players.entities) {
			Inventory& inv = registry.inventories.get(p);
			inv.artifact[give]++;

			std::string name = artifact_names.at((ARTIFACT)give);
			std::cout << "Artifact given: " << name << " (" << inv.artifact[give] << ")" << std::endl;
			reset_stats(p);
			calc_stats(p);
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_P) {
		auto& stats = registry.stats.get(player_main);
		auto& player = registry.players.get(player_main);
		//StatusEffect test = StatusEffect(10, 2, StatusType::INVINCIBLE, false, true);
		//apply_status(player_main, test);
		printf("\nPLAYER STATS:\natk: %f\ndef: %f\nspeed: %f\nhp: %f\nmp: %f\nrange: %f\nepmove: %f\nepatk: %f\nfloor: %i\n", stats.atk, stats.def, stats.speed, stats.maxhp, stats.maxmp, stats.range, stats.epratemove, stats.eprateatk, player.floor);
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_O) {
		roomSystem.updateObjective(roomSystem.current_objective.type, 100);
	}

	// SAVING THE GAME
	if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
		if (!tutorial) {
			saveSystem.saveGameState(turnOrderSystem.getTurnOrder(), roomSystem);
			logText("Game state saved!");
		}
	}

	///////////////////////////
	// menu hotkeys
	///////////////////////////
	if (action == GLFW_RELEASE && key == GLFW_KEY_1) {
		// attack
		if (current_game_state == GameStates::BATTLE_MENU) {
			if (tutorial) {
				if (tutorial_flags & SIGN_2) {
					attackAction();
				}
			} else {
				attackAction();
			}
		}
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_2) {
		// move
		if (current_game_state == GameStates::BATTLE_MENU) {
			if (registry.stats.get(player_main).ep <= 0) {
				logText("Cannot move with 0 EP!");
				Mix_PlayChannel(-1, error_sound, 0);
			}
			else {
				if (tutorial) {
					if (tutorial_flags & SIGN_1) {
						moveAction();
					}
				} else {
					moveAction();
				}
			}
		}
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_3) {
		// end turn/ guard
		if (current_game_state == GameStates::BATTLE_MENU) {

			// no ending turn if enemies are currently in the process of dying
			if (registry.squishTimers.size() > 0) {
				world.logText("Please wait...");
				Mix_PlayChannel(-1, error_sound, 0);
				return;
			}

			if (tutorial) {
				if (tutorial_flags & EP_DEPLETED) {
					for (Entity e : registry.guardButtons.entities) {
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
			} else {
				for (Entity e : registry.guardButtons.entities) {
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
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_4) {
		// item
		if (current_game_state == GameStates::BATTLE_MENU) {
			if (tutorial) {
				if (tutorial_flags & SIGN_4) {
					itemAction();
				}
			} else {
				itemAction();
			}
		}
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		if (current_game_state == GameStates::DIALOGUE) {
			return;
		}
		// pause menu
		// close the menu if pressed again
		if (current_game_state != GameStates::MAIN_MENU && current_game_state != GameStates::CREDITS) {
			if (current_game_state == GameStates::CUTSCENE) {
				int w, h;
				glfwGetWindowSize(window, &w, &h);
				if (registry.fadeTransitionTimers.size() == 0) {
					Entity temp = Entity();
					FadeTransitionTimer& timer = registry.fadeTransitionTimers.emplace(temp);
					timer.type = TRANSITION_TYPE::CUTSCENE_TO_MAIN;
				}
			}
			else if (current_game_state == GameStates::PAUSE_MENU) {
				backAction();
			}
			else if (current_game_state == GameStates::ATTACK_MENU || current_game_state == GameStates::MOVEMENT_MENU || current_game_state == GameStates::ITEM_MENU|| current_game_state == GameStates::COLLECTION_MENU) {
				printf("In Attack Menu or Item or Movement and escape to go back to main menu\n");
				backAction();

			}
			else {
				set_gamestate(GameStates::PAUSE_MENU);
				// render save and quit button
				createSaveQuit(renderer, { window_width_px / 2, window_height_px / 2 + 90 * ui_scale});

				// render cancel button
				createCancelButton(renderer, { window_width_px / 2, window_height_px / 2 - 90.f * ui_scale});
			}
		}
	}

	// global end turn
	if (action == GLFW_RELEASE && key == GLFW_KEY_ENTER) {
		if (current_game_state == GameStates::ATTACK_MENU || current_game_state == GameStates::MOVEMENT_MENU || current_game_state == GameStates::ITEM_MENU) {
			backAction();
		}

		// no ending turn if enemies are currently in the process of dying
		if (registry.squishTimers.size() > 0) { 
			world.logText("Please wait...");
			Mix_PlayChannel(-1, error_sound, 0);
			return; 
		}

		if (tutorial) {
			if (tutorial_flags & EP_DEPLETED) {
				Player p = registry.players.get(player_main);
				Stats s = registry.stats.get(player_main);

				if (!p.attacked && !p.moved && s.ep > 50) {
					logText("You brace yourself...");
					registry.stats.get(player_main).guard = true;	
				}
				handle_end_player_turn(player_main);
			}
		}
		else {
			for (Entity e : registry.guardButtons.entities) {
				Player p = registry.players.get(player_main);
				Stats s = registry.stats.get(player_main);

				if (!p.attacked && !p.moved && s.ep > 50) {
					logText("You brace yourself...");
					registry.stats.get(player_main).guard = true;
				}
				handle_end_player_turn(player_main);
			}
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_Q) {
		for (Entity& p : registry.players.entities) {
			StatusEffect test = StatusEffect(20, 5, StatusType::ATK_BUFF, false, true);
			apply_status(p, test);
		}
	}

	// LOADING THE GAME
	if (action == GLFW_RELEASE && key == GLFW_KEY_L && get_is_player_turn() ) {
		// if save data exists reset the game
		if (saveSystem.saveDataExists()) {
			// remove entities to load in entities
			removeForLoad();
			//printf("Removed for load\n");
			// get saved game data
			json gameData = saveSystem.getSaveData();
			//printf("getting gameData\n");
			// load the entities in
			loadFromData(gameData);
			//printf("load game data?\n");
		}

		logText("Game state loaded!");
		remove_fog_of_war();
		create_fog_of_war();
	}

	// simulating a new room
	if (action == GLFW_RELEASE && key == GLFW_KEY_N && get_is_player_turn()) {
		if (!registry.roomTransitions.has(player_main)) {
			if (tutorial) {
				if (!registry.motions.has(objectiveCounter)) {
					objectiveCounter = createObjectiveCounter(renderer, { 256, window_height_px * (1.f / 16.f) + 32});
					objectiveDescText = createText(renderer, { 272, window_height_px * (1.f / 16.f) + 76 }, "", 2.f, { 1.0, 1.0, 1.0 });
					objectiveNumberText = createText(renderer, { 272, window_height_px * (1.f / 16.f) + 204 }, "", 2.f, { 1.0, 1.0, 1.0 });
				}
				tutorial = false;
			}
			RoomTransitionTimer& transition = registry.roomTransitions.emplace(player_main);
			transition.floor = roomSystem.current_floor;
		}
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

	if (action == GLFW_RELEASE) {
		// button sound
		Mix_PlayChannel(-1, ui_click, 0);
	}
}

// On mouse click callback
void WorldSystem::on_mouse(int button, int action, int mod) {

	// no interactions when being knocked back
	if (registry.knockbacks.has(player_main)) { return; }

	// disable input during transitions
	if (registry.roomTransitions.size() > 0) { return; }
	if (registry.fadeTransitionTimers.size() > 0) { return; }

	double xpos, ypos;
	//getting cursor position
	glfwGetCursorPos(window, &xpos, &ypos);
	//printf("Cursor Position at (%f, %f)\n", xpos, ypos);

	// get cursor position relative to world
	Camera camera = registry.cameras.get(active_camera_entity);
	vec2 world_pos = {xpos + camera.position.x, ypos + camera.position.y};


	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {

		// Advance cutscene
		if (!player_move_click && current_game_state == GameStates::CUTSCENE) {
			printf("set to cut scene dialogue\n");
			printf(":%d\n", countCutScene);
			countCutScene++;
			/*if (current_game_state != GameStates::CUTSCENE_DIALOGUE) {

				printf("true not in cut_scene dialoge\n");

			}*/



			cut_scene_start();
			if (current_game_state == GameStates::CUTSCENE && countCutScene == 21){
				// fade to main_menu screen 
				//screen.darken_screen_factor = 0;
				Entity temp = Entity();
				FadeTransitionTimer& timer = registry.fadeTransitionTimers.emplace(temp);
				timer.type = TRANSITION_TYPE::CUTSCENE_TO_MAIN;
				// logic is handled in step() when the timer expires
			}
			return;
		}

		if (current_game_state == GameStates::CREDITS) {
			Entity temp = Entity();
			FadeTransitionTimer& timer = registry.fadeTransitionTimers.emplace(temp);
			timer.type = TRANSITION_TYPE::CREDITS_TO_MAIN;
			return;
		}

		if (current_game_state == GameStates::DIALOGUE) {
			advanceTextbox();
			return;
		}

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
				// button sound
				Mix_PlayChannel(-1, ui_click, 0);

				// perform action based on button ENUM
				BUTTON_ACTION_ID action_taken = registry.buttons.get(e).action_taken;

				switch (action_taken) {
				case BUTTON_ACTION_ID::MENU_START:
					if (registry.fadeTransitionTimers.size() == 0) {
						Entity temp = Entity();
						FadeTransitionTimer& timer = registry.fadeTransitionTimers.emplace(temp);
						timer.type = TRANSITION_TYPE::MAIN_TO_GAME;
					}
					// logic has been moved to when the timer expires in step()
					break;
				case BUTTON_ACTION_ID::CREDITS:
					if (registry.fadeTransitionTimers.size() == 0) {
						Entity temp = Entity();
						FadeTransitionTimer& timer = registry.fadeTransitionTimers.emplace(temp);
						timer.type = TRANSITION_TYPE::MAIN_TO_CREDITS;
					}
					return;
				case BUTTON_ACTION_ID::MENU_QUIT: glfwSetWindowShouldClose(window, true); break;
				case BUTTON_ACTION_ID::CONTINUE:
					// if save data exists reset the game
					if (saveSystem.saveDataExists()) {
						if (registry.fadeTransitionTimers.size() == 0) {
							Entity temp = Entity();
							FadeTransitionTimer& timer = registry.fadeTransitionTimers.emplace(temp);
							timer.type = TRANSITION_TYPE::CONTINUE_TO_GAME;
						}
					}
					break;
				case BUTTON_ACTION_ID::SAVE_QUIT:
					if (!tutorial) {
						saveSystem.saveGameState(turnOrderSystem.getTurnOrder(), roomSystem);
						logText("Game state saved!");
					}
					glfwSetWindowShouldClose(window, true); break;
					break;
				case BUTTON_ACTION_ID::ACTIONS_ATTACK:
					if (current_game_state == GameStates::BATTLE_MENU) {
						attackAction();
					}
					break;
				case BUTTON_ACTION_ID::ACTIONS_MOVE:
					if (registry.stats.get(player_main).ep <= 0) {
						logText("Cannot move with 0 EP!");
						Mix_PlayChannel(-1, error_sound, 0);
						break;
					}
					if (current_game_state == GameStates::BATTLE_MENU) {
						moveAction();
					}
					break;
				case BUTTON_ACTION_ID::PAUSE:
					// TODO: pause enimies if it is their turn

					// inMenu = true;
					set_gamestate(GameStates::PAUSE_MENU);
					// render save and quit button
					createSaveQuit(renderer, { window_width_px / 2, window_height_px / 2 + 90 * ui_scale });

					// render cancel button
					createCancelButton(renderer, { window_width_px / 2, window_height_px / 2 - 90.f * ui_scale });

					return;
				case BUTTON_ACTION_ID::ACTIONS_CANCEL:
					cancelAction();
					return;
				case BUTTON_ACTION_ID::COLLECTION:
					// if the button is pressed again while the menu is already open, close the menu
					if (current_game_state == GameStates::COLLECTION_MENU) {
						set_gamestate(GameStates::BATTLE_MENU);
					}
					else {
						// render the collection menu
						createCollectionMenu(renderer, vec2(window_width_px / 2, window_height_px / 2 - 40.f * ui_scale), player_main);
						set_gamestate(GameStates::COLLECTION_MENU);
					}
					return;
				case BUTTON_ACTION_ID::ACTIONS_BACK:
					if (current_game_state != GameStates::PAUSE_MENU && current_game_state != GameStates::COLLECTION_MENU) {
						backAction();
					}
					return;
				case BUTTON_ACTION_ID::ACTIONS_ITEM:
					if (current_game_state == GameStates::BATTLE_MENU) {
						itemAction();
					}
					return;
				case BUTTON_ACTION_ID::OPEN_DIALOG:
					// remove all other description dialog components
					for (Entity dd : registry.descriptionDialogs.entities) {
						registry.remove_all_components_of(dd);
					}

					// get which icon was clicked
					if (registry.artifactIcons.has(e)) {
						ARTIFACT artifact = registry.artifactIcons.get(e).artifact;
						createDescriptionDialog(renderer, vec2(window_width_px / 2, window_height_px / 2 - 50.f * ui_scale), artifact);
					}
					return;
				case BUTTON_ACTION_ID::CLOSE_DIALOG:
					// remove all description dialog components
					for (Entity dd : registry.descriptionDialogs.entities) {
						registry.remove_all_components_of(dd);
					}
					return;
				case BUTTON_ACTION_ID::OPEN_ATTACK_DIALOG:
					// remove all other attack dialog components
					for (Entity ad : registry.attackDialogs.entities) {
						registry.remove_all_components_of(ad);
					}

					// get which icon was clicked
					if (registry.attackCards.has(e)) {
						ATTACK attack = registry.attackCards.get(e).attack;
						registry.players.get(player_main).selected_attack = attack;
						createAttackDialog(renderer, vec2(window_width_px / 2, window_height_px / 2 - 50.f * ui_scale), attack, registry.players.get(player_main).prepared);
					}
					return;
				case BUTTON_ACTION_ID::CLOSE_ATTACK_DIALOG:
					// remove all attack dialog components
					for (Entity ad : registry.attackDialogs.entities) {
						registry.remove_all_components_of(ad);
					}
					return;
				case BUTTON_ACTION_ID::OPEN_EQUIPMENT_DIALOG:
					// remove all other eqiupment dialog components
					for (Entity ed : registry.equipmentDialogs.entities) {
						registry.remove_all_components_of(ed);
					}

					// get which icon was clicked
					if (registry.itemCards.has(e)) {
						Equipment equipment = registry.itemCards.get(e).item;
						createEquipmentDialog(renderer, vec2(window_width_px / 2, window_height_px / 2 - 50.f * ui_scale), equipment);
					}
					break;
				case BUTTON_ACTION_ID::CLOSE_EQUIPMENT_DIALOG:
					// remove all equipment dialog components
					for (Entity ed : registry.equipmentDialogs.entities) {
						registry.remove_all_components_of(ed);
					}
					break;
				case BUTTON_ACTION_ID::USE_ATTACK:
					registry.players.get(player_main).using_attack = registry.players.get(player_main).selected_attack;
					for (Entity ad : registry.attackDialogs.entities) {
						registry.remove_all_components_of(ad);
					}
					return;
				case BUTTON_ACTION_ID::PREPARE_ATTACK:
					Player& p = registry.players.get(player_main);
					Stats& stats = registry.stats.get(player_main);
					if (p.attacked) {
						logText("You already attacked this turn!");
						Mix_PlayChannel(-1, error_sound, 0);
					}
					else if (stats.mp < attack_mpcosts.at(p.selected_attack) || stats.ep < attack_epcosts.at(p.selected_attack) * stats.eprateatk) {
						logText("Not enough MP or EP to attack!");
						Mix_PlayChannel(-1, error_sound, 0);
					}
					else {
						stats.mp -= attack_mpcosts.at(p.selected_attack);
						stats.ep -= attack_epcosts.at(p.selected_attack) * stats.eprateatk;
						registry.players.get(player_main).prepared = true;
						for (Entity ad : registry.attackDialogs.entities) {
							registry.remove_all_components_of(ad);
						}
						cancelAction();
					}
					return;
				}
			}
		}

		///////////////////////////
		// logic for guard button presses
		///////////////////////////
		if (current_game_state == GameStates::BATTLE_MENU) {
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
					// no ending turn if enemies are currently in the process of dying
					if (registry.squishTimers.size() > 0) {
						world.logText("Please wait...");
						Mix_PlayChannel(-1, error_sound, 0);
						return;
					}
					else {
						// button sound
						Mix_PlayChannel(-1, ui_click, 0);
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
		if (get_is_player_turn() && !player_move_click && xpos < window_width_px - 200.f && ypos > 100.f) {
			if (player_main && current_game_state >= GameStates::GAME_START && current_game_state != GameStates::CUTSCENE) {
				Player& player = registry.players.get(player_main);
				Motion& player_motion = registry.motions.get(player_main);
				Stats& player_stats = registry.stats.get(player_main);

				switch (current_game_state) {

				case GameStates::ATTACK_MENU:
					use_attack(world_pos);
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
						motion_struct.destination = { world_pos.x, world_pos.y };
						motion_struct.in_motion = true;
						player_move_click = true;
						player.moved = true;

						// set walk animation
						if (!registry.animations.has(player_main)) {
							AnimationData& anim = registry.animations.emplace(player_main);
							anim.spritesheet_texture = TEXTURE_ASSET_ID::PLAYER_SHEET;
							anim.frametime_ms = 80;
							anim.frame_indices = { 0, 1, 2, 3 };
							anim.spritesheet_columns = 4;
							anim.spritesheet_rows = 1;
							anim.spritesheet_width = 128;
							anim.spritesheet_height = 32;
							anim.frame_size = { anim.spritesheet_width / anim.spritesheet_columns, anim.spritesheet_height / anim.spritesheet_rows };
							registry.renderRequests.get(player_main).used_texture = TEXTURE_ASSET_ID::PLAYER_SHEET;
							registry.renderRequests.get(player_main).used_geometry = GEOMETRY_BUFFER_ID::ANIMATION;
						}
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
								interactable.interacted = true;
							}
							// Chest behaviour
							else if (interactable.type == INTERACT_TYPE::ARTIFACT_CHEST && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								interactable.interacted = true;
								Chest& chest = registry.chests.get(entity);
								if (chest.opened) {
									continue;
								}
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
								
								createArtifact(renderer, motion.position + vec2(16, 16), (ARTIFACT)loot);

								std::string name = artifact_names.at((ARTIFACT)loot);
								logText("You open the chest and find " + name + "!");

								chest.opened = true;
								chest.needs_retexture = true;
								Mix_PlayChannel(-1, chest_sound, 0);
								break;
							}
							else if (interactable.type == INTERACT_TYPE::ITEM_CHEST && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								interactable.interacted = true;
								Chest& chest = registry.chests.get(entity);
								if (chest.opened) {
									continue;
								}
								
								Player player = registry.players.get(player_main);
								EQUIPMENT type;

								// choose equipment
								if (ichoose(0, 1)) {
									type = EQUIPMENT::SHARP;
								}
								else {
									type = EQUIPMENT::ARMOUR;
								}

								// If player has no weapon, give them a weapon
								if (registry.inventories.get(player_main).equipped[0].attacks[1] == ATTACK::NONE) {
									type = EQUIPMENT::SHARP;
								}

								Equipment equip = createEquipment(type, player.floor);
								createEquipmentEntity(renderer, motion.position + vec2(16,16), equip);

								logText("You open the chest and find some equipment!");

								chest.opened = true;
								chest.needs_retexture = true;
								Mix_PlayChannel(-1, chest_sound, 0);
								break;
							}
							// Pickup item behaviour
							else if (interactable.type == INTERACT_TYPE::PICKUP && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								interactable.interacted = true;
								break;
							}
							// Door Behaviour
							else if (interactable.type == INTERACT_TYPE::DOOR && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								interactable.interacted = true;
								if (!registry.roomTransitions.has(player_main)) {
									RoomTransitionTimer& transition = registry.roomTransitions.emplace(player_main);
									transition.floor = roomSystem.current_floor;
								}
							}
							// Boss Door Behaviour
							else if (interactable.type == INTERACT_TYPE::BOSS_DOOR && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								if (!registry.roomTransitions.has(player_main)) {
									RoomTransitionTimer& transition = registry.roomTransitions.emplace(player_main);
									transition.floor = Floors((int)roomSystem.current_floor + 1);
									if (transition.floor == Floors::FLOOR1 || transition.floor == Floors::BOSS1) {
										playMusic(Music::BACKGROUND);
									}
									if (transition.floor == Floors::FLOOR2 || transition.floor == Floors::BOSS2) {
										playMusic(Music::RUINS);
									}
									roomSystem.setNextFloor(transition.floor);
								}
							}
							// Switch Behaviour
							else if (interactable.type == INTERACT_TYPE::SWITCH && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								interactable.interacted = true;
								Switch& switch_component = registry.switches.get(entity);
								if (!switch_component.activated) {
									switch_component.activated = true;
									RenderRequest& rr = registry.renderRequests.get(entity);
									rr.used_texture = TEXTURE_ASSET_ID::SWITCH_ACTIVE;
									Mix_PlayChannel(-1, switch_sound, 0);
									roomSystem.updateObjective(ObjectiveType::ACTIVATE_SWITCHES, 1);
									break;
								}
							}
							else if (interactable.type == INTERACT_TYPE::CAMPFIRE && dist_to(registry.motions.get(player_main).position, motion.position) <= 100) {
								if (!interactable.interacted) {
									Stats& stats = registry.stats.get(player_main);
									heal(player_main, stats.maxhp);
									stats.mp = stats.maxmp;
									// Guide to Healthy Eating
									if (registry.inventories.get(player_main).artifact[(int)ARTIFACT::GUIDE_HEALBUFF] > 0) {
										StatusEffect buff = StatusEffect(0.2 * registry.inventories.get(player_main).artifact[(int)ARTIFACT::GUIDE_HEALBUFF], 5, StatusType::ATK_BUFF, true, true);
										if (has_status(player_main, StatusType::ATK_BUFF)) { remove_status(player_main, StatusType::ATK_BUFF); }
										apply_status(player_main, buff);
									}
									interactable.interacted = true;
									break;
								}
							}
							else if (interactable.type == INTERACT_TYPE::SIGN_2 && dist_to(registry.motions.get(player_main).position, motion.position) <= 150) {
								interactable.interacted = true;
								if (registry.signs2.has(entity)) {
									Sign2& sign = registry.signs2.get(entity);
									activeTextbox = createTextbox(renderer, vec2(window_width_px/2.f, window_height_px/2.f), sign.messages);
									set_gamestate(GameStates::DIALOGUE);
								}
							}
						}
					}
				}
			}
		}
	}


	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE && get_is_player_turn() && !player_move_click && current_game_state >= GameStates::GAME_START && current_game_state != GameStates::CUTSCENE) {
		if (current_game_state == GameStates::DIALOGUE) {
			return;
		}
		if (current_game_state == GameStates::ATTACK_MENU || current_game_state == GameStates::MOVEMENT_MENU || current_game_state == GameStates::ITEM_MENU) {
			backAction();
		}
		// close all dialogs and menus
		// remove all description dialog components
		for (Entity dd : registry.descriptionDialogs.entities) {
			registry.remove_all_components_of(dd);
		}
		// remove all attack dialog components
		for (Entity ad : registry.attackDialogs.entities) {
			registry.remove_all_components_of(ad);
		}
		cancelAction();
	}

}

void WorldSystem::start_game() {
	set_gamestate(GameStates::BATTLE_MENU);
	if (current_game_state != GameStates::CUTSCENE || current_game_state != GameStates::MAIN_MENU) {
		playMusic(Music::BACKGROUND);
	}
	// spawn the actions bar
	// createActionsBar(renderer, { window_width_px / 2, window_height_px - 100.f });
	createAttackButton(renderer, { window_width_px - 125.f, 200.f });
	createMoveButton(renderer, { window_width_px - 125.f, 350.f });
	createGuardButton(renderer, { window_width_px - 125.f, 500.f }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD);
	createItemButton(renderer, { window_width_px - 125.f, 650.f });

	// spawn the collection and pause buttons
	createPauseButton(renderer, { window_width_px - 80.f, 50.f });
	createCollectionButton(renderer, { window_width_px - 160.f, 50.f });

	for (int i = registry.renderRequests.size() - 1; i >= 0; i--) {
		if (registry.renderRequests.components[i].used_layer == RENDER_LAYER_ID::BG) {
			registry.remove_all_components_of(registry.renderRequests.entities[i]);
		}
	}
	is_player_turn = true;
	background = createGameBackground(renderer, { 0.f, 0.f }, TEXTURE_ASSET_ID::CAVE_COLOR, RENDER_LAYER_ID::BG);
	background_back = createGameBackground(renderer, { 0.f, 0.f }, TEXTURE_ASSET_ID::CAVE_BACK, RENDER_LAYER_ID::BG_1);
	background_mid = createGameBackground(renderer, { 0.f, 0.f }, TEXTURE_ASSET_ID::CAVE_MID, RENDER_LAYER_ID::BG_2);
	background_front = createGameBackground(renderer, { 0.f, 0.f }, TEXTURE_ASSET_ID::CAVE_FRONT, RENDER_LAYER_ID::BG_3);

	float statbarsX = 150.f;
	float statbarsY = window_height_px - START_BB_HEIGHT - 55.f;
	createHPFill(renderer, { statbarsX, statbarsY });
	createHPBar(renderer, { statbarsX, statbarsY });
	createMPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createMPBar(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createEPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	createEPBar(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	
	turnUI = createTurnUI(renderer, { window_width_px * (3.f / 4.f), window_height_px * (1.f / 16.f) });
	objectiveCounter = createObjectiveCounter(renderer, { 256, window_height_px * (1.f / 16.f) + 32});
	objectiveDescText = createText(renderer, { 272, window_height_px * (1.f / 16.f) + 76 }, "", 2.f, { 1.0, 1.0, 1.0 });
	objectiveNumberText = createText(renderer, { 272, window_height_px * (1.f / 16.f) + 204 }, "", 2.f, { 1.0, 1.0, 1.0 });
}

void WorldSystem::start_tutorial() {
	// set flags
	tutorial_flags = 0x0;

	set_gamestate(GameStates::BATTLE_MENU);
	if (current_game_state != GameStates::CUTSCENE || current_game_state != GameStates::MAIN_MENU) {
		playMusic(Music::BACKGROUND);
	}
	// spawn the actions bar
	// createActionsBar(renderer, { window_width_px / 2, window_height_px - 100.f });
	// createAttackButton(renderer, { window_width_px - 125.f, 200.f });
	// createMoveButton(renderer, { window_width_px - 125.f, 350.f });
	// createGuardButton(renderer, { window_width_px - 125.f, 500.f }, BUTTON_ACTION_ID::ACTIONS_GUARD, TEXTURE_ASSET_ID::ACTIONS_GUARD);
	// createItemButton(renderer, { window_width_px - 125.f, 650.f });

	// spawn the collection and pause buttons
	createPauseButton(renderer, { window_width_px - 80.f, 50.f });
	createCollectionButton(renderer, { window_width_px - 160.f, 50.f });

	for (int i = registry.renderRequests.size() - 1; i >= 0; i--) {
		if (registry.renderRequests.components[i].used_layer == RENDER_LAYER_ID::BG) {
			registry.remove_all_components_of(registry.renderRequests.entities[i]);
		}
	}
	is_player_turn = true;
	background = createGameBackground(renderer, { 0.f, 0.f }, TEXTURE_ASSET_ID::CAVE_COLOR, RENDER_LAYER_ID::BG);
	background_back = createGameBackground(renderer, { 0.f, 0.f }, TEXTURE_ASSET_ID::CAVE_BACK, RENDER_LAYER_ID::BG_1);
	background_mid = createGameBackground(renderer, { 0.f, 0.f }, TEXTURE_ASSET_ID::CAVE_MID, RENDER_LAYER_ID::BG_2);
	background_front = createGameBackground(renderer, { 0.f, 0.f }, TEXTURE_ASSET_ID::CAVE_FRONT, RENDER_LAYER_ID::BG_3);

	float statbarsX = 150.f;
	float statbarsY = window_height_px - START_BB_HEIGHT - 55.f * ui_scale;
	createHPFill(renderer, { statbarsX, statbarsY });
	createHPBar(renderer, { statbarsX, statbarsY });
	createMPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createMPBar(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT });
	createEPFill(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	createEPBar(renderer, { statbarsX, statbarsY + STAT_BB_HEIGHT * 2 });
	
	turnUI = createTurnUI(renderer, { window_width_px * (3.f / 4.f), window_height_px * (1.f / 16.f) });
	// objectiveCounter = createObjectiveCounter(renderer, { 256, window_height_px * (1.f / 16.f) + 32});
	// objectiveDescText = createText(renderer, { 272, window_height_px * (1.f / 16.f) + 76 }, "", 2.f, { 1.0, 1.0, 1.0 });
	// objectiveNumberText = createText(renderer, { 272, window_height_px * (1.f / 16.f) + 204 }, "", 2.f, { 1.0, 1.0, 1.0 });
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
	Player& p = registry.players.get(player_main);
	Inventory& inv = registry.inventories.get(player_main);
	p.attacked = false;
	p.moved = false;
	p.prepared = false;

	if (registry.stats.get(player_main).guard) {
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

	// handle traps
	handle_traps();

	// Burrbag effect
	if (inv.artifact[(int)ARTIFACT::BURRBAG] > 0) {
		int triggers = inv.artifact[(int)ARTIFACT::BURRBAG];
		createTrap(world.renderer, player_main, registry.motions.get(player_main).position, {64, 64}, 40, 4, triggers, TEXTURE_ASSET_ID::BURRS);
	}

	// Lively Bulb effect
	if (inv.artifact[(int)ARTIFACT::LIVELY_BULB] > 0) {
		Motion& player_motion = registry.motions.get(player_main);
		int stacks = inv.artifact[(int)ARTIFACT::LIVELY_BULB];
		float frac = 999;
		float dir = 0;
		bool valid = false;

		// choose target
		for (Entity& e : registry.enemies.entities) {
			if (registry.hidden.has(e)) { continue; }
			Motion& enemy_motion = registry.motions.get(e);
			Stats& enemy_stats = registry.stats.get(e);
			if (enemy_stats.hp / enemy_stats.maxhp < frac) {
				valid = true;
				dir = atan2(enemy_motion.position.y - player_motion.position.y, enemy_motion.position.x - player_motion.position.x);
			}
		}

		if (valid) {
			// fire
			createProjectile(renderer, player_main, dirdist_extrapolate(player_motion.position, dir, 64), { 16, 16 }, dir, 80 * stacks, TEXTURE_ASSET_ID::PLANT_PROJECTILE);
		}
	}
}

void WorldSystem::removeForLoad() {
	// empty queue
	turnOrderSystem.emptyQueue();

	// remove player for loading
	registry.remove_all_components_of(player_main);

	// remove enemies
	for (Entity enemy : registry.enemies.entities) {
		if (registry.enemyHPBars.has(enemy)) {
			EnemyHPBar& hpBar = registry.enemyHPBars.get(enemy);
		}
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
	
	// remove attack indicators
	for (Entity indicator : registry.attackIndicators.entities) {
		registry.remove_all_components_of(indicator);
	}

	// remove enemy hp bars/fills
	for (Entity hpdisplay : registry.hpDisplays.entities) {
		registry.remove_all_components_of(hpdisplay);
	}

	// remove shadows
	for (Entity shadow : registry.shadows.entities) {
		registry.remove_all_components_of(shadow);
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

	// remove interactables
	for (Entity interactable : registry.interactables.entities) {
		registry.remove_all_components_of(interactable);
	}

	// remove attack indicators
	for (Entity indicator : registry.attackIndicators.entities) {
		registry.remove_all_components_of(indicator);
	}

	// remove enemy hp bars/fills
	for (Entity hpdisplay : registry.hpDisplays.entities) {
		registry.remove_all_components_of(hpdisplay);
	}

	// remove shadows
	for (Entity shadow : registry.shadows.entities) {
		registry.remove_all_components_of(shadow);
	}
}

void WorldSystem::loadFromData(json data) {
	if (data["music"] != nullptr) {
		playMusic(data["music"]);
	}
	tutorial = false;

	// load player
	json entityList = data["entities"];
	json collidablesList = data["map"]["collidables"];
	json interactablesList = data["map"]["interactables"];
	json tilesList = data["map"]["tiles"];
	json roomSystemJson = data["room"];
	json attackIndicatorList = data["attack_indicators"];
	json traplist = data["trapEntities"];

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
	// load room system
	loadRoomSystem(roomSystemJson);
	// load attack indicators
	loadAttackIndicators(attackIndicatorList);
	// load traps in game 
	loadTraps(traplist);
}

Entity WorldSystem::loadPlayer(json playerData) {
	// create a player from the save data
	// create player
	Entity e = createPlayer(renderer, { 0, 0 });

	// load motion
	loadMotion(e, playerData["motion"]);

	// get queueable stuff
	loadQueueable(e, playerData["queueable"]);

	// get inventory
	Inventory inv = loadInventory(e, playerData["inventory"]);

	// load player statuses
	loadStatuses(e, playerData["statuses"]);

	// load stats
	loadStats(e, playerData["stats"]);

	// get player component stuff
	loadPlayerComponent(e, playerData["player"], inv);

	return e;
}

// loading artifact collection onto the title screen
Inventory WorldSystem::loadPlayerCollectionTitleScreen(json playerData, float floor_width, float floor_height) {

	
	Inventory inv;
	json inventoryData = playerData["inventory"];
	printf("YES ???? we are finally in collection loading \n");
	std::vector<vec2> arifact_type_num; 

	
	//printf("column :%d \n", column);
	// Max height for spawning 
	// width + moves it right 
	// height + moves it down the screen 
	float max_height_top = 600.f; // less
	float max_height_bot = window_height_px*ui_scale - 72.f; // greater
	float h_diff = max_height_bot - max_height_top; // less

	float max_width_left_edge = window_width_px / 3 + 40.f;
	float max_width_right_edge = window_width_px * (2/3); // greater
	float w_diff =  max_width_right_edge- max_width_left_edge;

	
	printf("max_w left :%fl\n", max_width_left_edge);
	printf("max_w right :%fl\n", max_width_right_edge);

	printf("max height top :%fl\n", max_height_top);
	printf("max height bot :%fl\n", max_height_bot);

	// copy of the inventory - make sure it's copy by duplicate and not reference 
	// count up the total nuumer of artifacts in there 
	// while >0 
	// irand range random x and y position (feed the boundaries into i rand range)
	// 2 of artifact want two sitting on the floor 
	// decrement total artifacts by 1 
	// get json obj for inventory saved in saveData

	// get artifacts
	int artifact[static_cast<int>(ARTIFACT::ARTIFACT_COUNT)];
	int i = 0;
	int count_total_artifacts = 0;

	// looping the list of artifacts 
	// int i gives the artifact type 
	// artifact gives the number of artifact the player has 
	for (auto& artifact : inventoryData["artifact"]) {

		inv.artifact[i] = artifact;
		if (artifact > 0) {

			int num_cur_artifacts = static_cast<int>(inv.artifact[i]);

			count_total_artifacts += num_cur_artifacts;
			printf("count of artifacts :%d\n", count_total_artifacts);
			
			while (num_cur_artifacts> 0) {
				int pos_x = 0;
				int pos_y = 0;
				int attempts = 20;
				bool valid = true;

				Entity icon = createArtifactIcon(renderer, vec2(pos_x, pos_y),
					static_cast<ARTIFACT>(i));
				Motion& icon_motion = registry.motions.get(icon);
				icon_motion.angle = irandRange(-450, 450) * (M_PI / 1800);

				while (attempts > 0) {
					icon_motion.position.x = irandRange(max_width_left_edge, max_width_right_edge);
					icon_motion.position.y = irandRange(max_height_top, max_height_bot);

					for (Entity e : registry.artifactIcons.entities) {
						if (collides_circle(registry.motions.get(e), registry.motions.get(icon))) {
							valid = false;
							break;
						}
					}
					if (valid) {
						break;
					}
					attempts--;
				}
				num_cur_artifacts--;
			}
		}
		i++;
	}

	// I don't need to return inventory, just need to check (1) if weapon/ artifact exist if yes 
	// render the stupid sprite

	printf("done QQ \n");
	printf("number of total artifacts :%d\n", count_total_artifacts);
	return inv;


}

int WorldSystem::calculate_abs_value(float v1, float v2) {
	
	return abs(v2 - v1);
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
		e = createKingSlime(renderer, { 0, 0 });
		loadBoss(e, enemyData["boss"]);
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
}

void WorldSystem::loadPlayerComponent(Entity e, json playerCompData, Inventory inv) {
	registry.players.get(e).attacked = playerCompData["attacked"];
	registry.players.get(e).gacha_pity = playerCompData["gacha_pity"];
	registry.players.get(e).floor = playerCompData["floor"];
	registry.players.get(e).room = playerCompData["room"];
	registry.players.get(e).total_rooms = playerCompData["total_rooms"];
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
	weapon.sprite = weaponJson["sprite"];
	i = 0;
	for (auto& attack : weaponJson["attacks"]) {
		weapon.attacks[i] = attack;
		i++;
	}

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
	armour.sprite = armourJson["sprite"];
	i = 0;
	for (auto& attack : armourJson["attacks"]) {
		armour.attacks[i] = attack;
		i++;
	}
	
	equip_item(e, weapon);
	equip_item(e, armour);
	inv.equipped[0] = weapon;
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
		
		ParticleEmitter emitter;
		bool add_emitter = false;
		switch (effect) {
		case StatusType::POISON:
		case StatusType::FANG_POISON:
			emitter = setupParticleEmitter(PARTICLE_TYPE::POISON);
			add_emitter = true;
			break;
		case StatusType::ATK_BUFF:
			if (value < 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::ATK_DOWN);
				add_emitter = true;
			}
			else if (value > 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::ATK_UP);
				add_emitter = true;
			}
			break;
		case StatusType::RANGE_BUFF:
			if (value < 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::RANGE_DOWN);
				add_emitter = true;
			}
			else if (value > 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::RANGE_UP);
				add_emitter = true;
			}
			break;
		case StatusType::SLIMED:
			emitter = setupParticleEmitter(PARTICLE_TYPE::SLIMED);
			add_emitter = true;
		case StatusType::STUN:
			emitter = setupParticleEmitter(PARTICLE_TYPE::STUN);
			add_emitter = true;
			break;
		case StatusType::INVINCIBLE:
			emitter = setupParticleEmitter(PARTICLE_TYPE::INVINCIBLE);
			add_emitter = true;
			break;
		case StatusType::HP_REGEN:
			emitter = setupParticleEmitter(PARTICLE_TYPE::HP_REGEN);
			add_emitter = true;
			break;
		default:
			break;
		}
		if (add_emitter) {
			if (!registry.particleContainers.has(e)) {
				ParticleContainer& particleContainer = registry.particleContainers.emplace(e);
				particleContainer.emitters.push_back(emitter);
			}
			else {
				ParticleContainer& particleContainer = registry.particleContainers.get(e);
				particleContainer.emitters.push_back(emitter);
			}
		}
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
		EFFECT_ASSET_ID::TILE,
		GEOMETRY_BUFFER_ID::SPRITE,
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

// TODO tutorial: save/load new signs
void WorldSystem::loadInteractables(json interactablesList) {
	for (auto& interactable : interactablesList) {
		Entity e = Entity();

		registry.motions.emplace(e);
		loadMotion(e, interactable["motion"]);

		Interactable& interact_component = registry.interactables.emplace(e);
		interact_component.type = (INTERACT_TYPE)interactable["type"];
		interact_component.interacted = interactable["interacted"] == nullptr ? false : interactable["interacted"];
		//interact_component.interacted = interactable["interacted"];

		switch (interact_component.type) {
		case INTERACT_TYPE::ARTIFACT_CHEST: // artifact chest
			loadChest(e, interactable["chest"]);
			break;
		case INTERACT_TYPE::ITEM_CHEST: // item chest
			loadChest(e, interactable["chest"]);
			break;
		case INTERACT_TYPE::DOOR: // door
			loadDoor(e);
			break;
		case INTERACT_TYPE::BOSS_DOOR:
			loadBossDoor(e);
			break;
		case INTERACT_TYPE::STAIRS: // stairs
			break;
		case INTERACT_TYPE::SIGN: // sign
			loadSign(e, interactable["sign"]);
			break;
		case INTERACT_TYPE::SWITCH: // switch
			loadSwitch(e, interactable["switch"]);
			break;
		case INTERACT_TYPE::PICKUP: // equipment drop
			if (interactable["artifact"] != nullptr) {
				loadArtifact(e, interactable["artifact"]);
			}
			else if (interactable["equipment"] != nullptr) {
				loadEquipmentEntity(e, interactable["equipment"], interactable["spritesheet"]);
			}
			else if (interactable["consumable"] != nullptr) {
				loadConsumable(e, interactable["consumable"]);
			}
			break;
		case INTERACT_TYPE::CAMPFIRE:
			loadCampfire(e);
			break;
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

void WorldSystem::loadTraps(json trapList) {
	//Entity createTrap(RenderSystem* renderer, Entity owner, vec2 pos, vec2 scale, float multiplier, int turns, int triggers, TEXTURE_ASSET_ID texture);
	// remove all traps 
	printf("we are loading traps\n");
	while (registry.traps.entities.size() > 0)
		registry.remove_all_components_of(registry.traps.entities.back());

	json trap = trapList["traps"];
	if (trap.size() > 0) {
		printf("there is smt store there in the json\n");
		for (auto& traps : trap) {
			Entity entity = Entity();
			Entity p;
			json mData = traps["motions"];
			Motion& m = registry.motions.emplace(entity);
			printf("we got the motions for traps\n");
			m.scale = { mData["scale"]["x"], mData["scale"]["y"] };
			m.position = { mData["position_x"], mData["position_y"] };
			printf("we set the motions for scale & position\n");
			float multipler = traps["multiplier"];
			printf("we got the multiplier:%fl \n", multipler);
			int turns = traps["trap_turns"];
			printf("we got the trap turns :%d \n", turns);
			int triggers = traps["triggers"];
			printf("we got the trigger:%d \n", triggers);
			TEXTURE_ASSET_ID texture = traps["type"];
			printf("the artifact type is:%d", int(texture));
			
			printf("got the data to create traps again\n");
			if (traps["owner"] == "player") {
				printf("loading the traps back to page \n");
				p = player_main;
				createTrap(renderer, p, m.position, m.scale, multipler, turns, triggers, texture);
			}
			else if (traps["owner"] == "enemy") {
				for (Entity e : registry.enemies.entities) {
					if (registry.enemies.has(e)) {
						p = e;
						break;
					}
				}
				createTrap(renderer, p, m.position, m.scale, multipler, turns, triggers, texture);
				printf("loading trap back to page enemy owned\n");
			}
			if (traps["colors"] != nullptr) { // if has color insert it 
				printf("it has colors");
				registry.colors.insert(p, vec4(traps["colors"]["x"], traps["colors"]["y"], traps["colors"]["z"], traps["colors"]["w"]));
			}

		}
	}


}
void WorldSystem::loadChest(Entity e, json chestData) {
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(e, &mesh);

	Chest& chest = registry.chests.emplace(e);
	chest.isArtifact = chestData["isArtifact"];
	chest.opened = chestData["opened"];

	RenderRequest& rr = registry.renderRequests.emplace(e);
	rr.used_effect = EFFECT_ASSET_ID::TEXTURED;
	rr.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	rr.used_layer = RENDER_LAYER_ID::FLOOR_DECO;
	if (chest.isArtifact) {
		if (chest.opened) {
			rr.used_texture = TEXTURE_ASSET_ID::CHEST_ARTIFACT_OPEN;
		}
		else {
			rr.used_texture = TEXTURE_ASSET_ID::CHEST_ARTIFACT_CLOSED;
		}
	}
	else {
		if (chest.opened) {
			rr.used_texture = TEXTURE_ASSET_ID::CHEST_ITEM_OPEN;
		}
		else {
			rr.used_texture = TEXTURE_ASSET_ID::CHEST_ITEM_CLOSED;
		}
	}
	registry.hidables.emplace(e);
}

void WorldSystem::loadDoor(Entity e) {
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(e, &mesh);
	registry.solid.emplace(e);

	registry.renderRequests.insert(
		e,
		{ TEXTURE_ASSET_ID::DOOR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
}

void WorldSystem::loadBossDoor(Entity e) {
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(e, &mesh);
	registry.solid.emplace(e);
	registry.colors.insert(e, vec4(1, 0.4, 0.4, 1.f));

	registry.renderRequests.insert(
		e,
		{ TEXTURE_ASSET_ID::DOOR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
}

void WorldSystem::loadSwitch(Entity e, json switchData) {
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(e, &mesh);

	Switch& switch_component = registry.switches.emplace(e);
	switch_component.activated = switchData["activated"];

	RenderRequest rr = { TEXTURE_ASSET_ID::SWITCH_DEFAULT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::FLOOR_DECO };
	if (switch_component.activated) {
		rr.used_texture = TEXTURE_ASSET_ID::SWITCH_ACTIVE;
	}
	registry.renderRequests.insert(e, rr);
}

void WorldSystem::loadEquipmentEntity(Entity e, json equipData, json spritesheetData) {
	Equipment& equip = registry.equipment.emplace(e);
	equip.type = equipData["type"];
	equip.atk = equipData["atk"];
	equip.def = equipData["def"];
	equip.speed = equipData["speed"];
	equip.ep = equipData["ep"];
	equip.mp = equipData["mp"];
	equip.hp = equipData["hp"];
	equip.range = equipData["range"];
	equip.sprite = equipData["sprite"];
	int i = 0;
	for (auto atk : equipData["attacks"]) {
		equip.attacks[i] = (ATTACK)atk;
		i++;
	}

	Spritesheet& ss = registry.spritesheets.emplace(e);
	ss.texture = spritesheetData["texture"];
	ss.height = spritesheetData["height"];
	ss.width = spritesheetData["width"];
	ss.columns = spritesheetData["columns"];
	ss.rows = spritesheetData["rows"];
	ss.frame_size = { spritesheetData["frame_size"]["x"], spritesheetData["frame_size"]["y"] };
	ss.index = spritesheetData["index"];

	registry.renderRequests.insert(
		e,
		{ TEXTURE_ASSET_ID::EQUIPMENT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITESHEET });
	registry.hidables.emplace(e);
}

void WorldSystem::loadArtifact(Entity e, json artifactData) {
	registry.artifacts.insert(e, { artifactData["type"]});

	TEXTURE_ASSET_ID sprite = artifact_textures.at(artifactData["type"]);

	registry.renderRequests.insert(
		e,
		{ sprite,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(e);
}

void WorldSystem::loadConsumable(Entity e, json consumableData) {
	registry.consumables.insert(e, { consumableData["type"] });

	RenderRequest& rr = registry.renderRequests.emplace(e);
	rr.used_effect = EFFECT_ASSET_ID::TEXTURED;
	rr.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	rr.used_layer = RENDER_LAYER_ID::SPRITE;
	switch ((CONSUMABLE)consumableData["type"]) {
	case CONSUMABLE::REDPOT:
		rr.used_texture = TEXTURE_ASSET_ID::POTION_RED;
		break;
	case CONSUMABLE::BLUPOT:
		rr.used_texture = TEXTURE_ASSET_ID::POTION_BLUE;
		break;
	case CONSUMABLE::YELPOT:
		rr.used_texture = TEXTURE_ASSET_ID::POTION_YELLOW;
		break;
	case CONSUMABLE::INSTANT:
		rr.used_texture = TEXTURE_ASSET_ID::POTION_RED;
		break;
	default:
		rr.used_texture = TEXTURE_ASSET_ID::POTION_RED;
		break;
	}
	registry.hidables.emplace(e);
}

void WorldSystem::loadCampfire(Entity e) {
	AnimationData& anim = registry.animations.emplace(e);
	anim.spritesheet_texture = TEXTURE_ASSET_ID::CAMPFIRE_SPRITESHEET;
	anim.frametime_ms = 150;
	anim.frame_indices = { 0, 1, 2, 3, 4 };
	anim.spritesheet_columns = 5;
	anim.spritesheet_rows = 1;
	anim.spritesheet_width = 320;
	anim.spritesheet_height = 64;
	anim.frame_size = { anim.spritesheet_width / anim.spritesheet_columns, anim.spritesheet_height / anim.spritesheet_rows };

	registry.renderRequests.insert(
		e,
		{ TEXTURE_ASSET_ID::CAMPFIRE_SPRITESHEET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::ANIMATION,
			RENDER_LAYER_ID::FLOOR_DECO });
	registry.hidables.emplace(e);
}

void WorldSystem::loadRoomSystem(json roomSystemData) {
	roomSystem.current_floor = roomSystemData["current_floor"];
	roomSystem.current_room_idx = roomSystemData["current_room_idx"];
	roomSystem.rooms_cleared_current_floor = roomSystemData["rooms_cleared_current_floor"];
	roomSystem.current_objective = { 
		(ObjectiveType)roomSystemData["current_objective"]["type"], 
		roomSystemData["current_objective"]["remaining_count"],
		roomSystemData["current_objective"]["completed"]
	};
	roomSystem.updateObjective(roomSystem.current_objective.type, 0);
}

void WorldSystem::loadBoss(Entity e, json bossData) {
	Boss& boss = registry.bosses.get(e);
	boss.num_turns = bossData["num_turns"];
	boss.counter0 = bossData["counter0"];
	boss.counter1 = bossData["counter1"];
	boss.counter2 = bossData["counter2"];
}

void WorldSystem::loadAttackIndicators(json indicatorList) {
	for (auto& indicator : indicatorList) {
		Entity e = Entity();

		registry.motions.emplace(e);
		loadMotion(e, indicator["motion"]);

		RenderRequest renderRequest = {
		static_cast<TEXTURE_ASSET_ID>(indicator["renderRequest"]["used_texture"]),
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER_ID::FLOOR_DECO
		};
		registry.attackIndicators.emplace(e);
		registry.renderRequests.insert(e, renderRequest);
	}
}


void WorldSystem::logText(std::string msg, vec3 textColor) {
	int NUM_LINES = 12;
	
	vec2 defaultPos = vec2(50.f, (2.0f * window_height_px) * (7.5f/10.f));
	// vec3 textColor = vec3(1.0f, 1.0f, 1.0f); // white

	// shift existing logged text upwards
	for (Entity e : registry.logTexts.entities) {
		Text& text = registry.texts.get(e);
		text.position[1] -= 50.f;
		if (text.position[1] <= defaultPos[1] - 50.f*NUM_LINES) {
			registry.remove_all_components_of(e);
		}
	}

	Entity e = createText(renderer, defaultPos, msg, 1.5f, textColor);
	registry.logTexts.emplace(e);
	registry.renderRequests.get(e).used_layer = RENDER_LAYER_ID::LOG_TEXT;
	// TextTimer& timer = registry.textTimers.emplace(e);
	// timer.counter_ms = 8000;
}

void WorldSystem::doTurnOrderLogic() {
	Entity currentTurnEntity = turnOrderSystem.getCurrentTurnEntity();

	// if current entity is not doing turn and stopped moving, get the next turn entity
	if (!registry.queueables.get(currentTurnEntity).doing_turn && !registry.motions.get(currentTurnEntity).in_motion) {
		// if player just finished their turn, set is player turn to false
		if (registry.players.has(currentTurnEntity)) {
			set_is_player_turn(false);
		}
		// perform specific behaviour for bosses
		else if (registry.bosses.has(currentTurnEntity)) {
			// TODO: something's gotta go here eventually, right?
		}
		// perform end-of-movement attacks for enemies
		else {
			set_enemy_state_attack(currentTurnEntity);
			aiSystem.step(currentTurnEntity);
		}

		// handle end-of-turn behaviour
		handle_status_ticks(currentTurnEntity, false, false);

		// get next turn
		currentTurnEntity = turnOrderSystem.getNextTurn();

		// if the current entity is the player, call start_player_turn()
		if (registry.players.has(currentTurnEntity)) {
			set_is_player_turn(true);
			start_player_turn();
			logText("It is now your turn!", { 0.6, 0.6, 0.6 });
			set_gamestate(GameStates::BATTLE_MENU);
		}
		// handle start-of-turn behaviour
		handle_status_ticks(currentTurnEntity, true, false);
		reset_stats(currentTurnEntity);
		calc_stats(currentTurnEntity);
	}

	// if current turn entity is enemy and is still doing_turn call ai.step();
	if (!registry.players.has(currentTurnEntity) && registry.queueables.get(currentTurnEntity).doing_turn) {
		aiSystem.step(currentTurnEntity);

		// now that ai did its step, set doing turn to false
		registry.queueables.get(currentTurnEntity).doing_turn = false;
	}
}

void WorldSystem::updateTutorial() {
	if (!(tutorial_flags & SIGN_1)) {
		if (registry.interactables.has(tutorial_sign_1) && registry.interactables.get(tutorial_sign_1).interacted) {
			printf("flag 1 triggered\n");
			tutorial_flags = tutorial_flags | SIGN_1;
		}
	}
	else if (!(tutorial_flags & MOVEMENT_SELECTED)) {
		if (current_game_state == GameStates::MOVEMENT_MENU) {
			printf("flag 2 triggered\n");
			tutorial_flags = tutorial_flags | MOVEMENT_SELECTED;
			set_gamestate(GameStates::DIALOGUE);
			std::vector<std::vector<std::string>> messages = {
				{
					"You are in movement mode!",
				},
				{
					"In movement mode, click anywhere on the screen to move to that location.",
					"Energy Points (EP) will be depleted as you move."
				},
				{
					"To exit movement mode, click on the back button.",
					"You can also right click or hit ESC to exit movement mode."
				},
				{
					"Move until you run out of EP."
				}
			};
			activeTextbox = createTextbox(renderer, vec2(window_width_px/2.f, window_height_px/2.f), messages);
		}
	}
	else if (!(tutorial_flags & EP_DEPLETED)) {
		// check ep depleted
		if (registry.stats.get(player_main).ep <= 0) {
			printf("flag 3 triggered\n");
			tutorial_flags = tutorial_flags | EP_DEPLETED;
			set_gamestate(GameStates::BATTLE_MENU);
			set_gamestate(GameStates::DIALOGUE);
			std::vector<std::vector<std::string>> messages = {
				{
					"You have run out of EP!"
				},
				{
					"Your actions are limited with 0 EP.",
					"To refill your EP, end your turn."
				}
			};
			activeTextbox = createTextbox(renderer, vec2(window_width_px/2.f, window_height_px/2.f), messages);
			registry.remove_all_components_of(tutorial_wall_1);
		}
	}
	else if (!(tutorial_flags & SIGN_2)) {
		if (registry.interactables.has(tutorial_sign_2) && registry.interactables.get(tutorial_sign_2).interacted) {
			printf("flag 4 triggered\n");
			tutorial_flags = tutorial_flags | SIGN_2;
			printf("%u\n", registry.keyIcons.entities.size());
			// spawn slime
			Motion& sign_motion = registry.motions.get(tutorial_sign_2);
			tutorial_slime = createEnemy(renderer, { sign_motion.position.x, sign_motion.position.y + 256.f*ui_scale });
			registry.stats.get(tutorial_slime).hp = 14;
			turnOrderSystem.addNewEntity(tutorial_slime);
		}
	}
	else if (!(tutorial_flags & ATTACK_SELECTED)) {
		if (current_game_state == GameStates::ATTACK_MENU) {
			printf("flag 5 triggered\n");
			tutorial_flags = tutorial_flags | ATTACK_SELECTED;
			set_gamestate(GameStates::DIALOGUE);
			std::vector<std::vector<std::string>> messages = {
				{
					"You are in attack mode!",
				},
				{
					"In attack mode, your basic attack will always be pre-selected.",
					"Simply left click on a nearby enemy to hit it."
				},
				{
					"You will need 50 EP to perform your basic attack!",
					"And you can only attack once per turn!",
					"If you do not have enough EP, refresh it by ending your turn.",
				},
				{
					"To exit attack mode, click on the back button.",
					"You can also right click or hit ESC to exit attack mode."
				},
				{
					"Defeat the slime to move on."
				}
			};
			activeTextbox = createTextbox(renderer, vec2(window_width_px/2.f, window_height_px/2.f), messages);
		}
	}
	else if (!(tutorial_flags & SLIME1_DEFEATED)) {
		Motion& sign_motion = registry.motions.get(tutorial_sign_2);
		if (registry.enemies.size() <= 0) {
			printf("flag 6 triggered\n");
			tutorial_flags = tutorial_flags | SLIME1_DEFEATED;
			// spawn campfire
			tutorial_campfire = createCampfire(renderer, { sign_motion.position.x, sign_motion.position.y + 128.f*ui_scale });
			createMouseAnimation(renderer, { registry.motions.get(tutorial_campfire).position.x, registry.motions.get(tutorial_campfire).position.y - 64 });
			set_gamestate(GameStates::DIALOGUE);
			std::vector<std::vector<std::string>> messages = {
				{
					"A campfire has appeared!"
				},
				{
					"Camfires will recover your HP and MP to full!",
				},
				{
					"To interact with a campfire move close to it",
					"and left click when not in a sub menu to interact with it."
				}
			};
			activeTextbox = createTextbox(renderer, vec2(window_width_px/2.f, window_height_px/2.f), messages);
		}
	}
	else if (!(tutorial_flags & CAMPFIRE_INTERACTED)) {
		if (registry.interactables.has(tutorial_campfire) && registry.interactables.get(tutorial_campfire).interacted) {
			printf("flag 7 triggered\n");
			tutorial_flags = tutorial_flags | CAMPFIRE_INTERACTED;
			set_gamestate(GameStates::DIALOGUE);
			std::vector<std::vector<std::string>> messages = {
				{
					"There are many things that you can interact with.",
					"If something looks interesting, approach it",
					"and left click to interact with it!",
					"Remember to exit out of sub menus before interacting!"
				}
			};
			activeTextbox = createTextbox(renderer, vec2(window_width_px/2.f, window_height_px/2.f), messages);
			registry.remove_all_components_of(tutorial_wall_2);
		}
	}
	else if (!(tutorial_flags & SIGN_3)) {
		if (registry.interactables.has(tutorial_sign_3) && registry.interactables.get(tutorial_sign_3).interacted) {
			printf("flag 8 triggered\n");
			tutorial_flags = tutorial_flags | SIGN_3;
			registry.remove_all_components_of(tutorial_wall_3);
		}
	}
	else if (!(tutorial_flags & CHEST_1)) {
		if (registry.interactables.has(tutorial_chest_1) && registry.interactables.get(tutorial_chest_1).interacted) {
			printf("flag 9 triggered\n");
			tutorial_flags = tutorial_flags | CHEST_1;
			set_gamestate(GameStates::DIALOGUE);
			std::vector<std::vector<std::string>> messages = {
				{
					"You are ambushed by an enemy slime!"
					"",
					"Grab the sword by getting close and left clicking!",
				},
				{
					"With this new sword, you will see new abilities in attack mode!",
					"Click the icons to view the attacks."
				},
				{
					"Advanced combat tip:",
					"If you want to move but still be able to attack,",
					"you can prepare an attack action!"
				},
				{
					"Click on an attack and choose 'prepare'",
					"You can now move without the worry of running low on EP!",
				},
				{
					"To use the prepared attack,",
					"Enter attack mode and left click an enemy to use your attack!"
				},
				{
					"Defeat the slime to move on."
				}
			};
			activeTextbox = createTextbox(renderer, vec2(window_width_px/2.f, window_height_px/2.f), messages);
			// spawn slime
			Motion& sign_motion = registry.motions.get(tutorial_sign_3);
			tutorial_slime = createEnemy(renderer, { sign_motion.position.x + 64.f*ui_scale, sign_motion.position.y + 64.f*ui_scale });
			turnOrderSystem.addNewEntity(tutorial_slime);
		}
	}
	else if (!(tutorial_flags & SLIME2_DEFEATED)) {
		Motion& player_motion = registry.motions.get(player_main);
		if (registry.enemies.size() <= 0) {
			printf("flag 10 triggered\n");
			tutorial_flags = tutorial_flags | SLIME2_DEFEATED;
			set_gamestate(GameStates::DIALOGUE);
			std::vector<std::vector<std::string>> messages = {
				{
					"A path to the other chest opened up!"
				}
			};
			activeTextbox = createTextbox(renderer, vec2(window_width_px/2.f, window_height_px/2.f), messages);
			registry.remove_all_components_of(tutorial_wall_4);
		}
	}
	else if (!(tutorial_flags & CHEST_2)) {
		if (registry.interactables.has(tutorial_chest_2) && registry.interactables.get(tutorial_chest_2).interacted) {
			printf("flag 11 triggered\n");
			tutorial_flags = tutorial_flags | CHEST_2;
			set_gamestate(GameStates::DIALOGUE);
			std::vector<std::vector<std::string>> messages = {
				{
					"Artifacts gives you various beneficial effects!",
					"Left click the artifact to pick it up!"
				},
				{
					"You can see what the artifacts do in the glossary!",
					"Click the book in the top right to view all the artifacts!",
					"To see what an artifact does, click on an artifact in the book!"
				}
			};
			activeTextbox = createTextbox(renderer, vec2(window_width_px/2.f, window_height_px/2.f), messages);
			registry.remove_all_components_of(tutorial_wall_5);
		}
	}
	else if (!(tutorial_flags & SIGN_4)) {
		if (registry.interactables.has(tutorial_sign_4) && registry.interactables.get(tutorial_sign_4).interacted) {
			printf("flag 12 triggered\n");
			tutorial_flags = tutorial_flags | SIGN_4;
			Motion& sign_motion = registry.motions.get(tutorial_sign_4);
			createConsumable(renderer, {sign_motion.position.x, sign_motion.position.y + 64.f*ui_scale}, CONSUMABLE::INSTANT);
			registry.remove_all_components_of(tutorial_wall_6);
		}
	}
	else if (!(tutorial_flags & SIGN_5)) {
		if (registry.interactables.has(tutorial_sign_5) && registry.interactables.get(tutorial_sign_5).interacted) {
			printf("flag 13 triggered\n");
			tutorial_flags = tutorial_flags | SIGN_5;
			objectiveCounter = createObjectiveCounter(renderer, { 256, window_height_px * (1.f / 16.f) + 32});
			objectiveDescText = createText(renderer, { 272, window_height_px * (1.f / 16.f) + 76 }, "", 2.f, { 1.0, 1.0, 1.0 });
			objectiveNumberText = createText(renderer, { 272, window_height_px * (1.f / 16.f) + 204 }, "", 2.f, { 1.0, 1.0, 1.0 });
			roomSystem.setObjective(ObjectiveType::ACTIVATE_SWITCHES, 2);
		}
	}
	else if (!(tutorial_flags & EXIT)) {
		if (registry.texts.has(objectiveNumberText)) {
			int remain_obj = std::stoi(registry.texts.get(objectiveNumberText).message);
			if (remain_obj == 0) {
				printf("flag 14 triggered\n");
				tutorial_flags = tutorial_flags | EXIT;
				// remove door that spawned from objective system
				for (Entity e : registry.interactables.entities) {
					if (registry.interactables.get(e).type == INTERACT_TYPE::DOOR) {
						registry.remove_all_components_of(e);
					}
				}
				// spawn exit staircase
				tutorial_door = createDoor(renderer, { 4*64*ui_scale, 4*64*ui_scale }, false );
				createMouseAnimation(renderer, { 4 * 64 * ui_scale, 3 * 64 * ui_scale });
			}
		}
	}
	else if (registry.interactables.has(tutorial_door) && registry.interactables.get(tutorial_door).interacted) {
		Stats& stat = registry.stats.get(player_main);
		stat.hp = stat.maxhp;
		stat.mp = stat.maxmp;
		stat.ep = stat.maxep;
		tutorial = false;
		if (has_status(player_main, StatusType::HP_REGEN)) { remove_status(player_main, StatusType::HP_REGEN); }
	}
}

// Set attack state for enemies who attack after moving
void set_enemy_state_attack(Entity enemy) {
	if (!registry.enemies.has(enemy)) { return; }
	if (registry.enemies.get(enemy).type == ENEMY_TYPE::SLIME ||
		registry.enemies.get(enemy).type == ENEMY_TYPE::PLANT_SHOOTER ||
		registry.enemies.get(enemy).type == ENEMY_TYPE::CAVELING ||
		registry.enemies.get(enemy).type == ENEMY_TYPE::LIVING_ROCK ||
		registry.enemies.get(enemy).type == ENEMY_TYPE::LIVING_PEBBLE ||
		registry.enemies.get(enemy).type == ENEMY_TYPE::APPARITION) {
		registry.enemies.get(enemy).state = ENEMY_STATE::ATTACK;
	}
}

// Set game state
void set_gamestate(GameStates state) {
	previous_game_state = current_game_state;
	current_game_state = state;
}

// Enter Credits (This only exists because switch statements don't like variable initialization)
void WorldSystem::enter_credits() {
	set_gamestate(GameStates::CREDITS);
	Entity bg = createBackground(renderer, { window_width_px / 2, window_height_px / 2 });
	registry.renderRequests.get(bg).used_texture = TEXTURE_ASSET_ID::CG_CREDITS;
	registry.renderRequests.get(bg).used_layer = RENDER_LAYER_ID::UI_TOP;
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

void WorldSystem::playEnemyDeathSound(ENEMY_TYPE enemy_type) {
	switch (enemy_type) {
		case ENEMY_TYPE::SLIME:
			Mix_PlayChannel(-1, slime_death, 0);
			break;
		case ENEMY_TYPE::CAVELING:
			Mix_PlayChannel(-1, caveling_death, 0);
			break;
	}
}

void WorldSystem::playEnemyMoveSound(ENEMY_TYPE enemy_type) {
	switch (enemy_type) {
	case ENEMY_TYPE::SLIME:
		Mix_PlayChannel(-1, slime_move, 0);
		break;
	case ENEMY_TYPE::CAVELING:
		Mix_PlayChannel(-1, caveling_move, 0);
		break;
	}
}

// Remove a number of a status effect type from entity
void remove_status(Entity e, StatusType status, int number) {
	if (!registry.statuses.has(e)) { return; }

	int index = 0;
	StatusContainer& statuses = registry.statuses.get(e);
	statuses.sort_statuses_reverse();
	for (StatusEffect s : statuses.statuses) {
		if (s.effect == status && number > 0) {
			statuses.statuses.erase(statuses.statuses.begin() + index);
			number--;
			index++;
			remove_status_particle(e, s);
		}
	}
	reset_stats(e);
	calc_stats(e);
	return;
}

void remove_status_particle(Entity e, StatusEffect status) {
	if (!registry.particleContainers.has(e)) { return; }
	ParticleContainer& particleContainer = registry.particleContainers.get(e);
	PARTICLE_TYPE particle_type;
	switch (status.effect)
	{
	case StatusType::POISON:
	case StatusType::FANG_POISON:
		particle_type = PARTICLE_TYPE::POISON;
		break;
	case StatusType::ATK_BUFF:
		if (status.value < 0) {
			particle_type = PARTICLE_TYPE::ATK_DOWN;
		}
		else if (status.value > 0) {
			particle_type = PARTICLE_TYPE::ATK_UP;
		}
		break;
	case StatusType::DEF_BUFF:
		if (status.value < 0) {
			particle_type = PARTICLE_TYPE::DEF_DOWN;
		}
		else if (status.value > 0) {
			particle_type = PARTICLE_TYPE::DEF_UP;
		}
		break;
	case StatusType::RANGE_BUFF:
		if (status.value < 0) {
			particle_type = PARTICLE_TYPE::RANGE_DOWN;
		}
		else if (status.value > 0) {
			particle_type = PARTICLE_TYPE::RANGE_UP;
		}
		break;
	case StatusType::SLIMED:
		particle_type = PARTICLE_TYPE::SLIMED;
		break;
	case StatusType::STUN:
		particle_type = PARTICLE_TYPE::STUN;
		break;
	case StatusType::INVINCIBLE:
		particle_type = PARTICLE_TYPE::INVINCIBLE;
		break;
	case StatusType::HP_REGEN:
		particle_type = PARTICLE_TYPE::HP_REGEN;
		break;
	default:
		return;
	}

	for (int i = 0; i < particleContainer.emitters.size(); i++) {
		if (particleContainer.emitters[i].type == particle_type) {
			particleContainer.emitters.erase(particleContainer.emitters.begin() + i);
			return;
		}
	}
}
  
void WorldSystem::handleActionButtonPress() {
	// hide all the hotkeys if not in attack mode
	for (Entity ki : registry.keyIcons.entities) {
		registry.remove_all_components_of(ki);
	}

	// hide all action buttons
	for (Entity ab : registry.actionButtons.entities) {
		registry.remove_all_components_of(ab);
	}
	hideGuardButton = true;

	// create back button and move mode text
	createBackButton(renderer, { window_width_px - 125.f , window_height_px - 100.f * ui_scale });
}

void WorldSystem::moveAction() {
	if (current_game_state != GameStates::ENEMY_TURN) {
		// set player action to move
		Player& player = registry.players.get(player_main);
		Stats stats = registry.stats.get(player_main);
		player.action = PLAYER_ACTION::MOVING;

		// show ep range
		Motion motion = registry.motions.get(player_main);
		create_ep_range(stats.ep, motion.movement_speed, motion.position);

		// set game state to move menu
		set_gamestate(GameStates::MOVEMENT_MENU);
		handleActionButtonPress();
		createMoveModeText(renderer, { window_width_px - 125.f, 350.f * ui_scale });
	}
}

void WorldSystem::attackAction() {
	if (current_game_state != GameStates::ENEMY_TURN) {
		// set player action to attack
		Player& player = registry.players.get(player_main);
		Equipment weapon = registry.inventories.get(player_main).equipped[0];
		player.action = PLAYER_ACTION::ATTACKING;
		float button_y = 180.f;

		// if prepared attack, only show the prepared attack
		if (player.prepared) {
			player.using_attack = player.selected_attack;
			createAttackCard(renderer, { window_width_px - 125.f, button_y }, player.using_attack);
		}
		else {
			player.selected_attack = ATTACK::NONE;
			player.using_attack = ATTACK::NONE;

			//createAttackModeText(renderer, { window_width_px - 125.f, 200.f });
			// render attack types 
			createAttackCard(renderer, { window_width_px - 125.f, button_y }, ATTACK::NONE);
			for (ATTACK a : weapon.attacks) {
				button_y += 150 * 4 / 5;
				createAttackCard(renderer, { window_width_px - 125.f, button_y }, a);
			}
		}
		// set game state to attack menu
		set_gamestate(GameStates::ATTACK_MENU);
		handleActionButtonPress();
	}
}

void WorldSystem::backAction() {
	// hide all item cards
	for (Entity ic : registry.itemCards.entities) {
		registry.remove_all_components_of(ic);
	}

	// hide all attack cards
	for (Entity ac : registry.attackCards.entities) {
		registry.remove_all_components_of(ac);
	}

	// hide all the hotkeys if not in attack mode
	for (Entity ki : registry.keyIcons.entities) {
		registry.remove_all_components_of(ki);
	}

	// remove all attack dialog 
	for (Entity ad : registry.attackDialogs.entities) {
		registry.remove_all_components_of(ad);
	}

	// remove the artifact dialogs
	for (Entity dd : registry.descriptionDialogs.entities) {
		registry.remove_all_components_of(dd);
	}

	// remove all equipment dialogs
	for (Entity ed : registry.equipmentDialogs.entities) {
		registry.remove_all_components_of(ed);
	}

	// set gamestate back to normal
	set_gamestate(GameStates::BATTLE_MENU);

}

void WorldSystem::itemAction() {
	Inventory& inv = registry.inventories.get(player_main);
	printf("sprite: %d", inv.equipped[0].sprite);
	createItemMenu(renderer, { window_width_px - 125.f, 200.f * ui_scale }, inv);

	handleActionButtonPress();

	// set game state to move menu
	set_gamestate(GameStates::ITEM_MENU);
}

void WorldSystem::cancelAction() {
	backAction();
}

void WorldSystem::advanceTextbox() {
	if (registry.textboxes.size() == 0) {
		if(current_game_state != GameStates::CUTSCENE){
			set_gamestate(GameStates::BATTLE_MENU);
		}
	}
		Textbox& textbox = registry.textboxes.get(activeTextbox);
		// clear lines
		for (Entity text : textbox.lines) {
			registry.remove_all_components_of(text);
		}
		textbox.lines.clear();
		textbox.num_lines = 0;
		if (textbox.next_message >= textbox.num_messages) {
			// clear textbox (no more messages)
			if (registry.animations.has(textbox.icon)) {
				registry.remove_all_components_of(textbox.icon);
			}
			for (Entity textbox : registry.textboxes.entities) {
				registry.remove_all_components_of(textbox);
			}
			set_gamestate(previous_game_state);
			return;
		}
		vec2 pos = registry.motions.get(activeTextbox).position;
		std::vector<std::string> message = textbox.messages[textbox.next_message];
		for (std::string line : message) {
			textbox.num_lines++;
			Entity text = createText(renderer, pos*2.f + vec2(-TEXTBOX_BB_WIDTH + 100.f, -TEXTBOX_BB_HEIGHT + 75.f * textbox.num_lines), line, 2.0f, vec3(1.f));
			textbox.lines.push_back(text);
		}
		textbox.next_message++;
}

void WorldSystem::generateNewRoom(Floors floor, bool repeat_allowed) {
	// save game (should be just player stuff)
	json playerData = saveSystem.jsonifyPlayer(player_main);
	// remove all entities for new room
	removeForNewRoom();
	// remove player
	registry.remove_all_components_of(player_main);
	// make new map
	std::string next_map = roomSystem.getRandomRoom(floor, repeat_allowed);
	spawnData = createTiles(renderer, next_map);
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
	motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });

	// Refill Player EP
	reset_stats(player_main);
	calc_stats(player_main);
	stats.ep = stats.maxep;

	spawn_enemies_random_location(spawnData.enemySpawns, spawnData.minEnemies, spawnData.maxEnemies);
	spawn_items_random_location(spawnData.itemSpawns, spawnData.minItems, spawnData.maxItems);

	remove_fog_of_war();
	create_fog_of_war();

	// setup turn order system
	turnOrderSystem.setUpTurnOrder();
	// start first turn
	turnOrderSystem.getNextTurn();

	// create an objective
	if (roomSystem.current_floor == Floors::BOSS1 || roomSystem.current_floor == Floors::BOSS2) {
		roomSystem.setObjective(ObjectiveType::DEFEAT_BOSS, 1);
	}
	else {
		roomSystem.setRandomObjective();
	}
	roomSystem.updateClearCount();

	saveSystem.saveGameState(turnOrderSystem.getTurnOrder(), roomSystem);

	if (!registry.loadingTimers.has(player_main)) {
		registry.loadingTimers.emplace(player_main);
	}
	printf("rooms cleared on current floor: %d\n", roomSystem.rooms_cleared_current_floor);
}

void WorldSystem::update_turn_ui() {
	// clear icon registry
	for (Entity e : registry.icons.entities) {
		registry.remove_all_components_of(e);
	}

	Motion& turn_ui_motion = registry.motions.get(turnUI);
	vec2 position = turn_ui_motion.position;
	vec2 startPos = vec2(turn_ui_motion.scale[0] / 2.f, 0.f);
	vec2 offset = vec2(0.f);

	// get queue
	std::queue<Entity> queue = turnOrderSystem.getTurnOrder();

	for (int count = 0; !queue.empty() && count < 5; queue.pop()) {
		Entity e = queue.front();
		if (!registry.motions.has(e)) { continue; }
		if (!registry.hidden.has(e)) {
			offset[0] = 48.f*count + 32.f;
			TEXTURE_ASSET_ID texture_id;
			if (registry.players.has(e)) {
				texture_id = TEXTURE_ASSET_ID::PLAYER;
			}
			else {
				texture_id = registry.renderRequests.get(e).used_texture;
			}
			createIcon(renderer, position - startPos + offset, texture_id);
			count++;
		}
	}
	return;
}

void WorldSystem::update_background_collection(float floor_w, float floor_h) {


	if (current_game_state == GameStates::MAIN_MENU) {
		printf("!@!@#!@$@$@\n");
		float x_offset = 0.f;
		float y_offset = 0.f;
		vec2 pos = { 0,0 };
		if (saveSystem.saveDataExists()) {
			// remove entities to load in entities
			//removeForLoad();
			//printf("Removed for load\n");
			// get saved game data
			json data = saveSystem.getSaveData();
			// load player
			json entityList = data["entities"];
			//json collidablesList = data["map"]["collidables"];
			//json interactablesList = data["map"]["interactables"];
			//json tilesList = data["map"]["tiles"];
			//json roomSystemJson = data["room"];
			//json attackIndicatorList = data["attack_indicators"];

			// load enemies
			std::queue<Entity> entities;
			for (auto& entity : entityList) {
				Entity e;
				Inventory inv;
				if (entity["type"] == "player") {
					printf("111 type is player successful... loading player invetory \n");
					loadPlayerCollectionTitleScreen(entity,floor_w, floor_h);
					//player_main = e; 
				}
			}
		}

	}
}

void WorldSystem::updateGameBackground() {
	Motion playerMotion = registry.motions.get(player_main);

	Motion& backgroundMotion = registry.motions.get(background);
	backgroundMotion.position.x = playerMotion.position.x + window_width_px/2.f;
	backgroundMotion.position.y = playerMotion.position.y;

	Motion& background_b_motion = registry.motions.get(background_back);
	background_b_motion.position.x = playerMotion.position.x + window_width_px/2.f - fmod((playerMotion.position.x + window_width_px) * 0.9f, window_width_px);
	background_b_motion.position.y = playerMotion.position.y;

	Motion& background_m_motion = registry.motions.get(background_mid);
	background_m_motion.position.x = playerMotion.position.x + window_width_px/2.f - fmod((playerMotion.position.x + window_width_px) * 1.2f, window_width_px);
	background_m_motion.position.y = playerMotion.position.y;

	Motion& background_f_motion = registry.motions.get(background_front);
	background_f_motion.position.x = playerMotion.position.x + window_width_px/2.f - fmod((playerMotion.position.x + window_width_px) * 1.5f, window_width_px);
	background_f_motion.position.y = playerMotion.position.y;
}

// Use attack depending on player's current using attack
// TODO: Change this so it's less hard-coded (please)
void WorldSystem::use_attack(vec2 target_pos) {
	Player& player = registry.players.get(player_main);
	Motion& player_motion = registry.motions.get(player_main);
	Stats& player_stats = registry.stats.get(player_main);

	float mp_cost = attack_mpcosts.at(player.using_attack);
	float ep_cost = attack_epcosts.at(player.using_attack) * player_stats.eprateatk;
	bool attack_success = false;

	float dist = dist_to(player_motion.position, target_pos);
	float angle = atan2(target_pos.y - player_motion.position.y, target_pos.x - player_motion.position.x);

	// costs 0 if prepared
	if (player.prepared) {
		mp_cost = 0;
		ep_cost = 0;
	}

	// I hate my own implementation of this, and I want to change it as soon as Milestone 3 is over
	switch (player.using_attack) {
	case ATTACK::NONE:
		try {
			Entity& target = get_targeted_enemy(target_pos);

			// only attack if the player hasn't attacked that turn
			if (!player.attacked) {
				Motion m = registry.motions.get(target);

				// only attack if have enough ep and is close enough
				if (player_stats.ep >= ep_cost && dist_to_edge(player_motion, m) <= 50.f) {

					// show attack animation
					createAttackAnimation(renderer, { m.position.x, m.position.y }, player.using_attack);

					// play attack sound
					Mix_PlayChannel(-1, sword_parry, 0);

					logText(deal_damage(player_main, target, 100.f));

					// wobble the enemy lol
					if (!registry.wobbleTimers.has(target) && !registry.knockbacks.has(target)) {
						WobbleTimer& wobble = registry.wobbleTimers.emplace(target);
						wobble.orig_scale = m.scale;
					}

					player_stats.mp = min(player_stats.maxmp, player_stats.mp + (player_stats.maxmp * 0.1f * player_stats.mpregen));
					// Arcane Funnel effect
					if (has_status(player_main, StatusType::ARCANE_FUNNEL)) {
						player_stats.mp = min(player_stats.maxmp, player_stats.mp + (player_stats.maxmp * 0.1f * player_stats.mpregen));
						Entity mana = createBigSlash(world.renderer, { m.position.x, m.position.y }, 0, 256);
						registry.renderRequests.get(mana).used_texture = TEXTURE_ASSET_ID::MANACIRCLE;
					}

					attack_success = true;
				}
				else if (player_stats.ep < ep_cost) {
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
		catch (int e) {
			break;
		}
		break;

	case ATTACK::ROUNDSLASH:
		// only attack if the player hasn't attacked that turn
		if (!player.attacked) {

			// only attack if have enough ep and mp
			if (player_stats.ep >= ep_cost && player_stats.mp >= mp_cost) {
				// show attack animation
				createAttackAnimation(renderer, { player_motion.position.x, player_motion.position.y }, player.using_attack);
				Mix_PlayChannel(-1, sword_slash, 0);

				// check enemies that are in area
				Motion aoe = {};
				aoe.position = player_motion.position;
				aoe.scale = { 300.f, 300.f };

				for (Entity& en : registry.enemies.entities) {	

					if (collides_circle(registry.motions.get(en), aoe)) {
						// wobble the enemy lol
						if (!registry.wobbleTimers.has(en) && !registry.knockbacks.has(en)) {
							WobbleTimer& wobble = registry.wobbleTimers.emplace(en);
							wobble.orig_scale = registry.motions.get(en).scale;
						}

						logText(deal_damage(player_main, en, 80.f));
					}
				}

				attack_success = true;
			}
			else {
				logText("Not enough MP or EP to attack!");
				// play error sound
				Mix_PlayChannel(-1, error_sound, 0);
			}
		}
		else {
			logText("You already attacked this turn!");
			// play error sound
			Mix_PlayChannel(-1, error_sound, 0);
		}
		break;

	case ATTACK::SAPPING_STRIKE:
		try {
			Entity& target = get_targeted_enemy(target_pos);

			// only attack if the player hasn't attacked that turn
			if (!player.attacked) {
				Motion m = registry.motions.get(target);

				// only attack if have enough ep and is close enough
				if (player_stats.ep >= ep_cost && player_stats.mp >= mp_cost && dist_to_edge(player_motion, m) <= 50.f) {

					// show attack animation
					Entity anim = createAttackAnimation(renderer, { m.position.x, m.position.y }, player.using_attack);
					registry.colors.insert(anim, {0.f, 0.f, 1.f, 1.f});

					// play attack sound
					Mix_PlayChannel(-1, sword_parry, 0);

					logText(deal_damage(player_main, target, 80.f));

					// wobble the enemy lol
					if (!registry.wobbleTimers.has(target) && !registry.knockbacks.has(target)) {
						WobbleTimer& wobble = registry.wobbleTimers.emplace(target);
						wobble.orig_scale = m.scale;
					}

					player_stats.mp = min(player_stats.maxmp, player_stats.mp + 30.f);
					attack_success = true;
				}
				else if (player_stats.ep < ep_cost || player_stats.mp < mp_cost) {
					logText("Not enough MP or EP to attack!");
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
		catch (int e) {
			break;
		}
		break;

	case ATTACK::PIERCING_THRUST:
		// only attack if the player hasn't attacked that turn
		if (!player.attacked) {

			// only attack if have enough ep and mp
			if (player_stats.ep >= ep_cost && player_stats.mp >= mp_cost) {
				Mix_PlayChannel(-1, sword_pierce, 0);
				Motion aoe = {};
				aoe.position = dirdist_extrapolate(player_motion.position, angle, 150.f);
				aoe.angle = angle;
				aoe.scale = { 200.f, 5.f };
				Entity animation = createAttackAnimation(renderer, dirdist_extrapolate(player_motion.position, angle, 100.f), player.using_attack);
				registry.motions.get(animation).angle = angle + M_PI/2 + M_PI;

				// check enemies that are in area
				for (Entity& en : registry.enemies.entities) {

					if (collides_rotrect_circle(aoe, registry.motions.get(en))) {
						// wobble the enemy lol
						if (!registry.wobbleTimers.has(en) && !registry.knockbacks.has(en)) {
							WobbleTimer& wobble = registry.wobbleTimers.emplace(en);
							wobble.orig_scale = registry.motions.get(en).scale;
						}
						Stats& enemy_stats = registry.stats.get(en);
						float def_mod = enemy_stats.def * 0.4;
						enemy_stats.def -= def_mod;
						logText(deal_damage(player_main, en, 120.f));
						enemy_stats.def += def_mod;
					}
				}

				attack_success = true;
			}
			else {
				logText("Not enough MP or EP to attack!");
				// play error sound
				Mix_PlayChannel(-1, error_sound, 0);
			}
		}
		else {
			logText("You already attacked this turn!");
			// play error sound
			Mix_PlayChannel(-1, error_sound, 0);
		}
		break;

	case ATTACK::PARRYING_STANCE:
		// only attack if the player hasn't attacked that turn
		if (!player.attacked) {

			// only attack if have enough ep and mp
			if (player_stats.ep >= ep_cost && player_stats.mp >= mp_cost) {
				logText("Parrying Stance activated!");
				Mix_PlayChannel(-1, special_sound, 0);

				StatusEffect stance = StatusEffect(0, 0, StatusType::PARRYING_STANCE, false, true);
				apply_status(player_main, stance);

				ep_cost = 0;
				attack_success = true;
			}
			else {
				logText("Not enough MP or EP to attack!");
				// play error sound
				Mix_PlayChannel(-1, error_sound, 0);
			}
		}
		else {
			logText("You already attacked this turn!");
			// play error sound
			Mix_PlayChannel(-1, error_sound, 0);
		}
		break;

	case ATTACK::DISENGAGE:

		// only attack if have enough ep and mp
		if (player_stats.ep >= ep_cost && player_stats.mp >= mp_cost) {

			if (!registry.knockbacks.has(player_main)) {
				KnockBack& knockback = registry.knockbacks.emplace(player_main);
				knockback.remaining_distance = 300.f;
				knockback.angle = atan2(target_pos.y - player_motion.position.y, target_pos.x - player_motion.position.x);
			}

			StatusEffect trigger = StatusEffect(0, 0, StatusType::DISENGAGE_TRIGGER, false, false);
			apply_status(player_main, trigger);
			Mix_PlayChannel(-1, whoosh, 0);

			attack_success = true;
		}
		else {
			logText("Not enough MP or EP to attack!");
			// play error sound
			Mix_PlayChannel(-1, error_sound, 0);
		}
		break;

	case ATTACK::TERMINUS_VERITAS:
		// only attack if the player hasn't attacked that turn
		if (!player.attacked) {

			// only attack if have enough ep and mp
			if (player_stats.ep >= ep_cost && player_stats.mp >= mp_cost) {
				Mix_PlayChannel(-1, sword_end, 0);
				Motion aoe = {};
				aoe.position = dirdist_extrapolate(player_motion.position, angle, player_stats.range * 1.1);
				aoe.angle = angle;
				aoe.scale = { player_stats.range * 2.f, player_stats.range * 2.f };

				createBigSlash(renderer, player_motion.position, angle, player_stats.range*2);

				// logic for Terminus Veritas damage
				mp_cost = player_stats.mp;
				if (player.prepared) { mp_cost += attack_mpcosts.at(player.using_attack); }
				float multiplier = 4.5f * mp_cost;
				if (mp_cost > 90.f) {
					multiplier = 5.f * mp_cost;
				}

				// check enemies that are in area
				for (Entity& en : registry.enemies.entities) {

					if (collides_rotrect_circle(aoe, registry.motions.get(en)) 
						&& dist_to(player_motion.position, registry.motions.get(en).position) <= player_stats.range) {
						// wobble the enemy lol
						if (!registry.wobbleTimers.has(en) && !registry.knockbacks.has(en)) {
							WobbleTimer& wobble = registry.wobbleTimers.emplace(en);
							wobble.orig_scale = registry.motions.get(en).scale;
						}
						Stats& enemy_stats = registry.stats.get(en);
						float def_mod = enemy_stats.def * 0.4;
						logText(deal_damage(player_main, en, multiplier));
					}
				}

				attack_success = true;
			}
			else {
				logText("Not enough MP or EP to attack!");
				// play error sound
				Mix_PlayChannel(-1, error_sound, 0);
			}
		}
		else {
			logText("You already attacked this turn!");
			// play error sound
			Mix_PlayChannel(-1, error_sound, 0);
		}
		break;

	default:
		break;
	}

	// perform behaviour if attack succeeds
	if (attack_success) {
		player_stats.ep = max(0.f, player_stats.ep - ep_cost);
		player_stats.mp = max(0.f, player_stats.mp - mp_cost);
		if (player.using_attack != ATTACK::DISENGAGE) {
			player.attacked = true;
		}
		player.prepared = false;
		// hide all attack cards
		for (Entity ac : registry.attackCards.entities) {
			registry.remove_all_components_of(ac);
		}
		attackAction();
	}
}

// return clicked enemy, throws exception if not found
Entity& get_targeted_enemy(vec2 target_pos) {
	for (Entity& en : registry.enemies.entities) {

		// super simple bounding box for now
		Motion m = registry.motions.get(en);
		int enemyX = m.position[0];
		int enemyY = m.position[1];

		if ((target_pos.x <= (enemyX + m.scale[0] / 2) && target_pos.x >= (enemyX - m.scale[0] / 2)) &&
			(target_pos.y >= (enemyY - m.scale[1] / 2) && target_pos.y <= (enemyY + m.scale[1] / 2))) {
			return en;
		}
	}
	throw 0;
}

void WorldSystem::playMusic(Music music) {
	if (current_music == music)
		return;
	switch (music) {
	case Music::NONE:
		Mix_PauseMusic();
		break;
	case Music::BACKGROUND:
		current_music = music;
		Mix_PlayMusic(background_music, -1);
		break;
	case Music::MENU:
		current_music = music;
		Mix_PlayMusic(menu_music, -1);
		break;
	case Music::CUTSCENE:
		current_music = music;
		Mix_PlayMusic(cutscene_music, -1);
		break;
	case Music::BOSS0:
		current_music = music;
		Mix_PlayMusic(boss0_music, -1);
		break;
	case Music::RUINS:
		current_music = music;
		Mix_PlayMusic(ruins_music, -1);
		break;
	case Music::BOSS1:
		current_music = music;
		Mix_PlayMusic(boss1_music, -1);
		break;
	default:
		printf("unsupported Music enum value %d\n", music);
	}
}

ParticleEmitter setupParticleEmitter(PARTICLE_TYPE type) {
	ParticleEmitter emitter = ParticleEmitter();
	emitter.type = type;

	switch (type) {
	case PARTICLE_TYPE::POISON:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::POISON_BUBBLE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 300;
		emitter.max_interval_ms = 600;
		emitter.particle_decay_ms = 1500;
		emitter.base_scale = vec2(20, 20);
		emitter.min_scale_factor = 0.5;
		emitter.max_scale_factor = 1.2;
		emitter.min_offset_x = -8;
		emitter.max_offset_x = 8;
		emitter.min_offset_y = 0;
		emitter.max_offset_y = 24;
		emitter.min_velocity_x = -40;
		emitter.max_velocity_x = 40;
		emitter.min_velocity_y = -20;
		emitter.max_velocity_y= -60;
		emitter.min_angle = 0;
		emitter.max_angle = 0;
		break;
	case PARTICLE_TYPE::ATK_UP:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::BUFF_ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 300;
		emitter.max_interval_ms = 700;
		emitter.particle_decay_ms = 1500;
		emitter.base_scale = vec2(32, 32);
		emitter.min_scale_factor = 0.75;
		emitter.max_scale_factor = 1;
		emitter.min_offset_x = -48;
		emitter.max_offset_x = 48;
		emitter.min_offset_y = -16;
		emitter.max_offset_y = 0;
		emitter.min_velocity_x = 0;
		emitter.max_velocity_x = 0;
		emitter.min_velocity_y = -60;
		emitter.max_velocity_y = -40;
		emitter.min_angle = 0;
		emitter.max_angle = 0;
		emitter.color_shift = { 1.f, 0.f, 0.f, 0.75f };
		break;
	case PARTICLE_TYPE::ATK_DOWN:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::BUFF_ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 300;
		emitter.max_interval_ms = 700;
		emitter.particle_decay_ms = 1500;
		emitter.base_scale = vec2(32, 32);
		emitter.min_scale_factor = 0.75;
		emitter.max_scale_factor = 1;
		emitter.min_offset_x = -48;
		emitter.max_offset_x = 48;
		emitter.min_offset_y = -64;
		emitter.max_offset_y = -32;
		emitter.min_velocity_x = 0;
		emitter.max_velocity_x = 0;
		emitter.min_velocity_y = 30;
		emitter.max_velocity_y = 50;
		emitter.min_angle = M_PI;
		emitter.max_angle = M_PI;
		emitter.color_shift = { 1.f, 0.f, 0.f, 0.75f };
		break;
	case PARTICLE_TYPE::RANGE_UP:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::BUFF_ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 300;
		emitter.max_interval_ms = 700;
		emitter.particle_decay_ms = 1500;
		emitter.base_scale = vec2(32, 32);
		emitter.min_scale_factor = 0.75;
		emitter.max_scale_factor = 1;
		emitter.min_offset_x = -48;
		emitter.max_offset_x = 48;
		emitter.min_offset_y = -16;
		emitter.max_offset_y = 0;
		emitter.min_velocity_x = 0;
		emitter.max_velocity_x = 0;
		emitter.min_velocity_y = -60;
		emitter.max_velocity_y = -40;
		emitter.min_angle = 0;
		emitter.max_angle = 0;
		emitter.color_shift = { 1.f, 1.f, 0.f, 0.75f };
		break;
	case PARTICLE_TYPE::RANGE_DOWN:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::BUFF_ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 300;
		emitter.max_interval_ms = 700;
		emitter.particle_decay_ms = 1500;
		emitter.base_scale = vec2(32, 32);
		emitter.min_scale_factor = 0.75;
		emitter.max_scale_factor = 1;
		emitter.min_offset_x = -48;
		emitter.max_offset_x = 48;
		emitter.min_offset_y = -64;
		emitter.max_offset_y = -32;
		emitter.min_velocity_x = 0;
		emitter.max_velocity_x = 0;
		emitter.min_velocity_y = 30;
		emitter.max_velocity_y = 50;
		emitter.min_angle = M_PI;
		emitter.max_angle = M_PI;
		emitter.color_shift = { 1.f, 1.f, 0.f, 0.75f };
		break;
	case PARTICLE_TYPE::DEF_UP:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::BUFF_ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 300;
		emitter.max_interval_ms = 700;
		emitter.particle_decay_ms = 1500;
		emitter.base_scale = vec2(32, 32);
		emitter.min_scale_factor = 0.75;
		emitter.max_scale_factor = 1;
		emitter.min_offset_x = -48;
		emitter.max_offset_x = 48;
		emitter.min_offset_y = -16;
		emitter.max_offset_y = 0;
		emitter.min_velocity_x = 0;
		emitter.max_velocity_x = 0;
		emitter.min_velocity_y = -60;
		emitter.max_velocity_y = -40;
		emitter.min_angle = 0;
		emitter.max_angle = 0;
		emitter.color_shift = { 0.f, 0.f, 1.f, 0.75f };
		break;
	case PARTICLE_TYPE::DEF_DOWN:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::BUFF_ARROW,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 300;
		emitter.max_interval_ms = 700;
		emitter.particle_decay_ms = 1500;
		emitter.base_scale = vec2(32, 32);
		emitter.min_scale_factor = 0.75;
		emitter.max_scale_factor = 1;
		emitter.min_offset_x = -48;
		emitter.max_offset_x = 48;
		emitter.min_offset_y = -64;
		emitter.max_offset_y = -32;
		emitter.min_velocity_x = 0;
		emitter.max_velocity_x = 0;
		emitter.min_velocity_y = 30;
		emitter.max_velocity_y = 50;
		emitter.min_angle = M_PI;
		emitter.max_angle = M_PI;
		emitter.color_shift = { 0.f, 0.f, 1.f, 0.75f };
		break;
	case PARTICLE_TYPE::SLIMED:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::SLIME_DROPLET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 200;
		emitter.max_interval_ms = 500;
		emitter.particle_decay_ms = 520;
		emitter.base_scale = vec2(12, 12);
		emitter.min_scale_factor = 1.0;
		emitter.max_scale_factor = 2.0;
		emitter.min_offset_x = -32;
		emitter.max_offset_x = 32;
		emitter.min_offset_y = -32;
		emitter.max_offset_y = -32;
		emitter.min_velocity_x = 0;
		emitter.max_velocity_x = 0;
		emitter.min_velocity_y = 20;
		emitter.max_velocity_y = 40;
		break;
	case PARTICLE_TYPE::STUN:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::STUN_PARTICLE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 200;
		emitter.max_interval_ms = 400;
		emitter.particle_decay_ms = 1000;
		emitter.base_scale = vec2(32, 32);
		emitter.min_scale_factor = 0.5;
		emitter.max_scale_factor = 1;
		emitter.min_offset_x = 0;
		emitter.max_offset_x = 0;
		emitter.min_offset_y = -24;
		emitter.max_offset_y = -24;
		emitter.min_velocity_x = -60;
		emitter.max_velocity_x = 60;
		emitter.min_velocity_y = -60;
		emitter.max_velocity_y = 60;
		emitter.min_angle = 0;
		emitter.max_angle = 2*M_PI;
		emitter.color_shift = { 1.f, 1.f, 1.f, 0.8f };
		break;
	case PARTICLE_TYPE::INVINCIBLE:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::INVINCIBLE_PARTICLE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 450;
		emitter.max_interval_ms = 800;
		emitter.particle_decay_ms = 2000;
		emitter.base_scale = vec2(32, 32);
		emitter.min_scale_factor = 1;
		emitter.max_scale_factor = 1;
		emitter.min_offset_x = -32;
		emitter.max_offset_x = 32;
		emitter.min_offset_y = -32;
		emitter.max_offset_y = 32;
		emitter.min_velocity_x = 0;
		emitter.max_velocity_x = 0;
		emitter.min_velocity_y = 0;
		emitter.max_velocity_y = 0;
		emitter.min_angle = 0;
		emitter.max_angle = 0;
		break;
	case PARTICLE_TYPE::HP_REGEN:
		emitter.render_data = RenderRequest{
			TEXTURE_ASSET_ID::HP_REGEN_PARTICLE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT };
		emitter.min_interval_ms = 300;
		emitter.max_interval_ms = 500;
		emitter.particle_decay_ms = 1000;
		emitter.base_scale = vec2(16, 16);
		emitter.min_scale_factor = 1;
		emitter.max_scale_factor = 1;
		emitter.min_offset_x = -32;
		emitter.max_offset_x = 32;
		emitter.min_offset_y = -32;
		emitter.max_offset_y = 32;
		emitter.min_velocity_x = 0;
		emitter.max_velocity_x = 0;
		emitter.min_velocity_y = 0;
		emitter.max_velocity_y = 0;
		emitter.min_angle = 0;
		emitter.max_angle = 0;
		break;
	default:
		break;
	}
	return emitter;
}