#include "cutscene.hpp"
#include "world_system.hpp"
#include "world_init.hpp"



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




void CutSceneSystem::scene_transition(RenderSystem* renderer, int cut_scene_number) {
	cut_scene_text_log(renderer, "Press Escape to skip");
	if (cut_scene_number == 0) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		//cut_scene_dialogue(renderer, "Press Escape to Skip ...");
		Entity m1 = createMouseAnimation(renderer, { window_width_px / 2 - 64 * ui_scale, window_height_px / 2 - 2 * 64 * ui_scale });
		registry.hidables.emplace(m1);
		//createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::BG);
		//createCollectionButton(renderer, { window_width_px - 160.f, 50.f });
		//createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
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