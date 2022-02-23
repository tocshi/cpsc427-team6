#include "save_system.hpp"

void saveToFile(json j) {
	std::ofstream o(SAVE_DATA_PATH);
	o << std::setw(4) << j << std::endl;
}

void SaveSystem::saveGameState() {
	json saveState;
	json playerData;

	// equipped items
	json equipData;

	//item iventory 
	json consumeData;


	for (Entity e : registry.players.entities) {
		Player player = registry.players.get(e);
		playerData["hp"] = player.hp;
		playerData["ep"] = player.ep;
		playerData["maxEP"] = player.maxEP;
		playerData["mp"] = player.mp;
		
	}
	// Equipe items
	printf("LEFT PLAYER LOOP");
	for (Entity e : registry.equipables.entities) {
		Equipable equipItem = registry.equipables.get(e);
		printf("IN equiptable items");

		// item axe avilable add to stat
		if (equipItem.axe) {
			printf("true has axe");
			equipData["axe"] = true;
		}
		if (equipItem.sword) {
			equipData["sword"]= true;

		}
		if (equipItem.wand) {
			equipData["wand"] = true;
		}
	}

	for (Entity e : registry.consumables.entities) {
		Consumable consumableItem = registry.consumables.get(e);

		if (consumableItem.hp_potion) {
			consumeData["hp_potion"] = true;
		}
		if (consumableItem.magic_potion) {
			consumeData["mp_potion"] = true;
		}
	}

	//for (Entity e: registry.)
	// current invetory of items 

	saveState["player"] = playerData;
	saveState["Iventory"]["Equiptment"] = equipData;
	saveState["Inventory"]["Consumable"] = consumeData;

	saveToFile(saveState);
}

