#include "save_system.hpp"

void saveToFile(json j) {
	std::ofstream o(SAVE_DATA_PATH);
	o << std::setw(4) << j << std::endl;
}

void SaveSystem::readJsonFile(json j) {

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

	//for (Entity e: registry.)
	// current invetory of items 

	saveState["player"] = playerData;
	saveState["inventory"] = inventory;

	saveToFile(saveState);
}

