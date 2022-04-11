#include "cutscene.hpp"
#include "world_system.hpp"
#include "world_init.hpp"



void CutSceneSystem::cut_scene_dialogue() {

	printf("Cut Scene Dialogu function \n");

}


void CutSceneSystem::scene_transition(RenderSystem* renderer, int cut_scene_number) {

	switch (cut_scene_number) {
	case 0:
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE1);
		printf("the cutscene 1 and cutscene count is :%d\n", cut_scene_number);
		break;

	case 1:
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE2);
		printf("the cutscene 2 and cutscene count is :%d\n", cut_scene_number);
		break;

	case 2:
		createCutScene(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::CUTSCENE3);
		printf("the cutscene 3 and cutscene count is :%d\n", cut_scene_number);
		break;
	}

	printf("Scene Transition function \n");

}