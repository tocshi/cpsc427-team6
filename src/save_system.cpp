#include "save_system.hpp"


void saveToFile(json j) {
	std::ofstream o(SAVE_DATA_PATH);
	o << std::setw(4) << j << std::endl;
}

bool SaveSystem::saveDataExists() {
	std::ifstream infile(SAVE_DATA_PATH);
	return infile.good();
}

json SaveSystem::getSaveData() {
	// read from the save data file
	std::ifstream i(SAVE_DATA_PATH);
	json j;
	i >> j;
	return j;
}

void SaveSystem::saveGameState(std::queue<Entity> entities) {
	json saveState;
	std::queue<Entity> resultList;
	resultList = getSolidTileInteract(entities);
	saveState["entities"] = jsonifyEntities(entities);
	//saveState["entities"] = jsonifyEntities(resultList);
	saveState["map"]["collidables"] = jsonifyCollidables();
	saveState["map"]["interactables"] = jsonifyInteractables();
	saveState["map"]["tiles"] = jsonifyTiles();

	saveToFile(saveState);
}

std::queue<Entity> SaveSystem::getSolidTileInteract(std::queue<Entity> originalqueue)
{
	
	printf("%d size of list beginning\n", originalqueue.size());
	//int collideTotal = 0;
	for (Entity collide : registry.collidables.entities) {
		//int collideTotal++;
		if (registry.solid.has(collide)) { // is solid + collidable
			//printf("has a solid + collid is may be a wall\n");
			originalqueue.push(collide); 
		}
		// code to check if it is in the entitylist 
	}

	printf("%d size of list before tiles added \n:", originalqueue.size());
	for (Entity tiles : registry.tileUVs.entities) { // push tiles into the entity list
		 
		originalqueue.push(tiles);
	}
	printf("%d size of list after tiles added \n:", originalqueue.size());
	
	
	for (Entity interact : registry.interactables.entities) {
		originalqueue.push(interact); // add interact to queue
	}
	printf("%d size of list after interact added \n:", originalqueue.size());
	return originalqueue;
}

json SaveSystem::jsonifyEntities(std::queue<Entity> entities) {
	auto entityList = json::array();
	// loop through all the entities in order of turn system and save them
	while (entities.size() != 0) {
		Entity e = entities.front();
		entities.pop();
		json entity;
		// check to see what type of entity it is
		if (registry.players.has(e)) {
			entity = jsonifyPlayer(e);
		}
		else if (registry.enemies.has(e)) {
			entity = jsonifyEnemy(e);
			//printf("123 working?? \n");
		}
		entityList.push_back(entity);
	}

	return entityList;
}

json SaveSystem::jsonifyQueueable(Queueable q) {
	json queueable;
	queueable["doingTurn"] = q.doing_turn;
	return queueable;
}

json SaveSystem::jsonifyStats(Stats s) {
	json stats;
	stats["hp"] = s.hp;
	stats["maxHP"] = s.maxhp;

	stats["ep"] = s.ep;
	stats["maxEP"] = s.maxep;
	
	stats["mp"] = s.mp;
	stats["maxMP"] = s.maxmp;
	stats["mpregen"] = s.mpregen;
	
	stats["atk"] = s.atk;
	stats["def"] = s.def;

	stats["speed"] = s.speed;
	stats["range"] = s.range;
	stats["chase"] = s.chase;
	stats["guard"] = s.guard;

	stats["epratemove"] = s.epratemove;
	stats["eprateatk"] = s.eprateatk;

	return stats;
}

json SaveSystem::jsonifyMotion(Motion m) {
	json motion;
	motion["position_x"] = m.position.x;
	motion["position_y"] = m.position.y;
	motion["angle"] = m.angle;
	motion["velocity_x"] = m.velocity.x;
	motion["velocity_y"] = m.velocity.y;
	motion["destination_x"] = m.destination.x;
	motion["destination_y"] = m.destination.y;
	motion["movement_speed"] = m.movement_speed;
	motion["in_motion"] = m.in_motion;
	motion["scale"]["x"] = m.scale.x;
	motion["scale"]["y"] = m.scale.y;
	return motion;
}

json SaveSystem::jsonifyPlayer(Entity player) {
	json playerData;
	// set the entity type
	playerData["type"] = "player";

	// save player component stuff
	Player p = registry.players.get(player);
	json pl;

	pl["attacked"] = p.attacked;
	pl["gacha_pity"] = p.gacha_pity;
	pl["floor"] = p.floor;
	pl["room"] = p.room;
	pl["total_rooms"] = p.total_rooms;
	playerData["player"] = pl;

	// jsonify queueable
	Queueable q = registry.queueables.get(player);
	playerData["queueable"] = jsonifyQueueable(q);

	// jsonify stats
	Stats player_stats = registry.stats.get(player);
	playerData["stats"] = jsonifyStats(player_stats);

	// jsonify motion
	Motion player_motion = registry.motions.get(player);
	playerData["motion"] = jsonifyMotion(player_motion);

	// jsonify inventory
	playerData["inventory"] = jsonifyInventory(player);

	// jsonify statuses
	playerData["statuses"] = jsonifyStatus(player);

	return playerData;
}

// add plantshooter 
json SaveSystem::jsonifyEnemy(Entity enemy) {
	json enemyData;

	enemyData["type"] = "enemy";

	Enemy e = registry.enemies.get(enemy);
	json enemyEnemy;
	// save enemy component stuff
	enemyEnemy["state"] = e.state;
	enemyEnemy["hit_by_player"] = e.hit_by_player;
	enemyEnemy["type"] = e.type;
	enemyData["enemy"] = enemyEnemy;

	// jsonify queueable stuff
	Queueable q = registry.queueables.get(enemy);
	enemyData["queueable"] = jsonifyQueueable(q);

	// jsonify stats
	Stats enemy_stats = registry.stats.get(enemy);
	enemyData["stats"] = jsonifyStats(enemy_stats);

	// jsonify motion
	Motion m = registry.motions.get(enemy);
	enemyData["motion"] = jsonifyMotion(m);

	// jsonify inventory
	enemyData["inventory"] = jsonifyInventory(enemy);

	// jsonify statuses
	enemyData["statuses"] = jsonifyStatus(enemy);

	return enemyData;
}

json SaveSystem::jsonifyInventory(Entity inv) {
	//json inventoryData; 

	Inventory i = registry.inventories.get(inv);
	json inventory;
	inventory["artifact"] = i.artifact;
	inventory["consumable"] = i.consumable;	
	inventory["equipped"]["weapon"] = jsonifyEquipment(i.equipped[0]);
	inventory["equipped"]["armour"] = jsonifyEquipment(i.equipped[1]); 
	return inventory; 
}


json SaveSystem::jsonifyEquipment(Equipment e) {
	json equipt; 
	equipt["type"] = e.type;
	equipt["atk"] = e.atk;
	equipt["def"] = e.def;
	equipt["speed"] = e.speed;
	equipt["ep"] = e.ep;
	equipt["mp"] = e.ep;
	equipt["hp"] = e.mp;
	equipt["range"] = e.range;
	equipt["sprite"] = e.sprite;
	auto attacks = json::array();
	for (ATTACK attack : e.attacks) {
		attacks.push_back(attack);
	}
	equipt["attacks"] = attacks;
	return equipt;
}

json SaveSystem::jsonifySpritesheet(Spritesheet ss) {
	json spritesheetJson;
	spritesheetJson["texture"] = ss.texture;
	spritesheetJson["height"] = ss.height;
	spritesheetJson["width"] = ss.width;
	spritesheetJson["rows"] = ss.rows;
	spritesheetJson["columns"] = ss.columns;
	spritesheetJson["frame_size"]["x"] = ss.frame_size.x;
	spritesheetJson["frame_size"]["y"] = ss.frame_size.y;
	spritesheetJson["index"] = ss.index;
	return spritesheetJson;
}

json SaveSystem::jsonifyStatus(Entity e) {
	json statusArrayJson = json::array();
	StatusContainer sc = registry.statuses.get(e);
	for (StatusEffect status : sc.statuses) {
		json statusJson;
		statusJson["value"] = status.value;
		statusJson["turns_remaining"] = status.turns_remaining;
		statusJson["effect"] = status.effect;
		statusJson["percentage"] = status.percentage;
		statusJson["apply_at_turn_start"] = status.apply_at_turn_start;
		statusArrayJson.push_back(statusJson);
	}
	return statusArrayJson;
}

json SaveSystem::jsonifyInteractables() {
	auto interactablesList = json::array();
	for (Entity entity : registry.interactables.entities) {
		json interactableJson;
		Interactable interactable = registry.interactables.get(entity);
		interactableJson["type"] = interactable.type;
		Motion m = registry.motions.get(entity);
		interactableJson["motion"] = jsonifyMotion(m);
		if (interactable.type == INTERACT_TYPE::SIGN) {
			interactableJson["sign"] = jsonifySign(entity);
		}
		else if (interactable.type == INTERACT_TYPE::SWITCH) {
			interactableJson["switch"] = jsonifySwitch(entity);
		}
		else if (interactable.type == INTERACT_TYPE::ARTIFACT_CHEST || interactable.type == INTERACT_TYPE::ITEM_CHEST) {
			interactableJson["chest"] = jsonifyChest(entity);
		}
		else if (interactable.type == INTERACT_TYPE::PICKUP) {
			if (registry.equipment.has(entity)) {
				Equipment eq = registry.equipment.get(entity);
				interactableJson["equipment"] = jsonifyEquipment(eq);
				Spritesheet ss = registry.spritesheets.get(entity);
				interactableJson["spritesheet"] = jsonifySpritesheet(ss);
			}
			else if (registry.artifacts.has(entity)) {
				Artifact a = registry.artifacts.get(entity);
				interactableJson["artifact"] = jsonifyArtifact(a);
			}
		}
		interactablesList.push_back(interactableJson);
	}
	return interactablesList;
}

json SaveSystem::jsonifyCollidables() {
	// right now the only collidables are walls
	auto collidablesList = json::array();
	for (Entity entity : registry.collidables.entities) {
		json collidableJson;
		// check to see if solid, if not skip
		if (!registry.solid.has(entity)) {
			continue;
		}
		Motion m = registry.motions.get(entity);
		collidableJson["motion"] = jsonifyMotion(m);
		collidablesList.push_back(collidableJson);
	}
	return collidablesList;
}

json SaveSystem::jsonifyTiles() {
	auto tilesList = json::array();
	for (Entity tile : registry.tileUVs.entities) {
		json tileJson;
		TileUV tileUV = registry.tileUVs.get(tile);
		Motion m = registry.motions.get(tile);
		RenderRequest rr = registry.renderRequests.get(tile);
		tileJson["motion"] = jsonifyMotion(m);
		tileJson["tileUV"]["layer"] = tileUV.layer;
		tileJson["tileUV"]["tileID"] = tileUV.tileID;
		tileJson["tileUV"]["uv_start"]["x"] = tileUV.uv_start.x;
		tileJson["tileUV"]["uv_start"]["y"] = tileUV.uv_start.y;
		tileJson["tileUV"]["uv_end"]["x"] = tileUV.uv_end.x;
		tileJson["tileUV"]["uv_end"]["y"] = tileUV.uv_end.y;
		tileJson["renderRequest"]["used_texture"] = (int)rr.used_texture;
		tileJson["renderRequest"]["used_layer"] = (int)rr.used_layer;
		tilesList.push_back(tileJson);
	}
	return tilesList;
}

json SaveSystem::jsonifySign(Entity e) {
	json signJson;
	auto messageList = json::array();
	Sign sign = registry.signs.get(e);
	for (std::pair<std::string, int> message : sign.messages) {
		json messageJson;
		messageJson["message"] = message.first;
		messageJson["number"] = message.second;
		messageList.push_back(messageJson);
	}
	signJson["messages"] = messageList;
	return signJson;
}

json SaveSystem::jsonifySwitch(Entity e) {
	Switch s = registry.switches.get(e);

	json switchJson;
	switchJson["activated"] = s.activated;
	return switchJson;
}

json SaveSystem::jsonifyChest(Entity e) {
	Chest chest = registry.chests.get(e);

	json chestJson;
	chestJson["isArtifact"] = chest.isArtifact;
	chestJson["opened"] = chest.opened;
	return chestJson;
}

json SaveSystem::jsonifyArtifact(Artifact a) {
	json artifactJson;
	artifactJson["type"] = a.type;
	return artifactJson;
}