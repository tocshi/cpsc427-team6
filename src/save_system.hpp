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
	bool saveDataExists();
	json getSaveData();
	std::queue<Entity> getSolidTileInteract(std::queue<Entity> orignalqueue); // puts solid/ collidables obj + tileMAp into the entitylist
	json jsonifyPlayer(Entity player);
private:
	json jsonifyEntities(std::queue<Entity> entities);
	json jsonifyEnemy(Entity enemy);
	json jsonifyMotion(Motion m);
	json jsonifyStats(Stats s);
	json jsonifyQueueable(Queueable q);
	json jsonifyInventory(Entity inv);
	json jsonifyEquipment(Equipment e);
	json jsonifyInteractables();
	json jsonifyTiles();
	json jsonifyCollidables();
	json jsonifySign(Entity e);
	json jsonifyStatus(Entity e);
	json jsonifySwitch(Entity e);
	json jsonifyChest(Entity e);
	json jsonifySpritesheet(Spritesheet ss);
	json jsonifyArtifact(Artifact a);
	//json jsonify
};