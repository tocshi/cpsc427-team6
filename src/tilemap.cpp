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
	// This will contain all of our tiles as objects.
	std::vector<Entity> tileObjects;
	// 2
	// We iterate through each layer in the tile map
	for (const auto& layer : *tiles)
	{
		// And each tile in the layer
		for (const auto& tile : *layer.second)
		{
			std::shared_ptr<TileInfo> tileInfo = tile->properties;
			Entity entity = Entity();

			Motion& motion = registry.motions.emplace(entity);
			motion.position.x = tile->x * tileSizeX + offset.x;
			motion.position.y = tile->y * tileSizeY + offset.y;

			TileUV& tileUV = registry.tileUVs.emplace(entity);
			tileUV.uv_start = { 
				tileInfo->textureRect.x / tileInfo->textureSize.x, 
				tileInfo->textureRect.y / tileInfo->textureSize.y };
			tileUV.uv_end = {
				(tileInfo->textureRect.x + tileInfo->textureRect.width) / tileInfo->textureSize.x,
				(tileInfo->textureRect.y + tileInfo->textureRect.height) / tileInfo->textureSize.y };
			
			registry.renderRequests.insert(entity, { 
				static_cast<TEXTURE_ASSET_ID>(tileInfo->textureID),
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE,
				RENDER_LAYER_ID::BG
			});
		}
	}
	return tileObjects;
}


std::shared_ptr<MapTiles> TileMapParser::BuildMapTiles(rapidxml::xml_node<>* rootNode, RenderSystem *renderer)
{
	std::shared_ptr<TileSheetData> tileSheetData = BuildTileSheetData(rootNode, renderer);
	std::shared_ptr<MapTiles> map = std::make_shared<MapTiles>();
	// We loop through each layer in the XML document.
	for (rapidxml::xml_node<>* node = rootNode->first_node("layer");
		node; node = node->next_sibling())
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
		int firstid = std::atoi(tilesheetNode->first_attribute("firstgid")->value());
	// Build the tile set data.
	tileSheetData.tileSize.x = std::atoi(tilesheetNode->first_attribute("tilewidth")->value());
	tileSheetData.tileSize.y = std::atoi(tilesheetNode->first_attribute("tileheight")->value());
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
				int textureX = tileId % tileSheetData->columns - 1;
				int textureY = tileId / tileSheetData->columns;
				std::shared_ptr<TileInfo> tileInfo =
					std::make_shared<TileInfo>(
						tileSheetData->textureId, 
						tileId,
						tileSheetData->imageSize,
						Rect(
							textureX * tileSheetData->tileSize.x,
							textureY * tileSheetData->tileSize.y,
							tileSheetData->tileSize.x,
							tileSheetData->tileSize.y)
						);
				itr = tileSet.insert(std::make_pair(tileId, tileInfo)).first;
			}
			std::shared_ptr<Tile> tile = std::make_shared<Tile>();
			// Bind properties of a tile from a set.
			tile->properties = itr->second; // 7
			tile->x = count % width - 1;
			tile->y = count / width;
			layer->emplace_back(tile); // 8
		}
		count++;
	}
	const std::string layerName = layerNode->first_attribute("name")->value();
	return std::make_pair(layerName, layer);
}