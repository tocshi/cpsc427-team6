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
	//"Submit your dungeon - themed game here!Winner will be decided by a board of professional judges..."
	//"..$1000 cash reward, and a chance to work with a development team to further expand your game idea!"
	while (registry.textboxes.entities.size() > 0) {
		registry.remove_all_components_of(registry.textboxes.entities.back());
	}

	if (cut_scene_number < 0) { return; }

	std::vector<std::vector<std::string>> messages;

	printf("cut scene number :%d\n", cut_scene_number);
	if (cut_scene_number == 0) {
		messages = {
			{
				"\"Daring Dungeons Game Jam\"",

			},
		};
	}

	if (cut_scene_number == 1) {
		messages = {
			{
				"\"Submit your dungeon-themed game here! Winners will be decided by",
				"a board of professional judges...\"",
			},
		};
	}

	if (cut_scene_number == 2) {
		messages = {
			{	
				"\"...$1000 cash reward, and a chance to work with a development team to",
				"further expand your game idea!.\"",
			},
		};
	}

	if (cut_scene_number == 3) { // happy
		messages = {
		{
			"Oh heck yes! I feel like I was made for this!",
			},
		};
	}
	
	if (cut_scene_number == 4) { // normal
		messages = {
		{
			"Wait...how long do I have to make a game?",
			},
		};
	}
	
	if (cut_scene_number == 5) { // surprised
		messages = {
			{
				"By the end of the month?!? Whoa whoa whoa!!",
			},
		};
	}
	
	if (cut_scene_number == 6) { // normal
		messages = {
			{
				"Okay, okay. I know I can handle this. I just have to start...right now!",
				
			},
		};
	}
	
	if (cut_scene_number == 7) { // normal + many hours of writing 
		messages = {
			{
				"*many hours of writing and brainstorming later...*",
			},
		};
	}
	
	if (cut_scene_number == 8) { // notebook 
		messages = {
			{
				"Alright! Now that's a lot of progress for now, but I've still got a ways ",
				"to go.",
			},
		};
	}

	if (cut_scene_number == 9) { // stomache growling
		messages = {
			{
				"*stomach growling*",
			},
		};
	}
	
	if (cut_scene_number == 10) { //[room] angry 
		messages = {
			{
				"Ugh, not now, not while I'm still in the groove!",
			},
		};
	}
	
	if (cut_scene_number == 11) { //[room] normal
		messages = {
			{
				"Just a bit more...",
			},
		};
	}
	
	if (cut_scene_number == 12) { //[room] normal
		messages = {
			{
				"Just...",
			},
		};
	}
	
	if (cut_scene_number == 13) { //[room] angry
		messages = {
			{
				"a...",
			},
		};
	}
	
	if (cut_scene_number == 14) { //[room] angry
		messages = {
			{
				"bit...",
			},
		};
	}
	
	if (cut_scene_number == 15) { //[room] angry
		messages = {
			{
				"mor-",
			},
		};
	}
	
	if (cut_scene_number == 16) { //screen fades to black
		messages = {
			{
				"",
			},
		};
	}
	
	if (cut_scene_number == 17) { // confused cave
		messages = {
			{
				"Where...? What...?",
			},
		};
	}
	
	if (cut_scene_number == 18) { //confused cave
		messages = {
			{
				"This place...where am I? And why...? I can't seem to remember...",
				
			},
		};
	}
	
	if (cut_scene_number == 19) { //screen fades to black
		messages = {
			{
				"Well, I guess the only way to find out is to look around.",
				
			},
		};
	}
	
	if (cut_scene_number == 20) { //screen fades to black
		messages = {
			{
				"There's gotta be something around here...",

			},
		};
	}

	activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.2), messages, true);
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
	cut_scene_text_log(renderer, "Press Esc to skip");
	
	if (cut_scene_number == 0 || cut_scene_number ==1 || cut_scene_number== 2) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		updateDialogue(renderer, cut_scene_number);
		//updateDialogue(renderer, cut_scene_number);
	}
	else if (cut_scene_number == 3) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::SHOU_HAPPY);
		updateDialogue(renderer, cut_scene_number);
		//updateDialogue(renderer, cut_scene_number);

	}
	else if (cut_scene_number == 7) { // * signs 
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		updateDialogue(renderer, cut_scene_number);
	}
	else if (cut_scene_number == 4 || cut_scene_number == 6 || cut_scene_number ==11||cut_scene_number == 12) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::SHOU_NORMAL);
		updateDialogue(renderer, cut_scene_number);
		//cut_scene_dialogue(renderer, "Press Escape to skip and go to Main Menu");
	}
	else if (cut_scene_number == 5) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::SHOU_SURPRISED);
		updateDialogue(renderer, cut_scene_number);

	} 
	else if (cut_scene_number == 8 || cut_scene_number == 9) { // notebook
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::BG_NOTEBOOK);
		updateDialogue(renderer, cut_scene_number);
	}
	else if (cut_scene_number == 10 || cut_scene_number == 13 || cut_scene_number == 14 || cut_scene_number ==15 ) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::SHOU_ANGRY);
		updateDialogue(renderer, cut_scene_number);

	}
	else if (cut_scene_number == 16){ // fade to black or main_menu
		Entity temp = Entity();
		FadeTransitionTimer& timer = registry.fadeTransitionTimers.emplace(temp);
		timer.type = TRANSITION_TYPE::CUTSCENE_SWITCH;
	}
	else if (cut_scene_number == 17 || cut_scene_number == 18) { // cave confused
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::BG_CAVE);
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::SHOU_CONFUSED);
		updateDialogue(renderer, cut_scene_number);
	}
	else if (cut_scene_number == 19| cut_scene_number == 20 ){// cave normal
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::BG_CAVE);
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::SHOU_NORMAL);
		updateDialogue(renderer, cut_scene_number);
	}

	printf("Scene Transition function \n");

}