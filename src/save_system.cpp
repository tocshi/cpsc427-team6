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
		registry.players.get(e).ep = registry.players.get(e).maxEP;
		registry.players.get(e).hp = hp;
		registry.players.get(e).mp = mp;

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

	for (Entity e : registry.equipables.entities) {
		registry.equipables.get(e).axe = axe;
		registry.equipables.get(e).wand = wand;
		registry.equipables.get(e).sword = sword;
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

void SaveSystem::saveGameState() {
	json saveState;
	json playerData;

	
	// equipped items
	json inventory;


	for (Entity e : registry.players.entities) {
		Player player = registry.players.get(e);
		playerData["stats"]["hp"] = player.hp;
		playerData["stats"]["ep"] = player.ep;
		playerData["stats"]["maxEP"] = player.maxEP;
		playerData["stats"]["mp"] = player.mp;
		
		Motion player_motion = registry.motions.get(e);

		playerData["motion"]["position_x"] = player_motion.position.x;
		playerData["motion"]["position_y"] = player_motion.position.y;
		playerData["motion"]["angle"] = player_motion.angle;
		playerData["motion"]["velocity_x"] = player_motion.velocity.x;
		playerData["motion"]["velocity_y"] = player_motion.velocity.y;
		playerData["motion"]["destination_x"] = player_motion.destination.x;
		playerData["motion"]["destination_y"] = player_motion.destination.y;
		playerData["motion"]["movement_speed"] = player_motion.movement_speed;
		playerData["motion"]["in_motion"] = player_motion.in_motion;
			// get player position
	 
		
	}
	// Equipment items
	printf("LEFT PLAYER LOOP");
	for (Entity e : registry.equipables.entities) {
		Equipable equipItem = registry.equipables.get(e);
		printf("IN equiptable items");

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
		}
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

	//for (Entity e: registry.)
	// current invetory of items 

	saveState["player"] = playerData;
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

