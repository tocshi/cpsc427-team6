#include "save_system.hpp"

void saveToFile(json j) {
	std::ofstream o(SAVE_DATA_PATH);
	o << std::setw(4) << j << std::endl;
}

// similar function already written, but keep for now
void setPlayer(json obj) {

	float hp = obj["player"]["stats"]["hp"].get<float>();
	float ep = obj["player"]["stats"]["ep"].get<float>();
	float mp = obj["player"]["stats"]["mp"].get<float>();

	// position
	float posX = obj["player"]["motion"]["position_x"].get<float>();
	float posY = obj["player"]["motion"]["position_y"].get<float>();
	//angle
	float angle = obj["player"]["motion"]["angle"].get<float>();
	// veloicty 
	float velx= obj["player"]["motion"]["velocity_x"].get<float>();
	float vely = obj["player"]["motion"]["velocity_y"].get<float>();
	
	// dest
	float destx = obj["player"]["motion"]["destination_x"].get<float>();
	float desty = obj["player"]["motion"]["destination_y"].get<float>();
	float movementSpeed = obj["player"]["motion"]["movement_speed"].get<float>();
	bool in_motion = obj["player"]["motion"]["in_motion"].get < bool>();

	// scale i

	for (Entity e: registry.players.entities) {
		// we are heading to a new room so EP is max
		registry.stats.get(e).ep = registry.stats.get(e).maxep;
		registry.stats.get(e).hp = hp;
		registry.stats.get(e).mp = mp;

		// setting the motion of the player 
		registry.motions.get(e).angle = angle;
		registry.motions.get(e).velocity = { velx,vely };
		registry.motions.get(e).position = { posX,posY };
		registry.motions.get(e).in_motion = in_motion;
		registry.motions.get(e).movement_speed = movementSpeed;
		registry.motions.get(e).destination = { destx,desty };
	}
}

void setArtifacts(json obj) {
	bool artifact = obj["inventory"]["artifact_1"].get<bool>();
	for (Entity e : registry.artifacts.entities) {
		registry.artifacts.get(e).artifact1 = artifact;

	}
}

void setInventory(json obj) {
	bool axe = obj["inventory"]["equiptment"]["axe"].get<bool>();
	bool wand = obj["inventory"]["equiptment"]["wand"].get<bool>();
	bool sword = obj["inventory"]["equiptment"]["sword"].get<bool>();

	// consume 
	bool hp_potion = obj["inventory"]["consumables"]["hp_potion"].get<bool>();
	bool mp_potion= obj["inventory"]["consumables"]["mp_potion"].get<bool>();

	for (Entity e : registry.equipment.entities) {
		/*
		registry.equipables.get(e).axe = axe;
		registry.equipables.get(e).wand = wand;
		registry.equipables.get(e).sword = sword;
		*/
	}

	for (Entity e : registry.consumables.entities) {
		registry.consumables.get(e).magic_potion = mp_potion;
		registry.consumables.get(e).hp_potion = hp_potion;
	}
}
void SaveSystem::readJsonFile() {
	//reading data 
	std::ifstream fileToRead(SAVE_DATA_PATH);
	//json jf = json::parse(fileToRead);
	json obj;
	fileToRead >> obj;
	//float x = obj["player"]["stats"]["hp"].get<float>();
	//printf("%f", x);
	printf("start the setting for saved data to new room");
	// similar function written so use other one for now 
	//setPlayer(obj);
	setArtifacts(obj);
	setInventory(obj);

	printf("\n");
	printf("DONE READING FILE");


}

void SaveSystem::saveGameState(std::queue<Entity> entities) {
	json saveState;
	
	// equipped items
	json inventory;
	
	// Equipment items
	for (Entity e : registry.equipment.entities) {
		Equipment equipItem = registry.equipment.get(e);
		printf("IN equiptable items");

		/*
		// item axe avilable add to stat
		if (equipItem.axe) {
			//printf("true has axe");
			inventory["equiptment"]["axe"] = true;
		}
		if (equipItem.sword) {
			inventory["equiptment"]["sword"]= true;

		}
		if (equipItem.wand) {
			inventory["equiptment"]["wand"] = true;
		}*/
	}

	for (Entity e : registry.consumables.entities) {
		Consumable consumableItem = registry.consumables.get(e);

		// available items in inventory
		if (consumableItem.hp_potion) {
			inventory["consumables"]["hp_potion"] = true;
		}
		if (consumableItem.magic_potion) {
			inventory["consumables"]["mp_potion"] = true;
		}
	}

	for (Entity e : registry.artifacts.entities) {
		Artifact artifacts = registry.artifacts.get(e);
		if (artifacts.artifact1) {
			inventory["artifact_1"] = artifacts.artifact1;
		}
	}

	saveState["entities"] = jsonifyEntities(entities);
	saveState["inventory"] = inventory;

	saveToFile(saveState);
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

json SaveSystem::jsonifyEntities(std::queue<Entity> entities) {
	auto entityList = json::array();
	// loop through all the entities in order of turn system and save them
	while (entities.size() != 0) {
		Entity e = entities.front();
		entities.pop();
		json entity;
		bool changed = false;
		// check to see what type of entity it is
		if (registry.players.has(e)) {
			changed = true;
			entity = jsonifyPlayer(e);
		}
		else if (registry.enemies.has(e)) {
			changed = true;
			entity = jsonifyEnemy(e);
		}

		// if something was actually jsonified put it into the array (removes nulls)
		if (changed) {
			// put entity json onto json array
			entityList.push_back(entity);
		}
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
	stats["gaurd"] = s.guard;

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

	playerData["invetory"] = jsonifyInventory(player);
	return playerData;
}

// add plantshooter 
json SaveSystem::jsonifyEnemy(Entity enemy) {
	json enemyData;
	// save entity type
	//enemyData["type"] = "slime";
	
	// save slime enemy component stuff
	
	if (registry.enemies.get(enemy).type == ENEMY_TYPE::SLIME) {
		enemyData["type"] = "slime";
	}
	else if (registry.enemies.get(enemy).type == ENEMY_TYPE::PLANT_SHOOTER) {
		enemyData["type"] = "plantShooter";
	}

	Enemy e = registry.enemies.get(enemy);
	json enemyEnemy;
	enemyEnemy["state"] = e.state;
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

	enemyData["hit_by_enemy"] = e.hit_by_player;
	enemyData["inventory"] = jsonifyInventory(enemy);

	return enemyData;
}

json SaveSystem::jsonifyInventory(Entity inv) {
	//json inventoryData; 

	Inventory i = registry.inventories.get(inv);
	json inventory;
	inventory["artifact"] = i.artifact;
	inventory["consumble"] = i.consumable;	
	inventory["equipped"]["Weapon"] = jsonifyEquiptment(i.equipped[0]);
	inventory["equipped"]["Armour"] = jsonifyEquiptment(i.equipped[1]); 
	return inventory; 
}


json SaveSystem::jsonifyEquiptment(Equipment e) {
	json equipt; 
	equipt["type"] = e.type;
	equipt["attack"] = e.atk;
	equipt["ep"] = e.ep;
	equipt["mp"] = e.ep;
	equipt["hp"] = e.mp;
	return equipt;
}

