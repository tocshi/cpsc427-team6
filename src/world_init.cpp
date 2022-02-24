#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::EGG,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createEgg(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.deadlys.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::EGG,
			GEOMETRY_BUFFER_ID::EGG });

	return entity;
}

// =================================================
// ================================
// Player
Entity createPlayer(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;
	motion.in_motion = false;
	motion.movement_speed = 200;
	motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });

	// Create player stats
	auto& stats = registry.stats.emplace(entity);

	// Create and (empty) Player component to be able to refer to all players
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// Player created with given motion component
Entity createPlayer(RenderSystem* renderer, Motion m)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = m.angle;
	motion.velocity = m.velocity;
	motion.position = m.position;
	motion.in_motion = m.in_motion;
	motion.movement_speed = m.movement_speed;
	motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });
	motion.destination = m.destination;

	// Create player stats
	auto& stats = registry.stats.emplace(entity);

	// Create and (empty) Player component to be able to refer to all players
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// Enemy slime (split into different enemies for future)
Entity createEnemy(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;
	motion.destination = pos;
	motion.in_motion = false;
	motion.movement_speed = 200;

	motion.scale = vec2({ ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT });

	// Create slime stats
	auto& stats = registry.stats.emplace(entity);
	stats.name = "Slime";

	// Create and (empty) Enemy component to be able to refer to all enemies
	// make it a slime enemy for now
	registry.slimeEnemies.insert(
		entity,
		{ 300,
		{ window_width_px / 2, 350.f },
		SLIME_STATE::IDLE });
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SLIME,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

// Boss
Entity createBoss(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ BOSS_BB_WIDTH, BOSS_BB_HEIGHT });

	// Create and (empty) BOSS component to be able to refer to all bosses
	registry.test.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOSS,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

// Artifact
Entity createArtifact(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ ARTIFACT_BB_WIDTH, ARTIFACT_BB_HEIGHT });

	// Create and (empty) ARTIFACT component to be able to refer to all artifacts
	//registry.test.emplace(entity);
	registry.artifacts.emplace(entity); // grab the artifact entity
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ARTIFACT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

// Item (consumable)
Entity createConsumable(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ CONSUMABLE_BB_WIDTH, CONSUMABLE_BB_HEIGHT });

	// Create and (empty) CONSUMABLE component to be able to refer to all consumables
	//registry.test.emplace(entity);
	registry.consumables.emplace(entity); // Replace to refer to Consuamble stats
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CONSUMABLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

// Item (equipable)
Entity createEquipable(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ EQUIPABLE_BB_WIDTH, EQUIPABLE_BB_HEIGHT });

	// Create and (empty) EQUIPABLE component to be able to refer to all equipables
	//registry.test.emplace(entity);
	registry.equipables.emplace(entity); // TRY FOR EQUIPTMENT
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EQUIPABLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

// Chest
Entity createChest(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ CHEST_BB_WIDTH, CHEST_BB_HEIGHT });

	// Create and (empty) CHEST component to be able to refer to all chests
	registry.test.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CHEST,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

// Door
Entity createDoor(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ DOOR_BB_WIDTH, DOOR_BB_HEIGHT });

	// Create and (empty) DOOR component to be able to refer to all doors
	registry.test.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DOOR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// Sign
Entity createSign(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ SIGN_BB_WIDTH, SIGN_BB_HEIGHT });

	// Create and (empty) SIGN component to be able to refer to all signs
	registry.test.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SIGN,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// Stair
Entity createStair(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ STAIR_BB_WIDTH, STAIR_BB_HEIGHT });

	// Create and (empty) STAIR component to be able to refer to all stairs
	registry.test.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::STAIR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// Wall
Entity createWall(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ WALL_BB_WIDTH, WALL_BB_HEIGHT });

	// Create and (empty) DOOR component to be able to refer to all doors
	registry.test.emplace(entity);
	registry.solid.emplace(entity);
	registry.collidables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WALL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createBackground(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	// Setting initial values
	motion.scale = vec2({ window_width_px, window_height_px });

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BG,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::BG
		});

	return entity;
}

// Menu Start Button
Entity createMenuStart(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ START_BB_WIDTH, START_BB_HEIGHT });

	// Create and (empty) START component to be able to refer to all start buttons
	registry.menuItems.emplace(entity);
	registry.buttons.insert(
		entity,
		{ BUTTON_ACTION_ID::MENU_START
		});
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::START,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// Menu Quit Button
Entity createMenuQuit(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ QUIT_BB_WIDTH, QUIT_BB_HEIGHT });

	// Create and (empty) QUIT component to be able to refer to all quit buttons
	registry.buttons.insert(
		entity,
		{ BUTTON_ACTION_ID::MENU_QUIT
		});
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::QUIT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Menu title
Entity createMenuTitle(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ TITLE_BB_WIDTH, TITLE_BB_HEIGHT });

	// Create and (empty) TITLE component to be able to refer to all title objects
	registry.menuItems.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TITLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


// HP Stat Bar 
Entity createHPBar(RenderSystem* renderer, vec2 position) {

	auto statEntity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(statEntity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(statEntity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values
	motion.scale = vec2({ STAT_BB_WIDTH, STAT_BB_HEIGHT });

	registry.renderRequests.insert(
		statEntity,
		{ TEXTURE_ASSET_ID::HPBAR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP
		});

	return statEntity;


}

// MP Stat Bar 
Entity createMPBar(RenderSystem* renderer, vec2 position) {

	auto statEntity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(statEntity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(statEntity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values
	motion.scale = vec2({ STAT_BB_WIDTH, STAT_BB_HEIGHT });

	registry.renderRequests.insert(
		statEntity,
		{ TEXTURE_ASSET_ID::MPBAR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP
		});

	return statEntity;


}

// EP Stat bar
Entity createEPBar(RenderSystem* renderer, vec2 position) {

	auto statEntity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(statEntity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(statEntity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values
	motion.scale = vec2({ STAT_BB_WIDTH, STAT_BB_HEIGHT });

	registry.renderRequests.insert(
		statEntity,
		{ TEXTURE_ASSET_ID::EPBAR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP
		});

	return statEntity;


}

// HP Bar filled portion
Entity createHPFill(RenderSystem* renderer, vec2 position) {

	auto statEntity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(statEntity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(statEntity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values
	motion.scale = vec2({ STAT_BB_WIDTH, STAT_BB_HEIGHT });

	registry.renderRequests.insert(
		statEntity,
		{ TEXTURE_ASSET_ID::HPFILL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI
		});

	return statEntity;


}

// MP Bar filled portion 
Entity createMPFill(RenderSystem* renderer, vec2 position) {

	auto statEntity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(statEntity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(statEntity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values
	motion.scale = vec2({ STAT_BB_WIDTH, STAT_BB_HEIGHT });

	registry.renderRequests.insert(
		statEntity,
		{ TEXTURE_ASSET_ID::MPFILL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI
		});

	return statEntity;


}

// EP Bar filled portion 
Entity createEPFill(RenderSystem* renderer, vec2 position) {

	auto statEntity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(statEntity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(statEntity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;

	// Setting initial values
	motion.scale = vec2({ STAT_BB_WIDTH, STAT_BB_HEIGHT });

	registry.renderRequests.insert(
		statEntity,
		{ TEXTURE_ASSET_ID::EPFILL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI
		});

	return statEntity;


}

Entity createFog(vec2 pos, float resolution, float radius, vec2 screen_resolution) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { resolution, resolution };

	Fog& fog = registry.fog.emplace(entity);
	fog.resolution = resolution;
	fog.radius = radius;
	fog.screen_resolution = screen_resolution;
	
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::FOG,
			GEOMETRY_BUFFER_ID::FOG,
			RENDER_LAYER_ID::EFFECT });

	return entity;
}

// Create an entity with a camera and motion component
Entity createCamera(vec2 pos)
{
	auto entity = Entity();

	// Create a Camera component to be able to refer to all cameras
	auto& camera = registry.cameras.emplace(entity);
	camera.position = pos;
	camera.active = true;

	return entity;
}

// Text entity
Entity createText(RenderSystem* renderer, vec2 pos, std::string msg, float scale = 1.0f, vec3 textColor = vec3(0.0f))
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the text component
	auto& text = registry.texts.emplace(entity);
	text.message = msg;
	text.position = pos;
	text.scale = scale;
	text.textColor = textColor;

	// Create an (empty) Bug component to be able to refer to all bug
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXT,
			GEOMETRY_BUFFER_ID::TEXTQUAD,
			RENDER_LAYER_ID::UI_TOP });

	return entity;
}

std::vector<Entity> createTiles(RenderSystem* renderer, const std::string& filepath) {
	TileMapParser parser = TileMapParser();
	return parser.Parse(tilemaps_path(filepath), renderer);
}