#include "cutscene.hpp"
#include "world_system.hpp"
#include "world_init.hpp"



void CutSceneSystem::cut_scene_dialogue() {

	printf("Cut Scene Dialogu function \n");

}


void CutSceneSystem::scene_transition(RenderSystem* renderer, int cut_scene_number) {

	if (cut_scene_number == 0) {
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		//createBackground(renderer, vec2(window_width_px / 2, window_height_px / 2));
		//createMenuStart(renderer, { window_width_px / 2, 400.f * ui_scale });
		//createMenuContinue(renderer, { window_width_px / 2, 600.f * ui_scale });
		//createMenuQuit(renderer, { window_width_px / 2, 800.f * ui_scale });
		createMenuTitle(renderer, { window_width_px / 2, 150.f * ui_scale });
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
	}

	switch (cut_scene_number) {
	case 0:
		
		printf("the cutscene 1 and cutscene count is :%d\n", cut_scene_number);
		break;

	case 1:
		
		printf("the cutscene 2 and cutscene count is :%d\n", cut_scene_number);
		break;

	case 2:
		
		printf("the cutscene 3 and cutscene count is :%d\n", cut_scene_number);
		break;
	}

	printf("Scene Transition function \n");

}