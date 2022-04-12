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
	while (registry.textboxes.entities.size() > 0) {
		registry.remove_all_components_of(registry.textboxes.entities.back());
	}

	printf("cut scene number :%d\n", cut_scene_number);
	if (cut_scene_number == 0) {
		std::vector<std::vector<std::string>> messages = {
			{
				"''Daring Dungeons Game Jam''",

			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.255),messages);
	}
	
	if (cut_scene_number == 1) {
		std::vector<std::vector<std::string>> messages = {
			{
				"''Submit your dungeon-themed game here! Winners will be decided by",
				"a board of professional judges...''",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}

	if (cut_scene_number == 2) {
		std::vector<std::vector<std::string>> messages = {
			{	
				"''...$1000 cash reward, and a chance to work with a development team to",
				"further expand your game idea!.''",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25),messages);
	}

	if (cut_scene_number == 3) { // happy
		std::vector<std::vector<std::string>> messages = {
		{
			"Oh heck yes! I feel like I was made for this!",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25),messages);
	}
	
	if (cut_scene_number == 4) { // normal
		std::vector<std::vector<std::string>> messages = {
		{
			"Wait...how long do I have to make a game?",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25),messages);
	}
	
	if (cut_scene_number == 5) { // surprised
		std::vector<std::vector<std::string>> messages = {
			{
				" By the end of the month?!? Whoa whoa whoa!!",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 6) { // normal
		std::vector<std::vector<std::string>> messages = {
			{
				"Okay, okay. I know I can handle this. I just have to start...right now!",
				
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 7) { // normal + many hours of writing 
		std::vector<std::vector<std::string>> messages = {
			{
				"*many hours of writing and brainstorming later...*",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 8) { // notebook 
		std::vector<std::vector<std::string>> messages = {
			{
				"Alright! Now that’s a lot of progress for now, but I’ve still got a ways ",
				"to go.",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}

	if (cut_scene_number == 9) { // stomache growling
		std::vector<std::vector<std::string>> messages = {
			{
				"*stomach growling*",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 10) { //[room] angry 
		std::vector<std::vector<std::string>> messages = {
			{
				"Ugh, not now, not while I’m still in the groove!",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 11) { //[room] normal
		std::vector<std::vector<std::string>> messages = {
			{
				"Just a bit more...",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 12) { //[room] normal
		std::vector<std::vector<std::string>> messages = {
			{
				"Just...",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 13) { //[room] angry
		std::vector<std::vector<std::string>> messages = {
			{
				"a...",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 14) { //[room] angry
		std::vector<std::vector<std::string>> messages = {
			{
				"bit...",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 15) { //[room] angry
		std::vector<std::vector<std::string>> messages = {
			{
				"mor-",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 16) { //screen fades to black
		std::vector<std::vector<std::string>> messages = {
			{
				"*screen fades to black*",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 17) { // confused cave
		std::vector<std::vector<std::string>> messages = {
			{
				"Where...? What...?",
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 18) { //confused cave
		std::vector<std::vector<std::string>> messages = {
			{
				"This place...where am I? And why...? I can’t seem to remember...",
				
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 19) { //screen fades to black
		std::vector<std::vector<std::string>> messages = {
			{
				"Well, I guess the only way to find out is to look around.",
				
			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
	}
	
	if (cut_scene_number == 20) { //screen fades to black
		std::vector<std::vector<std::string>> messages = {
			{
				"There’s gotta be something around here…",

			},
		};
		activeTextbox = createTextbox(renderer, vec2(window_width_px / 2, window_height_px / 1.25), messages);
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
	
		printf("screen should fade to black");
		updateDialogue(renderer, cut_scene_number);
	
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