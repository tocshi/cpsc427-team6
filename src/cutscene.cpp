#include "cutscene.hpp"
#include "world_system.hpp"
#include "world_init.hpp"

bool next_scene = false; 
bool main_character = false; 

void CutSceneSystem::cut_scene_text_log(RenderSystem* renderer, std::string msg) {
	
	
	for (Entity e : registry.textTimers.entities) {
		Text& text = registry.texts.get(e);
		text.position[1] -= 50.f;
	}

	// vec2 defaultPos = vec2((2.0f * window_width_px) * (1.f/20.f), (2.0f * window_height_px) * (7.f/10.f));
	vec2 defaultPos = vec2(50.f, 50.f);
	vec3 textColor = vec3(1.0f, 1.0f, 1.0f); // white

	Entity e = createText(renderer, defaultPos, msg, 1.5f, textColor);
	TextTimer& timer = registry.textTimers.emplace(e);
	timer.counter_ms = 8000;

	printf("cut scen dialogue\n");

}


void CutSceneSystem::updateDialogue(RenderSystem* renderer, int cut_scene_number) {
	//“Submit your dungeon - themed game here!Winner will be decided by a board of professional judges...”
	//“...$1000 cash reward, and a chance to work with a development team to further expand your game idea!”
	if (cut_scene_number == 0) {
		std::vector<std::vector<std::string>> messages = {
			{
				"Daring Dungeons Game Jam\n",
				"Submit your dungeon-themed game here!",
				" Winner will be decided by a board of professional judges...",
				"...$1000 cash reward, and a chance to work with a development team to",
				" further expand your game idea!."
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.4), messages);
	}


	
	//updateTextBox(renderer, activeTextbox);
}


void CutSceneSystem::updateTextBox(RenderSystem* renderer, Entity activeTextbox) {


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
		//set_gamestate(GameStates);
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



void CutSceneSystem::scene_transition(RenderSystem* renderer, int cut_scene_number) {
	cut_scene_text_log(renderer, "Press Escape to skip");
	if (cut_scene_number == 0) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		updateDialogue(renderer, cut_scene_number);
	}
	else if (cut_scene_number == 1) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE2);
		
	}
	else if (cut_scene_number == 2) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE3);
		//cut_scene_dialogue(renderer, "Press Escape to skip and go to Main Menu");
	}

	printf("Scene Transition function \n");

}