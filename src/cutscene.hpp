#pragma once
#include "render_system.hpp"

class CutSceneSystem
{
public:
	void cut_scene_text_log(RenderSystem* renderer, std::string msg);
	void scene_transition(RenderSystem* renderer, int cut_scene_count);
	void updateTextBox(RenderSystem* renderer, Entity activeTextbox);
	void updateDialogue(RenderSystem* renderer, int cut_scene_number);
private:
	Entity activeTextbox;
	
};