#include "tilemap.hpp"
#include "tiny_ecs_registry.hpp"

std::vector<Entity> TileMapParser::Parse(const std::string& file, RenderSystem *renderer, vec2 offset)
{
	char* fileLoc = new char[file.size() + 1]; // 1
	strcpy_s(fileLoc, file.size() + 1, file.c_str());
	//TODO: error checking - check file exists before attempting open.
	rapidxml::file<> xmlFile(fileLoc);
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());
	rapidxml::xml_node<>* rootNode = doc.first_node("map");
		// Loads tile layers from XML.
		std::shared_ptr<MapTiles> tiles = BuildMapTiles(rootNode, renderer);
	// We need these to calculate the tiles position in world space
	int tileSizeX = std::atoi(rootNode->first_attribute("tilewidth")->value());
	int tileSizeY = std::atoi(rootNode->first_attribute("tileheight")->value());
	int mapsizeX = std::atoi(rootNode->first_attribute("width")->value());
	int mapsizeY = std::atoi(rootNode->first_attribute("height")->value());

	int scaleFactor = 4; // TODO: determine based on tileset if we are using different tile sizes
	float uv_padding = 0.0001;
	// This will contain all of our tiles as objects.
	std::vector<Entity> tileObjects;
	// 2
	// We iterate through each layer in the tile map
	for (const auto& layer : *tiles)
	{
		// And each tile in the layer
		for (const auto& tile : *layer.second)
		{
			createTileFromData(tile, tileSizeX, tileSizeY, scaleFactor, uv_padding, std::string(layer.first));
		}
	}

	// Parse objects, which will be used for solid walls
	std::vector<MapObject> objects = BuildObjects(rootNode);
	for (MapObject& object : objects) {
		Entity entity = Entity();
		Motion& motion = registry.motions.emplace(entity);
		motion.scale = { object.objectRect.width * scaleFactor, object.objectRect.height * scaleFactor };
		motion.position.x = (object.objectRect.x + object.objectRect.width/2 - tileSizeX) * scaleFactor + offset.x;
		motion.position.y = (object.objectRect.y + object.objectRect.height/2) * scaleFactor + offset.y;
		registry.solid.emplace(entity);
		registry.collidables.emplace(entity);
	}

	// Choose randomized elements for walls
	for (rapidxml::xml_node<>* node = rootNode->first_node("group");
		node; node = node->next_sibling("group"))
	{
		if (node->first_attribute("name") != nullptr && std::string(node->first_attribute("name")->value()) == "randwalls") {
			// loop through the groups
			for (rapidxml::xml_node<>* subnode = node->last_node("group");
				subnode; subnode = subnode->previous_sibling("group"))
			{
				// use property of current group to determine how many of this group to spawn
				rapidxml::xml_node<>* properties = subnode->first_node("properties");
				if (properties == nullptr)
					continue;
				int min = 0;
				int max = 1;
				for (rapidxml::xml_node<>* property = properties->first_node("property");
					property; property = property->next_sibling("property"))
				{
					if (property->first_attribute("name") != nullptr &&
						std::string(property->first_attribute("name")->value()) == "min") {
						min = std::atoi(property->first_attribute("value")->value());
					}
					else if (property->first_attribute("name") != nullptr &&
						std::string(property->first_attribute("name")->value()) == "max") {
						max = std::atoi(property->first_attribute("value")->value()) + 1;
					}
				}

				// store the potential wall group nodes here
				std::vector<rapidxml::xml_node<>*> wallgroups = std::vector<rapidxml::xml_node<>*>();
				for (rapidxml::xml_node<>* wallgroup = subnode->first_node("group");
					wallgroup; wallgroup = wallgroup->next_sibling("group"))
				{
					wallgroups.push_back(wallgroup);
				}

				int select_count = std::min(irandRange(min, max), int(wallgroups.size()));
				// randomize the order, then pick select_count of them to spawn
				std::random_shuffle(wallgroups.begin(), wallgroups.end());
				for (int i = 0; i < select_count; i++) {
					for (rapidxml::xml_node<>* spawnlayer = wallgroups[i]->first_node();
						spawnlayer; spawnlayer = spawnlayer->next_sibling())
					{
						if (std::string(spawnlayer->name()) == "layer") {
							std::shared_ptr<TileSheetData> tileSheetData = BuildTileSheetData(rootNode, renderer);
							std::pair<std::string, std::shared_ptr<Layer>> layer = BuildLayer(spawnlayer, tileSheetData);
							// And each tile in the layer
							for (const auto& tile : *layer.second)
							{
								createTileFromData(tile, tileSizeX, tileSizeY, scaleFactor, uv_padding, "random_walls");
							}
						}
					}
					std::vector<MapObject> rand_objects = BuildObjects(wallgroups[i]);
					for (MapObject& object : rand_objects) {
						Entity entity = Entity();
						Motion& motion = registry.motions.emplace(entity);
						motion.scale = { object.objectRect.width * scaleFactor, object.objectRect.height * scaleFactor };
						motion.position.x = (object.objectRect.x + object.objectRect.width / 2 - tileSizeX) * scaleFactor + offset.x;
						motion.position.y = (object.objectRect.y + object.objectRect.height / 2) * scaleFactor + offset.y;
						registry.solid.emplace(entity);
						registry.collidables.emplace(entity);
					}
				}
			}
		}
	}

	// TODO: reset memory

	return tileObjects;
}


std::shared_ptr<MapTiles> TileMapParser::BuildMapTiles(rapidxml::xml_node<>* rootNode, RenderSystem *renderer)
{
	std::shared_ptr<TileSheetData> tileSheetData = BuildTileSheetData(rootNode, renderer);
	std::shared_ptr<MapTiles> map = std::make_shared<MapTiles>();
	// We loop through each layer in the XML document.
	for (rapidxml::xml_node<>* node = rootNode->first_node("layer");
		node; node = node->next_sibling("layer"))
	{
		std::pair<std::string, std::shared_ptr<Layer>> mapLayer =
			BuildLayer(node, tileSheetData);
		map->emplace(mapLayer);
	}
	return map;
}

std::shared_ptr<TileSheetData> TileMapParser::BuildTileSheetData(rapidxml::xml_node<>* rootNode, RenderSystem* renderer)
{
	TileSheetData tileSheetData;
	// Traverse to the tile set node.
	rapidxml::xml_node<>* tilesheetNode = rootNode->first_node("tileset");
		//TODO: add error checking to ensure these values actually exist.
		//TODO: add support for multiple tile sets.
		//TODO: implement this.
	int firstid = 0;
	if (tilesheetNode->first_attribute("firstgid") != nullptr) {
		firstid = std::atoi(tilesheetNode->first_attribute("firstgid")->value());
	}
	// Build the tile set data.
	tileSheetData.tileSize.x = std::atoi(tilesheetNode->first_attribute("tilewidth")->value());
	tileSheetData.tileSize.y = std::atoi(tilesheetNode->first_attribute("tileheight")->value());
	if (tilesheetNode->first_attribute("spacing") != nullptr) {
		tileSheetData.spacing = std::atoi(tilesheetNode->first_attribute("spacing")->value());
	}
	if (tilesheetNode->first_attribute("margin") != nullptr) {
		tileSheetData.margin = std::atoi(tilesheetNode->first_attribute("margin")->value());
	}
	
	tileSheetData.firstId = firstid;
	int tileCount = std::atoi(tilesheetNode->first_attribute("tilecount")->value());

	tileSheetData.columns = std::atoi(tilesheetNode->first_attribute("columns")->value());
	tileSheetData.rows = tileCount / tileSheetData.columns;
	rapidxml::xml_node<>*imageNode = tilesheetNode->first_node("image");
	// get the TEXTURE_ASSET_ID associated with the filepath
	std::string texturePath = std::string(imageNode->first_attribute("source")->value());
	tileSheetData.textureId = renderer->findTextureId(texturePath);

	//TODO: add error checking - we want to output a 
	//message if the texture is not found.
	tileSheetData.imageSize.x =
		std::atoi(imageNode->first_attribute("width")->value());
	tileSheetData.imageSize.y =
		std::atoi(imageNode->first_attribute("height")->value());
	return std::make_shared<TileSheetData>(tileSheetData);
}

std::pair<std::string, std::shared_ptr<Layer>>
TileMapParser::BuildLayer(rapidxml::xml_node<>* layerNode,
	std::shared_ptr<TileSheetData> tileSheetData)
{
	TileSet tileSet;
	std::shared_ptr<Layer> layer = std::make_shared<Layer>();
	int width = std::atoi(layerNode->first_attribute("width")->value());
	int height = std::atoi(layerNode->first_attribute("height")->value());
	vec2 offset = { 0,0 };
	if (layerNode->first_attribute("offsetx") != nullptr) {
		offset.x = std::atoi(layerNode->first_attribute("offsetx")->value());
	}
	if (layerNode->first_attribute("offsety") != nullptr) {
		offset.y = std::atoi(layerNode->first_attribute("offsety")->value());
	}
	rapidxml::xml_node<>* dataNode = layerNode->first_node("data");
	char* mapIndices = dataNode->value();
	std::stringstream fileStream(mapIndices);
	int count = 0;
	std::string line;
	while (fileStream.good())
	{
		std::string substr;
		std::getline(fileStream, substr, ','); // 1
			if (!IsInteger(substr)) // 2
			{
				// We remove special characters from the int before parsing
				substr.erase(
					std::remove(substr.begin(), substr.end(), '\r'), substr.end());
				substr.erase(
					std::remove(substr.begin(), substr.end(), '\n'), substr.end());
				//TODO: add additional check to 
				//confirm that the character removals have worked:
			}
		int tileId = std::stoi(substr); // 3
		if (tileId != 0) // 4
		{
			auto itr = tileSet.find(tileId); // 5
			if (itr == tileSet.end()) // 6
			{
				int textureX = (tileId - tileSheetData->firstId) % tileSheetData->columns;
				int textureY = (tileId - tileSheetData->firstId) / tileSheetData->columns;
				std::shared_ptr<TileInfo> tileInfo =
					std::make_shared<TileInfo>(
						tileSheetData->textureId, 
						tileId,
						tileSheetData->imageSize,
						Rect(
							textureX * (tileSheetData->tileSize.x + tileSheetData->spacing) + tileSheetData->margin,
							textureY * (tileSheetData->tileSize.y + tileSheetData->spacing) + tileSheetData->margin,
							tileSheetData->tileSize.x,
							tileSheetData->tileSize.y)
						);
				itr = tileSet.insert(std::make_pair(tileId, tileInfo)).first;
			}
			std::shared_ptr<Tile> tile = std::make_shared<Tile>();
			// Bind properties of a tile from a set.
			tile->properties = itr->second; // 7
			tile->offset = offset;
			tile->x = count % width - 1;
			tile->y = count / width;
			layer->emplace_back(tile); // 8
		}
		count++;
	}
	const std::string layerName = layerNode->first_attribute("name")->value();
	return std::make_pair(layerName, layer);
}

std::vector<MapObject> TileMapParser::BuildObjects(rapidxml::xml_node<>* rootNode) {
	std::vector<MapObject> objects = std::vector<MapObject>();
	// We loop through each layer in the XML document.
	for (rapidxml::xml_node<>* node = rootNode->first_node("objectgroup");
		node; node = node->next_sibling("objectgroup"))
	{
		if (std::string(node->first_attribute("name")->value()) == "walls" || 
			std::string(node->first_attribute("name")->value()) == "wall") {
			for (rapidxml::xml_node<>* objectnode = node->first_node("object");
				objectnode; objectnode = objectnode->next_sibling("object"))
			{
				MapObject object;
				object.objectId = std::atoi(objectnode->first_attribute("id")->value());
				object.objectRect = Rect(
					std::atoi(objectnode->first_attribute("x")->value()),
					std::atoi(objectnode->first_attribute("y")->value()),
					std::atoi(objectnode->first_attribute("width")->value()),
					std::atoi(objectnode->first_attribute("height")->value())
					);
				objects.push_back(object);
			}
		}
	}
	return objects;
}

Entity TileMapParser::createTileFromData(std::shared_ptr<Tile> tile, int tileSizeX, int tileSizeY, int scaleFactor, float uv_padding, std::string layer_name, vec2 offset) {
	std::shared_ptr<TileInfo> tileInfo = tile->properties;
	Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.scale = { tileSizeX * scaleFactor, tileSizeY * scaleFactor };
	motion.position.x = (scaleFactor * (tile->x * tileSizeX + tile->offset.x)) + (0.5f * motion.scale.x) + offset.x;
	motion.position.y = (scaleFactor * (tile->y * tileSizeY + tile->offset.y)) + (0.5f * motion.scale.y) + offset.y;

	TileUV& tileUV = registry.tileUVs.emplace(entity);
	tileUV.layer = layer_name;
	tileUV.tileID = tileInfo->tileID;
	tileUV.uv_start = {
		tileInfo->textureRect.x / tileInfo->textureSize.x + uv_padding,
		tileInfo->textureRect.y / tileInfo->textureSize.y + uv_padding };
	tileUV.uv_end = {
		(tileInfo->textureRect.x + tileInfo->textureRect.width) / tileInfo->textureSize.x - uv_padding,
		(tileInfo->textureRect.y + tileInfo->textureRect.height) / tileInfo->textureSize.y - uv_padding };

	RenderRequest renderRequest = {
		static_cast<TEXTURE_ASSET_ID>(tileInfo->textureID),
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::TILEMAP,
	};
	std::string layerName = layer_name;
	if (layerName == "floor deco") {
		renderRequest.used_layer = RENDER_LAYER_ID::FLOOR_DECO;
	}
	else if (layerName == "walls") {
		renderRequest.used_layer = RENDER_LAYER_ID::WALLS;
	}
	else if (layerName == "random_walls") {
		renderRequest.used_layer = RENDER_LAYER_ID::RANDOM_WALLS;
	}
	else {
		renderRequest.used_layer = RENDER_LAYER_ID::FLOOR;
	}
	registry.renderRequests.insert(entity, renderRequest);
	return entity;
}