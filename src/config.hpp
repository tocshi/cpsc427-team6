#include <../ext/json/single_include/nlohmann/json.hpp>
#include <iostream>

const std::string SAVE_CONFIG_PATH= data_path() + "/config.json";

using json = nlohmann::json;


class ConfigSystem {

public:
	json getConfigData();

	void saveConfigData(int height, int window_width, int ui_scale);
	json getConfig(json data);
	void testConfig();

};