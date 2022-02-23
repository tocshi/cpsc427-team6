#include "tiny_ecs_registry.hpp"
#include "common.hpp"

#include <../ext/json/single_include/nlohmann/json.hpp>

constexpr auto PATH_EXTENSION = "/saveData.json";

using json = nlohmann::json;

class SaveSystem
{
public:
	void saveGameState();
};