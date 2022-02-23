#pragma once

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

#include <../ext/json/single_include/nlohmann/json.hpp>

const std::string SAVE_DATA_PATH = data_path() + "/saveData.json";

using json = nlohmann::json;

class SaveSystem
{
public:
	void saveGameState();
	void readJsonFile();
};