#pragma once

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

#include <../ext/json/single_include/nlohmann/json.hpp>
#include <iostream>
#include <queue>


const std::string SAVE_DATA_PATH = data_path() + "/saveData.json";

using json = nlohmann::json;

class SaveSystem
{
public:
	void saveGameState(std::queue<Entity> entities);
	void readJsonFile();
	bool saveDataExists();
	json getSaveData();
private:
	json jsonifyEntities(std::queue<Entity> entities);
	json jsonifyPlayer(Entity player);
	json jsonifyEnemy(Entity enemy);
	json jsonifyMotion(Motion m);
	json jsonifyStats(Stats s);
	json jsonifyQueueable(Queueable q);
	//json jsonify
};