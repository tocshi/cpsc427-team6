#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::LINE,
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
	motion.movement_speed = 400;
	motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });

	// Create player stats
	auto& stats = registry.stats.emplace(entity);
	stats.hp = 100;
	stats.maxhp = 100;
	stats.mp = 100;
	stats.maxmp = 100;
	stats.ep = 100;
	stats.maxep = 100;
	stats.atk = 10;
	stats.def = 2;
	stats.speed = 10;
	stats.range = 400;

	// For Artifact Testing
	/*
	stats.maxhp = 1000;
	stats.hp = stats.maxhp;
	stats.mp = 100;
	stats.maxmp = 100;
	stats.ep = 100;
	stats.maxep = 100;
	stats.atk = 100;
	stats.def = 0;
	stats.speed = 10;
	stats.range = 400;*/
	
	registry.basestats.insert(entity, stats);

	// Create and (empty) Player component to be able to refer to all players
	auto& player = registry.players.emplace(entity);
	player.inv = registry.inventories.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	// add player to queuables
	registry.queueables.emplace(entity);
	registry.solid.emplace(entity);

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

	// add player to queuables
	registry.queueables.emplace(entity);
	registry.solid.emplace(entity);

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

	auto& enemy = registry.enemies.emplace(entity);
	enemy.initialPosition = pos;
	enemy.state = ENEMY_STATE::IDLE;
	enemy.type = ENEMY_TYPE::SLIME;
	enemy.inv = registry.inventories.emplace(entity);

	// Create slime stats
	auto& stats = registry.stats.emplace(entity);
	stats.name = "Slime";
	stats.prefix = "the";
	stats.maxhp = 28;
	stats.hp = stats.maxhp;
	stats.atk = 10;
	stats.def = 3;
	stats.speed = 8;
	stats.range = 250;

	// For Artifact Testing
	/*
	stats.maxhp = 1000;
	stats.hp = stats.maxhp;
	stats.atk = 100;
	stats.def = 0;
	stats.speed = 8;
	stats.range = 250;*/

	registry.basestats.insert(entity, stats);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SLIME,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	// add enemy to queuables
	registry.queueables.emplace(entity);
	registry.solid.emplace(entity);

	return entity;
}

// Enemy slime with motion component as input
Entity createEnemy(RenderSystem* renderer, Motion m)
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
	motion.destination = m.destination;
	motion.scale = vec2({ ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT });

	auto& enemy = registry.enemies.emplace(entity);
	enemy.state = ENEMY_STATE::IDLE;
	enemy.type = ENEMY_TYPE::SLIME;

	// Create slime stats
	auto& stats = registry.stats.emplace(entity);
	stats.name = "Slime";
	stats.prefix = "the";
	stats.maxhp = 25;
	stats.hp = stats.maxhp;
	stats.atk = 10;
	stats.def = 3;
	stats.speed = 8;
	stats.range = 250;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SLIME,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	// add enemy to queuables
	registry.queueables.emplace(entity);
	registry.solid.emplace(entity);

	return entity;
}

Entity createPlantShooter(RenderSystem* renderer, vec2 pos)
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
	motion.movement_speed = 0.f;

	motion.scale = vec2({ ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT });

	// Initilalize stats
	auto& stats = registry.stats.emplace(entity);
	stats.name = "Plant Shooter";
	stats.prefix = "the";
	stats.maxhp = 24.f;
	stats.hp = stats.maxhp;
	stats.atk = 8.f;
	stats.def = 2.f;
	stats.speed = 7.f;
	stats.range = 400.f;
	stats.chase = 0.f;

	registry.basestats.insert(entity, stats);

	auto& enemy = registry.enemies.emplace(entity);
	enemy.inv = registry.inventories.emplace(entity);
	enemy.initialPosition = pos;
	enemy.state = ENEMY_STATE::IDLE;
	enemy.type = ENEMY_TYPE::PLANT_SHOOTER;

	registry.queueables.emplace(entity);
	registry.solid.emplace(entity);
	// registry.damageables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLANT_SHOOTER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

Entity createPlantProjectile(RenderSystem* renderer, vec2 pos, vec2 dir, Entity owner)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = dir;
	motion.position = pos;
	motion.destination = pos + (dir * 500.f);
	motion.in_motion = true;
	motion.movement_speed = 300.f;

	motion.scale = vec2({ PLANT_PROJECTILE_BB_WIDTH, PLANT_PROJECTILE_BB_HEIGHT });

	// Initilalize stats
	// hp = 20, atk = 8, queue = 7, def = 2, range = 400
	auto& stat = registry.stats.emplace(entity);
	stat = registry.stats.get(owner);

	auto& projectileTimer = registry.projectileTimers.emplace(entity);
	projectileTimer.owner = owner;
	// Create and (empty) Enemy component to be able to refer to all enemies
	// registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLANT_PROJECTILE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

// Enemy slime (split into different enemies for future)
Entity createCaveling(RenderSystem* renderer, vec2 pos)
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

	auto& enemy = registry.enemies.emplace(entity);
	enemy.initialPosition = pos;
	enemy.state = ENEMY_STATE::IDLE;
	enemy.type = ENEMY_TYPE::CAVELING;
	enemy.inv = registry.inventories.emplace(entity);

	// Create caveling stats
	auto& stats = registry.stats.emplace(entity);
	stats.name = "Caveling";
	stats.prefix = "the";
	stats.maxhp = 19;
	stats.hp = stats.maxhp;
	stats.atk = 6;
	stats.def = 0;
	stats.speed = 15;
	stats.range = 300;

	registry.basestats.insert(entity, stats);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CAVELING,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	// add enemy to queuables
	registry.queueables.emplace(entity);
	registry.solid.emplace(entity);

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

	// add boss to queuables
	registry.queueables.emplace(entity);

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
	registry.equipment.emplace(entity); // TRY FOR EQUIPTMENT
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

	// Set interaction type
	auto& interactable = registry.interactables.emplace(entity);
	interactable.type = INTERACT_TYPE::CHEST;

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
Entity createSign(RenderSystem* renderer, vec2 pos, std::vector<std::pair<std::string, int>>& messages)
{
	auto entity = Entity();
	AnimationData& anim = registry.animations.emplace(entity);
	anim.spritesheet_texture = TEXTURE_ASSET_ID::SIGN_GLOW_SPRITESHEET;
	anim.frametime_ms = 200;
	anim.frame_indices = { 0, 1, 2, 3, 4, 5, 6, 7 };
	anim.spritesheet_columns = 8;
	anim.spritesheet_rows = 1;
	anim.spritesheet_width = 256;
	anim.spritesheet_height = 32;
	anim.frame_size = { anim.spritesheet_width / anim.spritesheet_columns, anim.spritesheet_height / anim.spritesheet_rows };

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
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SIGN_GLOW_SPRITESHEET,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::ANIMATION,
		RENDER_LAYER_ID::SPRITE
		});

	Sign& sign = registry.signs.emplace(entity);
	sign.messages = messages;

	registry.interactables.emplace(entity);

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

// create entity for cutScene
Entity createCutScene(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID tID) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ window_width_px, window_height_px });

	registry.renderRequests.insert(
		entity,
		{
		 tID, // textureAssetID
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::CUTSCENE

		}
	);
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

	registry.menuItems.emplace(entity);
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

// Actions bar
Entity createActionsBar(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ ACTIONS_BAR_BB_WIDTH, ACTIONS_BAR_BB_HEIGHT });

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACTIONS_BAR,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Attack button
Entity createAttackButton(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ ACTIONS_BUTTON_BB_WIDTH, ACTIONS_BUTTON_BB_HEIGHT });

	registry.actionButtons.emplace(entity);
	// Create and (empty) ACTIONS_ATTACK component to be able to refer to all attack buttons
	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::ACTIONS_ATTACK;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACTIONS_ATTACK,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Move button
Entity createMoveButton(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ ACTIONS_BUTTON_BB_WIDTH, ACTIONS_BUTTON_BB_HEIGHT });

	registry.actionButtons.emplace(entity);
	// Create and (empty) ACTIONS_MOVE component to be able to refer to all move buttons
	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::ACTIONS_MOVE;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACTIONS_MOVE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Guard button
Entity createGuardButton(RenderSystem* renderer, vec2 pos, BUTTON_ACTION_ID action, TEXTURE_ASSET_ID texture) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ ACTIONS_BUTTON_BB_WIDTH, ACTIONS_BUTTON_BB_HEIGHT });

	registry.actionButtons.emplace(entity);
	GuardButton& gb = registry.guardButtons.emplace(entity);
	gb.texture = texture;
	gb.action = action;
	registry.renderRequests.insert(
		entity,
		{ texture,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Item button
Entity createItemButton(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ ACTIONS_BUTTON_BB_WIDTH, ACTIONS_BUTTON_BB_HEIGHT });

	registry.actionButtons.emplace(entity);
	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::ACTIONS_ITEM;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACTIONS_ITEM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Back button
Entity createBackButton(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ MODE_BB_HEIGHT, MODE_BB_HEIGHT });

	registry.modeVisualizationObjects.emplace(entity);
	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::ACTIONS_BACK;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACTIONS_BACK,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Cancel button
Entity createCancelButton(RenderSystem* renderer, vec2 pos) {
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

	registry.menuItems.emplace(entity);

	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::ACTIONS_CANCEL;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACTIONS_CANCEL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Attack mode text
Entity createAttackModeText(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ MODE_BB_WIDTH, MODE_BB_HEIGHT });

	registry.modeVisualizationObjects.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACTIONS_ATTACK_MODE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Move mode text
Entity createMoveModeText(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ MODE_BB_WIDTH, MODE_BB_HEIGHT });

	registry.modeVisualizationObjects.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACTIONS_MOVE_MODE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Pause button
Entity createPauseButton(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ PAUSE_BUTTON_BB_WIDTH, PAUSE_BUTTON_BB_HEIGHT });

	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::PAUSE;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PAUSE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	return entity;
}

// Collection (book) button
Entity createCollectionButton(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ PAUSE_BUTTON_BB_WIDTH, PAUSE_BUTTON_BB_HEIGHT });

	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::COLLECTION;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::COLLECTION_BUTTON,
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

Entity createEpRange(vec2 pos, float resolution, float radius, vec2 screen_resolution) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { resolution, resolution };

	EpRange& ep = registry.epRange.emplace(entity);
	ep.resolution = resolution;
	ep.radius = radius;
	ep.screen_resolution = screen_resolution;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::EP,
			GEOMETRY_BUFFER_ID::EP,
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

	// Create an (empty) TEXT component to be able to refer to all text
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXT,
			GEOMETRY_BUFFER_ID::TEXTQUAD,
			RENDER_LAYER_ID::UI_TOP });

	return entity;
}

SpawnData createTiles(RenderSystem* renderer, const std::string& filepath) {
	TileMapParser parser = TileMapParser();
	return parser.Parse(tilemaps_path(filepath), renderer);
}

Entity createCampfire(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();
	AnimationData& anim = registry.animations.emplace(entity);
	anim.spritesheet_texture = TEXTURE_ASSET_ID::CAMPFIRE_SPRITESHEET;
	anim.frametime_ms = 150;
	anim.frame_indices = { 0, 1, 2, 3, 4 };
	anim.spritesheet_columns = 5;
	anim.spritesheet_rows = 1;
	anim.spritesheet_width = 320;
	anim.spritesheet_height = 64;
	anim.frame_size = { anim.spritesheet_width / anim.spritesheet_columns, anim.spritesheet_height / anim.spritesheet_rows };

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = { 64, 64 };
	
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CAMPFIRE_SPRITESHEET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::ANIMATION,
			RENDER_LAYER_ID::FLOOR_DECO });
	registry.hidables.emplace(entity);

	return entity;
}

Entity createExplosion(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();
	AnimationData& anim = registry.animations.emplace(entity);
	anim.spritesheet_texture = TEXTURE_ASSET_ID::EXPLOSION_SPRITESHEET;
	anim.frametime_ms = 80;
	anim.loop = false;
	anim.delete_on_finish = true;
	anim.frame_indices = { 0, 1, 2, 3, 4, 5, 6, 7 };
	anim.spritesheet_columns = 2;
	anim.spritesheet_rows = 4;
	anim.spritesheet_width = 367;
	anim.spritesheet_height = 185;
	anim.frame_size = { anim.spritesheet_width / anim.spritesheet_columns, anim.spritesheet_height / anim.spritesheet_rows };

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = { 128 , 128 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EXPLOSION_SPRITESHEET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::ANIMATION,
			RENDER_LAYER_ID::WALLS });

	return entity;
}