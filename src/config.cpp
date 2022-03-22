#include "config.hpp"

void saveConfigToFile(json j) {

	std::ofstream o(SAVE_CONFIG_PATH);
    o << std::setw(4) << j << std::endl;
}

json ConfigSystem::getConfigData() {
	// read from the save data file
	std::ifstream i(SAVE_CONFIG_DATA);
	json j;
	i >> j;
	return j;
}

void ConfigSystem::getConfig(json data) {
	int window_height_px = data["window_height_px"];
	int window_width_px = data["window_width_px"]; 
	int ui_scale = data["ui_scale"];
	printf("%d windo_height:", window_height_px);
	printf("%d windo_width:", window_width_px);
	printf("%d ui_scale:", ui_scale);
}

void ConfigSystem::saveConfigData( int height,int width,int ui_scale) {

	json configData;
	configData["window_width_px"] = width;
	configData["window_height_px"] = height;
	configData["ui_scale"] = ui_scale;

	// save the data to json file
	saveConfigToFile(configData);

}

void ConfigSystem::testConfig() {
	saveConfigData(1800, 900, 2.4);
	json data = getConfigData();
	getConfig(data);
}


