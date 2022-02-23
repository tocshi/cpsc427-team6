#include "save_system.hpp"

void saveToFile(json j) {
	
	//auto j = json::parse(R"({"happy": true, "pi": 3.141})");
	

	std::ofstream o(data_path() + PATH_EXTENSION);
	o << std::setw(4) << j << std::endl;
}

void SaveSystem::saveGameState() {
	json saveState;
	json playerData;

	for (Entity e : registry.players.entities) {
		Player player = registry.players.get(e);
		playerData["hp"] = player.hp;
		playerData["ep"] = player.ep;
		playerData["maxEP"] = player.maxEP;
		playerData["mp"] = player.mp;
	}
	saveState["player"] = playerData;

	saveToFile(saveState);
}

