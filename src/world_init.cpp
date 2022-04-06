#include "world_init.hpp"
#include "combat_system.hpp"
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
	motion.scale = scale * vec2(ui_scale, ui_scale);

	registry.debugComponents.emplace(entity);
	return entity;
}

// =================================================
// ================================
// Player
Entity createPlayer(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

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
	stats.atk = 0;
	stats.def = 0;
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
	registry.inventories.emplace(entity);

	Equipment weapon = {};
	weapon.type = EQUIPMENT::BLUNT;
	weapon.sprite = 0;
	weapon.atk = 10;

	// DEBUG
	/*
	weapon.attacks[0] = ATTACK::PIERCING_THRUST;
	weapon.attacks[1] = ATTACK::TERMINUS_VERITAS;
	weapon.attacks[2] = ATTACK::SAPPING_STRIKE;
	weapon.attacks[3] = ATTACK::ROUNDSLASH;
	*/

	Equipment armour = {};
	armour.type = EQUIPMENT::ARMOUR;
	armour.sprite = 2;
	armour.def = 2;

	equip_item(entity, weapon);
	equip_item(entity, armour);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::PLAYER });

	// add player to queuables
	registry.queueables.emplace(entity);
	registry.solid.emplace(entity);

	// add status container to player
	registry.statuses.emplace(entity);

	return entity;
}

// Enemy slime (split into different enemies for future)
Entity createEnemy(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

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
	registry.inventories.emplace(entity);

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

	// add status container to slime
	registry.statuses.emplace(entity);

	// add hp bar 
	EnemyHPBar& hpbar = registry.enemyHPBars.emplace(entity);
	hpbar.hpBacking = createEnemyHPBacking(pos + vec2(0, ENEMY_HP_BAR_OFFSET), entity);
	hpbar.hpFill = createEnemyHPFill(pos + vec2(0, ENEMY_HP_BAR_OFFSET), entity);

	return entity;
}

Entity createPlantShooter(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

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
	registry.inventories.emplace(entity);
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

	// add status container to plantshooter
	registry.statuses.emplace(entity);

	// add hp bar 
	EnemyHPBar& hpbar = registry.enemyHPBars.emplace(entity);
	hpbar.hpBacking = createEnemyHPBacking(pos + vec2(0, ENEMY_HP_BAR_OFFSET), entity);
	hpbar.hpFill = createEnemyHPFill(pos + vec2(0, ENEMY_HP_BAR_OFFSET), entity);

	return entity;
}

Entity createProjectile(RenderSystem* renderer, Entity owner, vec2 pos, vec2 scale, float dir, float multiplier, TEXTURE_ASSET_ID texture)
{
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.movement_speed = 400.f;
	motion.position = pos;
	motion.scale = scale;
	motion.velocity = dirdist_extrapolate(pos, dir, motion.movement_speed) - pos;
	motion.destination = dirdist_extrapolate(pos, dir, window_width_px);
	motion.angle = dir;
	motion.in_motion = true;

	// Initilalize stats
	auto& stat = registry.stats.emplace(entity);
	stat = registry.stats.get(owner);

	auto& projectileTimer = registry.projectileTimers.emplace(entity);
	projectileTimer.owner = owner;
	projectileTimer.multiplier = multiplier;
	// Create and (empty) Enemy component to be able to refer to all enemies
	// registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ texture,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

// Enemy slime (split into different enemies for future)
Entity createCaveling(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

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
	registry.inventories.emplace(entity);

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

	// add status container to caveling
	registry.statuses.emplace(entity);

	// add hp bar 
	EnemyHPBar& hpbar = registry.enemyHPBars.emplace(entity);
	hpbar.hpBacking = createEnemyHPBacking(pos + vec2(0, ENEMY_HP_BAR_OFFSET), entity);
	hpbar.hpFill = createEnemyHPFill(pos + vec2(0, ENEMY_HP_BAR_OFFSET), entity);

	return entity;
}

// Enemy slime (split into different enemies for future)
Entity createKingSlime(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;
	motion.destination = pos;
	motion.in_motion = false;
	motion.movement_speed = 200;

	motion.scale = vec2({ ENEMY_BB_WIDTH*4, ENEMY_BB_HEIGHT*4 });

	auto& enemy = registry.enemies.emplace(entity);
	enemy.initialPosition = pos;
	enemy.state = ENEMY_STATE::IDLE;
	enemy.type = ENEMY_TYPE::KING_SLIME;
	registry.inventories.emplace(entity);
	registry.bosses.emplace(entity);

	// Create king slime stats
	auto& stats = registry.stats.emplace(entity);
	stats.name = "King Slime";
	stats.prefix = "the";
	stats.maxhp = 500;
	stats.hp = stats.maxhp;
	stats.atk = 15;
	stats.def = 5;
	stats.speed = 5;
	stats.range = 300;

	registry.basestats.insert(entity, stats);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::KINGSLIME,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	// add enemy to queuables
	registry.queueables.emplace(entity);
	registry.solid.emplace(entity);

	// add status container to slime
	registry.statuses.emplace(entity);
	return entity;
}

// Generate Random Equipment
Equipment createEquipment(EQUIPMENT type, int tier) {

	Equipment equipment = {};
	equipment.type = type;

	// Create equipment random stats
	float atkmod = 0.f;
	float defmod = 0.f;
	float speedmod = 0.f;
	float hpmod = 0.f;
	float mpmod = 0.f;
	float netstat = -999.f;

	while (netstat < (-2 + 2 * tier) || netstat >(1 + 2 * tier)) {
		atkmod = irandRange(-4 + tier, 2 + tier);
		defmod = irandRange(-4 + tier, 2 + tier);
		speedmod = irandRange(-4 + tier, 2 + tier);
		hpmod = irandRange(-20 + tier * 5, 10 + tier * 5);
		mpmod = irandRange(-20 + tier * 5, 10 + tier * 5);
		netstat = atkmod + defmod + speedmod + (hpmod / 5) + (mpmod / 5);
	}

	// Create base stats and attacks
	if (type == EQUIPMENT::SHARP || type == EQUIPMENT::BLUNT || type == EQUIPMENT::RANGED) {
		equipment.atk = 7 + 4 * tier;

		// Create shuffled attack pool
		std::vector<ATTACK> sharp_attacks = { ATTACK::SAPPING_STRIKE, ATTACK::PIERCING_THRUST, ATTACK::PARRYING_STANCE, ATTACK::DISENGAGE };
		std::random_shuffle(std::begin(sharp_attacks), std::end(sharp_attacks));
		std::vector<ATTACK> blunt_attacks = { ATTACK::ARMOURCRUSHER, ATTACK::DISORIENTING_BASH, ATTACK::TECTONIC_SLAM, ATTACK::FERVENT_CHARGE };
		std::random_shuffle(std::begin(blunt_attacks), std::end(blunt_attacks));
		std::vector<ATTACK> ranged_attacks = { ATTACK::BINDING_ARROW, ATTACK::LUMINOUS_ARROW, ATTACK::HOOK_SHOT, ATTACK::FOCUSED_SHOT };
		std::random_shuffle(std::begin(ranged_attacks), std::end(ranged_attacks));
		std::vector<ATTACK> combined_attacks;
		combined_attacks.reserve(sharp_attacks.size() + blunt_attacks.size() + ranged_attacks.size());

		switch (type) {
		case EQUIPMENT::SHARP:
			equipment.attacks[0] = ATTACK::ROUNDSLASH;
			equipment.attacks[1] = sharp_attacks.back();
			sharp_attacks.pop_back();
			equipment.attacks[2] = sharp_attacks.back();
			sharp_attacks.pop_back();
			equipment.attacks[3] = ATTACK::TERMINUS_VERITAS;
			equipment.sprite = irandRange(1, 7) * 6;
			break;
		case EQUIPMENT::BLUNT:
			equipment.attacks[0] = ATTACK::WILD_SWINGS;
			equipment.attacks[1] = blunt_attacks.back();
			blunt_attacks.pop_back();
			equipment.attacks[2] = blunt_attacks.back();
			blunt_attacks.pop_back();
			equipment.attacks[3] = ATTACK::PRIMAL_RAGE;
			equipment.sprite = irandRange(1, 7) * 6 + 1;
			break;
		case EQUIPMENT::RANGED:
			equipment.attacks[0] = ATTACK::SPREAD_SHOT;
			equipment.attacks[1] = ranged_attacks.back();
			ranged_attacks.pop_back();
			equipment.attacks[2] = ranged_attacks.back();
			ranged_attacks.pop_back();
			equipment.attacks[3] = ATTACK::SKYBORNE_RAIN;
			equipment.sprite = irandRange(1, 7) * 6 + 3;
			break;
		}

		// Check for Chimera's Arm and resolve effect
		if (registry.inventories.get(registry.players.entities[0]).artifact[(int)ARTIFACT::CHIMERARM] > 0) {
			equipment.atk += 4 * registry.inventories.get(registry.players.entities[0]).artifact[(int)ARTIFACT::CHIMERARM];
			combined_attacks.insert(combined_attacks.end(), sharp_attacks.begin(), sharp_attacks.end());
			combined_attacks.insert(combined_attacks.end(), blunt_attacks.begin(), blunt_attacks.end());
			combined_attacks.insert(combined_attacks.end(), ranged_attacks.begin(), ranged_attacks.end());
			equipment.attacks[1] = combined_attacks[irand(combined_attacks.size())];
		}
	}
	else if (type == EQUIPMENT::ARMOUR) {
		equipment.def = 0 + 2 * tier;
		equipment.sprite = irandRange(1, 7) * 6 + 2;
	}

	equipment.atk += atkmod;
	equipment.def += defmod;
	equipment.speed += speedmod;
	equipment.hp += hpmod;
	equipment.mp += mpmod;

	return equipment;
}

// Generate interactable Equipment entity
Entity createEquipmentEntity(RenderSystem* renderer, vec2 pos, Equipment equipment)
{
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;
	motion.destination = pos;
	motion.in_motion = false;
	motion.movement_speed = 0;

	motion.scale = vec2({ PICKUP_BB_WIDTH, PICKUP_BB_HEIGHT });

	registry.equipment.insert(entity, equipment);
	auto& interactable = registry.interactables.emplace(entity);
	interactable.type = INTERACT_TYPE::PICKUP;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EQUIPMENT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITESHEET,
		});
	registry.hidables.emplace(entity);

	Spritesheet& spritesheet = registry.spritesheets.emplace(entity);
	spritesheet.texture = TEXTURE_ASSET_ID::EQUIPMENT;
	spritesheet.width = 96;
	spritesheet.height = 144;
	spritesheet.columns = 6;
	spritesheet.rows = 9;
	spritesheet.frame_size = { 16, 16 };
	spritesheet.index = equipment.sprite;

	return entity;
}

// Artifact
Entity createArtifact(RenderSystem* renderer, vec2 pos, ARTIFACT type)
{
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;
	motion.destination = pos;
	motion.in_motion = false;
	motion.movement_speed = 0;

	motion.scale = vec2({ PICKUP_BB_WIDTH, PICKUP_BB_HEIGHT });

	// Create and (empty) ARTIFACT component to be able to refer to all artifacts
	//registry.test.emplace(entity);
	auto& artifact = registry.artifacts.emplace(entity); // grab the artifact entity
	artifact.type = type;
	auto& interactable = registry.interactables.emplace(entity);
	interactable.type = INTERACT_TYPE::PICKUP;
	TEXTURE_ASSET_ID sprite = artifact_textures.at(type);

	registry.renderRequests.insert(
		entity,
		{ sprite,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.hidables.emplace(entity);

	return entity;
}

// Item (consumable)
Entity createConsumable(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ PICKUP_BB_WIDTH, PICKUP_BB_HEIGHT });

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

// Chest
Entity createChest(RenderSystem* renderer, vec2 pos, bool isArtifact)
{
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ CHEST_BB_WIDTH, CHEST_BB_HEIGHT });

	// Set interaction type
	auto& interactable = registry.interactables.emplace(entity);
	RenderRequest& rr = registry.renderRequests.emplace(entity);
	rr.used_effect = EFFECT_ASSET_ID::TEXTURED;
	rr.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	rr.used_layer = RENDER_LAYER_ID::FLOOR_DECO;

	if (isArtifact) {
		interactable.type = INTERACT_TYPE::ARTIFACT_CHEST;
		rr.used_texture = TEXTURE_ASSET_ID::CHEST_ARTIFACT_CLOSED;
	}
	else{
		interactable.type = INTERACT_TYPE::ITEM_CHEST;
		rr.used_texture = TEXTURE_ASSET_ID::CHEST_ITEM_CLOSED;
	}
	registry.chests.insert(entity, { isArtifact });

	registry.hidables.emplace(entity);

	return entity;
}

// Door
Entity createDoor(RenderSystem* renderer, vec2 pos, bool boss_door)
{
	auto entity = Entity();

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

	registry.solid.emplace(entity);
	auto& interactable = registry.interactables.emplace(entity);
	if (boss_door) {
		interactable.type = INTERACT_TYPE::BOSS_DOOR;
		registry.colors.insert(entity, vec3(1, 0.4, 0.4));
	}
	else {
		interactable.type = INTERACT_TYPE::DOOR;
	}

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

	auto& interactable = registry.interactables.emplace(entity);
	interactable.type = INTERACT_TYPE::SIGN;

	return entity;
}

// TODO tutorial: Sign using textboxes
Entity createSign2(RenderSystem* renderer, vec2 pos, std::vector<std::vector<std::string>>& messages)
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

	Sign2& sign = registry.signs2.emplace(entity);
	sign.messages = messages;

	auto& interactable = registry.interactables.emplace(entity);
	interactable.type = INTERACT_TYPE::SIGN_2;
	interactable.interacted = false;

	return entity;
}

// TODO tutorial: Textbox
Entity createTextbox(RenderSystem* renderer, vec2 pos, std::vector<std::vector<std::string>>& messages)
{
	auto entity = Entity();

	Textbox& textbox = registry.textboxes.emplace(entity);
	textbox.num_lines = 0;
	textbox.num_messages = messages.size();
	textbox.messages = messages;
	if (textbox.num_messages > 0) {
		for (std::string line : messages[0]) {
			textbox.num_lines++;
			Entity text = createText(renderer, pos*2.f + vec2(-TEXTBOX_BB_WIDTH + 100.f, -TEXTBOX_BB_HEIGHT + 75.f * textbox.num_lines), line, 2.0f, vec3(1.f));
			textbox.lines.push_back(text);
		}
	}
	textbox.next_message = 1;

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ TEXTBOX_BB_WIDTH, TEXTBOX_BB_HEIGHT });

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTBOX,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER_ID::UI
		});
	
	return entity;
}

// Stair
Entity createStair(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

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

	auto& interactable = registry.interactables.emplace(entity);
	interactable.type = INTERACT_TYPE::STAIRS;

	return entity;
}

// Wall
Entity createWall(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

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

Entity createGameBackground(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture_id, RENDER_LAYER_ID render_id)
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
	motion.scale = vec2({ window_width_px * 2, window_height_px });

	registry.renderRequests.insert(
		entity,
		{ texture_id,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 render_id
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

	registry.colors.insert(entity, {0.5f, 0.5f, 0.5f});

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

// Menu Continue Button
Entity createMenuContinue(RenderSystem * renderer, vec2 pos)
{
	auto entity = Entity();

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
		{ BUTTON_ACTION_ID::CONTINUE
		});
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CONTINUE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP });

	return entity;
}

// Save and Quit Button
Entity createSaveQuit(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

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
		{ BUTTON_ACTION_ID::SAVE_QUIT
		});
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SAVE_QUIT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP });

	return entity;
}

// Menu Quit Button
Entity createMenuQuit(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

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
		 RENDER_LAYER_ID::UI_TOP });

	return entity;
}

// Keybind icons
Entity createKeyIcon(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID texture) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ KEY_ICON_BB_WIDTH, KEY_ICON_BB_HEIGHT });

	registry.keyIcons.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ texture,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP });

	return entity;
}

// Actions bar
Entity createActionsBar(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

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
		 RENDER_LAYER_ID::UI_TOP });

	return entity;
}

// Attack mode text
Entity createAttackModeText(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

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

// Item menu
Entity createItemMenu(RenderSystem* renderer, vec2 top_card_pos, Inventory inv) {
	auto entity = Entity();
	
	// render item cards, and items on them (if any)
	// create weapon card
	createItemCard(renderer, top_card_pos, EQUIPMENT::SHARP, inv.equipped[0]);
	// create armour card
	createItemCard(renderer, vec2(top_card_pos.x, top_card_pos.y + 150.f * ui_scale), EQUIPMENT::ARMOUR, inv.equipped[1]);

	return entity;
}

// Item menu cards
Entity createItemCard(RenderSystem* renderer, vec2 pos, EQUIPMENT type, Equipment item) {
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

	ItemCard& ic = registry.itemCards.emplace(entity);
	ic.item = item;

	// TODO: add dialog for more item info
	//Button& b = registry.buttons.emplace(entity);
	//b.action_taken = BUTTON_ACTION_ID::OPEN_ATTACK_DIALOG;

	// get attack type from item
	switch (type) {
		case EQUIPMENT::ARMOUR:
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::ITEM_ARMOUR_CARD,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE,
				 RENDER_LAYER_ID::UI });
			break;
		case EQUIPMENT::SHARP:
		case EQUIPMENT::RANGED:
		case EQUIPMENT::BLUNT:
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::ITEM_WEAPON_CARD,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE,
				 RENDER_LAYER_ID::UI });
			break;
	}
	
	Entity equip = createItemEquipmentTexture(renderer, pos, item);
	registry.itemCards.emplace(equip);

	return entity;
}

// Generate equipment texture for item menu
Entity createItemEquipmentTexture(RenderSystem* renderer, vec2 pos, Equipment equipment)
{
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;
	motion.destination = pos;
	motion.in_motion = false;
	motion.movement_speed = 0;

	motion.scale = vec2({ PICKUP_BB_WIDTH, PICKUP_BB_HEIGHT });

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EQUIPMENT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITESHEET,
		 RENDER_LAYER_ID::UI_TOP
		});

	Spritesheet& spritesheet = registry.spritesheets.emplace(entity);
	spritesheet.texture = TEXTURE_ASSET_ID::EQUIPMENT;
	spritesheet.width = 96;
	spritesheet.height = 144;
	spritesheet.columns = 6;
	spritesheet.rows = 9;
	spritesheet.frame_size = { 16, 16 };
	spritesheet.index = equipment.sprite;

	return entity;
}

// Attack type cards
Entity createAttackCard(RenderSystem* renderer, vec2 pos, ATTACK attack) {
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ ACTIONS_BUTTON_BB_WIDTH * 4 / 5, ACTIONS_BUTTON_BB_HEIGHT * 4 / 5 });

	AttackCard& ac = registry.attackCards.emplace(entity);
	ac.attack = attack;

	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::OPEN_ATTACK_DIALOG;

	// get attack texture from map
	auto iter = attack_textures.find(attack);
	if (iter != attack_textures.end()) {
		// render the texture if one exists for the attack
		TEXTURE_ASSET_ID texture = iter->second;
		registry.renderRequests.insert(
			entity,
			{ texture,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE,
			 RENDER_LAYER_ID::UI });
	}
	else {
		printf("ERROR: texture does not exist for attack");
	}

	return entity;
}

// Attack dialog
Entity createAttackDialog(RenderSystem* renderer, vec2 pos, ATTACK attack, bool prepared) {
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ DESCRIPTION_DIALOG_BB_WIDTH, DESCRIPTION_DIALOG_BB_HEIGHT });

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DESCRIPTION_DIALOG,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::DIALOG });

	AttackDialog& ad = registry.attackDialogs.emplace(entity);

	// set ad title
	Entity tt;
	bool hasTT = false;
	auto iter = attack_names.find(attack);
	if (iter != attack_names.end()) {
		ad.title = iter->second;
		tt = createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2), ad.title, 2.0f, vec3(0.0f));
		hasTT = true;
	}
	else {
		printf("ERROR: name does not exist for attack");
	}

	// set ad effect
	std::vector<Entity> ctVect;
	bool hasCT = false;
	ctVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 80.f), "COST: ", 1.6f, vec3(0.0f)));
	iter = attack_costs_string.find(attack);
	if (iter != attack_costs_string.end()) {
		ad.cost = iter->second;
		if (ad.cost.size() > 40) {
			bool renderNewLine = true;
			float effectOffset = 0.f;
			int iter = 1;
			std::string effectLine = ad.cost.substr(0, 40);
			while (renderNewLine) {
				ctVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 130.f + effectOffset), effectLine, 1.2f, vec3(0.0f)));
				effectLine = ad.cost.substr(40 * iter);
				effectOffset += 30.f;
				if (effectLine.size() >= 40) {
					effectLine = ad.cost.substr(40 * iter, 40);
					iter++;
				}
				else {
					ctVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 130.f + effectOffset), effectLine, 1.2f, vec3(0.0f)));
					renderNewLine = false;
				}
			}
		}
		else {
			ctVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 130.f), ad.cost, 1.2f, vec3(0.0f)));
		}
		hasCT = true;
	}
	else {
		printf("ERROR: cost does not exist for attack");
	}

	// set dd description
	std::vector<Entity> dtVect;
	bool hasDT = false;
	iter = attack_descriptions.find(attack);
	dtVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 200.f), "DESCRIPTION: ", 1.6f, vec3(0.0f)));
	if (iter != attack_descriptions.end()) {
		ad.description = iter->second;
		if (ad.description.size() > 40) {
			bool renderNewLine = true;
			float descOffset = 0.f;
			int iter = 1;
			std::string descLine = ad.description.substr(0, 40);
			while (renderNewLine) {
				dtVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 250.f + descOffset), descLine, 1.2f, vec3(0.0f)));
				descLine = ad.description.substr(40 * iter);
				descOffset += 30.f;
				if (descLine.size() >= 40) {
					descLine = ad.description.substr(40 * iter, 40);
					iter++;
				}
				else {
					dtVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 250.f + descOffset), descLine, 1.2f, vec3(0.0f)));
					renderNewLine = false;
				}
			}
		}
		else {
			dtVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 400.f), ad.description, 1.2f, vec3(0.0f)));
		}

		hasDT = true;
	}
	else {
		printf("ERROR: description does not exist for attack");
	}

	// need to add new entities to attackDialogs at the end to avoid memory issues
	if (hasTT) { registry.attackDialogs.emplace(tt); }
	if (hasCT) {
		for (Entity ct : ctVect) {
			registry.attackDialogs.emplace(ct);
		}
	}
	if (hasDT) {
		for (Entity dt : dtVect) {
			registry.attackDialogs.emplace(dt);
		}
	}

	// render use / prepare buttons
	if (!prepared) {
		createAttackDialogButton(renderer, vec2(pos.x - DESCRIPTION_DIALOG_BB_WIDTH / 4 + 3.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 - 50.f), TEXTURE_ASSET_ID::USE_BUTTON, BUTTON_ACTION_ID::USE_ATTACK);
		createAttackDialogButton(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH / 4 - 3.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 - 50.f), TEXTURE_ASSET_ID::PREPARE_BUTTON, BUTTON_ACTION_ID::PREPARE_ATTACK);
	}

	// render the x button
	auto close_entity = Entity();

	Mesh& close_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(close_entity, &close_mesh);

	auto& close_motion = registry.motions.emplace(close_entity);
	close_motion.angle = 0.f;
	close_motion.velocity = { 0.f, 0.f };
	close_motion.position = { pos.x + (DESCRIPTION_DIALOG_BB_WIDTH / 2) - 60,
		pos.y - (DESCRIPTION_DIALOG_BB_HEIGHT / 2) + 50 };

	close_motion.scale = vec2({ PAUSE_BUTTON_BB_WIDTH / 1.5, PAUSE_BUTTON_BB_HEIGHT / 1.5 });


	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::CLOSE_ATTACK_DIALOG;

	// need to add 'x' to attackDialogs so it is closed when the entire modal is closed
	registry.attackDialogs.emplace(close_entity);

	registry.renderRequests.insert(
		close_entity,
		{ TEXTURE_ASSET_ID::MENU_CLOSE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP });

	return entity;
}

// generic button
Entity createAttackDialogButton(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID button_texture, BUTTON_ACTION_ID button_action) {
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ ATTACK_DIALOG_BUTTON_BB_WIDTH, ATTACK_DIALOG_BUTTON_BB_HEIGHT });

	registry.attackDialogs.emplace(entity);

	Button& b = registry.buttons.emplace(entity);
	b.action_taken = button_action;
	registry.renderRequests.insert(
		entity,
		{ button_texture,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP });

	return entity;
}

// Collection menu
Entity createCollectionMenu(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ COLLECTION_MENU_BB_WIDTH, COLLECTION_MENU_BB_HEIGHT });

	registry.menuItems.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::COLLECTION_PANEL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI });

	// render the x button
	auto close_entity = Entity();

	Mesh& close_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(close_entity, &close_mesh);

	registry.menuItems.emplace(close_entity);

	Button& b = registry.buttons.emplace(close_entity);
	b.action_taken = BUTTON_ACTION_ID::ACTIONS_CANCEL;

	auto& close_motion = registry.motions.emplace(close_entity);
	close_motion.angle = 0.f;
	close_motion.velocity = { 0.f, 0.f };
	close_motion.position = { pos.x  + (COLLECTION_MENU_BB_WIDTH / 2) - 60, pos.y - (COLLECTION_MENU_BB_HEIGHT / 2) + 50};

	close_motion.scale = vec2({ PAUSE_BUTTON_BB_WIDTH / 1.5, PAUSE_BUTTON_BB_HEIGHT / 1.5});

	registry.renderRequests.insert(
		close_entity,
		{ TEXTURE_ASSET_ID::MENU_CLOSE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP });

	// render the artifact icons
	float x_offset = 0.f;
	float y_offset = 0.f;
	for (int artifact = 0; artifact < (int)ARTIFACT::ARTIFACT_COUNT; artifact++) {
		float next_x = pos.x - ((COLLECTION_MENU_BB_WIDTH / 2) - 124.f) + x_offset;
		float next_y = pos.y - ((COLLECTION_MENU_BB_HEIGHT / 2) - 100) + y_offset;
		if (next_x >= pos.x + (COLLECTION_MENU_BB_WIDTH / 2) - 200) {
			x_offset = 0.f;
			y_offset += 150.f;
		}
		else {
			x_offset += (ARTIFACT_IMAGE_BB_WIDTH + 20.f);
		}
		createArtifactIcon(renderer, vec2(next_x, next_y),
			static_cast<ARTIFACT>(artifact));

		// need to render the current count beside it
		Inventory inv = registry.inventories.components[0];

		int size = inv.artifact[(int)artifact];
		std::string sizeStr = std::to_string(size);

		std::vector<Entity> textVect;
		textVect.push_back(createText(renderer, vec2((next_x + 30.f) * 2, (next_y + 40.f) * 2), sizeStr.substr(0, 1), 1.4f, vec3(0.0f)));
		if (size > 9) {
			// need to print two numbers		
			textVect.push_back(createText(renderer, vec2((next_x + 45.f) * 2, (next_y + 40.f) * 2), sizeStr.substr(1, 1), 1.4f, vec3(0.0f)));
		}

		for (Entity text : textVect) {
			registry.menuItems.emplace(text);
		}
	}

	return entity;
}

// Artifact icon
Entity createArtifactIcon(RenderSystem* renderer, vec2 pos, ARTIFACT artifact) {
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ ARTIFACT_IMAGE_BB_WIDTH, ARTIFACT_IMAGE_BB_HEIGHT });

	registry.menuItems.emplace(entity);

	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::OPEN_DIALOG;

	ArtifactIcon& ai = registry.artifactIcons.emplace(entity);
	ai.artifact = artifact;

	// get artifact texture from map
	auto iter = artifact_textures.find(artifact);
	if (iter != artifact_textures.end()) {
		// render the texture if one exists for the artifact
		TEXTURE_ASSET_ID texture = iter->second;
		registry.renderRequests.insert(
			entity,
			{ texture,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE,
			 RENDER_LAYER_ID::ARTIFACT_ICONS });
	}
	else {
		printf("ERROR: texture does not exist for artifact");
	}

	return entity;
}

// Description dialog
Entity createDescriptionDialog(RenderSystem* renderer, vec2 pos, ARTIFACT artifact) {
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ DESCRIPTION_DIALOG_BB_WIDTH, DESCRIPTION_DIALOG_BB_HEIGHT });

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DESCRIPTION_DIALOG,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::DIALOG });

	DescriptionDialog& dd = registry.descriptionDialogs.emplace(entity);

	// render the artifact image on top
	auto icon_entity = Entity();

	Mesh& icon_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(icon_entity, &icon_mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& icon_motion = registry.motions.emplace(icon_entity);
	icon_motion.angle = 0.f;
	icon_motion.velocity = { 0.f, 0.f };
	icon_motion.position = vec2(pos.x - DESCRIPTION_DIALOG_BB_WIDTH / 2 + 35.f, pos.y - DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 40.f);

	icon_motion.scale = vec2({ ARTIFACT_IMAGE_BB_WIDTH / 1.5f, ARTIFACT_IMAGE_BB_HEIGHT / 1.5f });

	registry.menuItems.emplace(icon_entity);

	// get artifact texture from map
	auto icon_iter = artifact_textures.find(artifact);
	if (icon_iter != artifact_textures.end()) {
		// render the texture if one exists for the artifact
		TEXTURE_ASSET_ID icon_texture = icon_iter->second;
		registry.renderRequests.insert(
			icon_entity,
			{ icon_texture,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE,
			 RENDER_LAYER_ID::DIALOG_TEXT });
	}
	else {
		printf("ERROR: texture does not exist for artifact");
	}

	// set dd title
	Entity tt;
	bool hasTT = false;
	auto iter = artifact_names.find(artifact);
	if (iter != artifact_names.end()) {
		dd.title = iter->second;
		tt = createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2), dd.title, 2.0f, vec3(0.0f));
		hasTT = true;
		// registry.descriptionDialogs.emplace(tt);
	}
	else {
		printf("ERROR: name does not exist for artifact");
	}

	// set dd effect
	std::vector<Entity> etVect;
	bool hasET = false;
	etVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 80.f), "EFFECT: ", 1.6f, vec3(0.0f)));
	iter = artifact_effects.find(artifact);
	if (iter != artifact_effects.end()) {
		dd.effect = iter->second;
		if (dd.effect.size() > 40) {
			bool renderNewLine = true;
			float effectOffset = 0.f;
			int iter = 1;
			std::string effectLine = dd.effect.substr(0, 40);
			while (renderNewLine) {
				etVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 130.f + effectOffset), effectLine, 1.2f, vec3(0.0f)));
				effectLine = dd.effect.substr(40 * iter);
				effectOffset += 30.f;
				if (effectLine.size() >= 40) {
					effectLine = dd.effect.substr(40 * iter, 40);
					iter++;
				}
				else {
					etVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 130.f + effectOffset), effectLine, 1.2f, vec3(0.0f)));
					renderNewLine = false;
				}
			}
		}
		else {
			etVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 130.f), dd.effect, 1.2f, vec3(0.0f)));
		}
		hasET = true;
		// registry.descriptionDialogs.emplace(et);
	}
	else {
		printf("ERROR: effect does not exist for artifact");
	}

	// set dd description
	std::vector<Entity> dtVect;
	bool hasDT = false;
	iter = artifact_descriptions.find(artifact);
	dtVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 350.f), "DESCRIPTION: ", 1.6f, vec3(0.0f)));
	if (iter != artifact_descriptions.end()) {
		dd.description = iter->second;
		if (dd.description.size() > 40) {
			bool renderNewLine = true;
			float descOffset = 0.f;
			int iter = 1;
			std::string descLine = dd.description.substr(0, 40);
			while (renderNewLine) {
				dtVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 400.f + descOffset), descLine, 1.2f, vec3(0.0f)));
				descLine = dd.description.substr(40 * iter);
				descOffset += 30.f;
				if (descLine.size() >= 40) {
					descLine = dd.description.substr(40 * iter, 40);
					iter++;
				}
				else {
					dtVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 400.f + descOffset), descLine, 1.2f, vec3(0.0f)));
					renderNewLine = false;
				}
			}
		}
		else {
			dtVect.push_back(createDialogText(renderer, vec2(pos.x + DESCRIPTION_DIALOG_BB_WIDTH + 20.f, pos.y + DESCRIPTION_DIALOG_BB_HEIGHT / 2 + 400.f), dd.description, 1.2f, vec3(0.0f)));
		}
		
		hasDT = true;
		// registry.descriptionDialogs.emplace(dt);
	}
	else {
		printf("ERROR: description does not exist for artifact");
	}

	// need to add new entities to descriptionDialogs at the end to avoid memory issues
	registry.descriptionDialogs.emplace(icon_entity);
	if (hasTT) { registry.descriptionDialogs.emplace(tt); }
	if (hasET) { 
		for (Entity et : etVect) {
			registry.descriptionDialogs.emplace(et);
		}
	}
	if (hasDT) {
		for (Entity dt : dtVect) {
			registry.descriptionDialogs.emplace(dt);
		}
	}

	// render the x button
	auto close_entity = Entity();

	Mesh& close_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(close_entity, &close_mesh);

	auto& close_motion = registry.motions.emplace(close_entity);
	close_motion.angle = 0.f;
	close_motion.velocity = { 0.f, 0.f };
	close_motion.position = { pos.x + (DESCRIPTION_DIALOG_BB_WIDTH / 2) - 60, 
		pos.y - (DESCRIPTION_DIALOG_BB_HEIGHT / 2) + 50 };

	close_motion.scale = vec2({ PAUSE_BUTTON_BB_WIDTH / 1.5, PAUSE_BUTTON_BB_HEIGHT / 1.5 });


	Button& b = registry.buttons.emplace(entity);
	b.action_taken = BUTTON_ACTION_ID::CLOSE_DIALOG;

	// need to add 'x' to descriptionDialogs so it is closed when the entire modal is closed
	registry.descriptionDialogs.emplace(close_entity);

	registry.renderRequests.insert(
		close_entity,
		{ TEXTURE_ASSET_ID::MENU_CLOSE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::UI_TOP });

	return entity;
}

// Menu title
Entity createMenuTitle(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

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

// stylized cursor
Entity createPointer(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID texture)
{
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ POINTER_BB_WIDTH, POINTER_BB_HEIGHT });

	registry.pointers.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ texture,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::CURSOR
		});

	return entity;
}


// Attack Indicator
Entity createAttackIndicator(RenderSystem* renderer, vec2 position, float x_scale, float y_scale, bool isCircle) {

	auto entity = Entity();

	TEXTURE_ASSET_ID texture;
	if (isCircle) {
		texture = TEXTURE_ASSET_ID::ATTACK_INDICATOR_CIRCLE;
	}
	else {
		texture = TEXTURE_ASSET_ID::ATTACK_INDICATOR_RECTANGLE;
	}

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.movement_speed = 0.f;

	// Setting initial values
	motion.scale = vec2({ x_scale, y_scale });

	registry.attackIndicators.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ texture,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		 RENDER_LAYER_ID::FLOOR_DECO
		});

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
	motion.scale = vec2({ STAT_BB_WIDTH , STAT_BB_HEIGHT });

	registry.colors.insert(statEntity, {0.9f, 0.9f, 0.f});

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
			RENDER_LAYER_ID::TEXT });

	return entity;
}

Entity createDamageText(RenderSystem* renderer, vec2 pos, std::string text_input, bool is_heal=false)
{
	// Reserve en entity
	auto entity = Entity();

	// Initialize the text component
	Text& text = registry.texts.emplace(entity);
	text.message = text_input;
	text.position = { 0,0 };
	text.scale = 3.f;
	text.textColor = is_heal ? vec3(0.4, 1.0, 0.45) : vec3(1.0, 0.3, 0.25);

	// approximately center it
	vec2 offset = { text_input.length() * text.scale * 4, 0 };

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos - offset;
	motion.scale = { 0.5 * ui_scale , 0.5 * ui_scale };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXT,
			GEOMETRY_BUFFER_ID::TEXTQUAD,
			RENDER_LAYER_ID::DAMAGE_TEXT });
	registry.damageText.emplace(entity);

	return entity;
}

Entity createMotionText(RenderSystem* renderer, vec2 pos, std::string msg, float scale = 1.0f, vec3 textColor = vec3(0.0f)) {
	// Reserve en entity
	auto entity = Entity();

	// Initialize the text component
	Text& text = registry.texts.emplace(entity);
	text.message = msg;
	text.position = { 0,0 };
	text.scale = scale;
	text.textColor = textColor;

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = { 1.f * ui_scale, 1.f * ui_scale };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXT,
			GEOMETRY_BUFFER_ID::TEXTQUAD,
			RENDER_LAYER_ID::FLOOR_DECO });

	return entity;
}

// Dialog text
Entity createDialogText(RenderSystem* renderer, vec2 pos, std::string msg, float scale, vec3 textColor)
{
	// Reserve en entity
	auto entity = Entity();

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
			RENDER_LAYER_ID::DIALOG_TEXT });

	return entity;
}

// Stats text
Entity createStatsText(RenderSystem* renderer, vec2 pos, std::string msg, float scale, vec3 textColor)
{
	// Reserve en entity
	auto entity = Entity();

	// Initialize the text component
	auto& text = registry.texts.emplace(entity);
	text.message = msg;
	text.position = pos;
	text.scale = scale;
	text.textColor = textColor;

	registry.statsText.emplace(entity);

	// Create an (empty) TEXT component to be able to refer to all text
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::TEXT,
			GEOMETRY_BUFFER_ID::TEXTQUAD,
			RENDER_LAYER_ID::DIALOG_TEXT });

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
	motion.scale = { 64 * ui_scale, 64 * ui_scale };

	Interactable& interactable = registry.interactables.emplace(entity);
	interactable.type = INTERACT_TYPE::CAMPFIRE;
	
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CAMPFIRE_SPRITESHEET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::ANIMATION,
			RENDER_LAYER_ID::FLOOR_DECO });
	registry.hidables.emplace(entity);

	return entity;
}

Entity createMouseAnimation(RenderSystem* renderer, vec2 pos) {
	Entity entity = Entity();
	AnimationData& anim = registry.animations.emplace(entity);
	anim.spritesheet_texture = TEXTURE_ASSET_ID::MOUSE_SPRITESHEET;
	anim.frametime_ms = 1000;
	anim.frame_indices = { 3, 0 };
	anim.spritesheet_columns = 1;
	anim.spritesheet_rows = 4;
	anim.spritesheet_width = 50;
	anim.spritesheet_height = 200;
	anim.frame_size = { anim.spritesheet_width / anim.spritesheet_columns, anim.spritesheet_height / anim.spritesheet_rows };

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = { 64, 64 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MOUSE_SPRITESHEET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::ANIMATION,
			RENDER_LAYER_ID::EFFECT });

	return entity;
}

Entity createAttackAnimation(RenderSystem* renderer, vec2 pos, ATTACK attack) {
	Entity entity = Entity();
	AnimationData& anim = registry.animations.emplace(entity);
	anim.spritesheet_texture = TEXTURE_ASSET_ID::SLASH_SPRITESHEET;
	anim.frametime_ms = 80;
	anim.spritesheet_columns = 6;
	anim.spritesheet_rows = 3;
	anim.spritesheet_width = 756;
	anim.spritesheet_height = 450;
	anim.frame_size = { anim.spritesheet_width / anim.spritesheet_columns, anim.spritesheet_height / anim.spritesheet_rows };

	anim.loop = false;
	anim.delete_on_finish = true;

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = { 126, 150 };

	switch (attack) {
	case ATTACK::NONE:
	case ATTACK::SAPPING_STRIKE:
		anim.frame_indices = { 0,1,2,3,4,5 };
		break;
	case ATTACK::ROUNDSLASH:
		anim.frame_indices = { 6,7,8,9,10,11 };
		motion.scale = { 504, 600 };
		motion.position += vec2(0, 32);
		break;
	case ATTACK::PIERCING_THRUST:
		anim.frame_indices = { 12,13,14,15,16,17 };
		motion.scale = { 100, 200 };
		break;
	default:
		anim.frame_indices = { 0,1,2,3,4,5 };
		break;
	}

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SLASH_SPRITESHEET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::ANIMATION,
			RENDER_LAYER_ID::EFFECT });

	return entity;
}

Entity createBigSlash(RenderSystem* renderer, vec2 pos, float angle, float scale) {
	Entity entity = Entity();
	ExpandTimer& t = registry.expandTimers.emplace(entity);
	t.target_scale = scale;

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = angle;
	motion.scale = { 0.1f, 0.1f };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BIGSLASH,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::EFFECT });

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
	motion.scale = { 128 * ui_scale , 128 * ui_scale };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EXPLOSION_SPRITESHEET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::ANIMATION,
			RENDER_LAYER_ID::WALLS });

	return entity;
}

Entity createTurnUI(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = { 256.f * ui_scale, 64.f * ui_scale };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TURN_UI,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::UI });

	return entity;
}

Entity createObjectiveCounter(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = { 512.f * ui_scale, 128.f * ui_scale };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::OBJECTIVE_COUNTER,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::UI });

	return entity;
}

Entity createIcon(RenderSystem* renderer, vec2 pos, TEXTURE_ASSET_ID texture_id) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = { 48.f * ui_scale, 48.f * ui_scale };

	registry.icons.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ texture_id,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
			RENDER_LAYER_ID::UI_TOP });

	return entity;
}

Entity createSwitch(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ SWITCH_BB_WIDTH, SWITCH_BB_HEIGHT });

	// Create and (empty) DOOR component to be able to refer to all doors
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SWITCH_DEFAULT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE,
		RENDER_LAYER_ID::FLOOR_DECO});

	auto& interactable = registry.interactables.emplace(entity);
	interactable.type = INTERACT_TYPE::SWITCH;
	registry.switches.emplace(entity);

	return entity;
}

Entity createConsumable(RenderSystem* renderer, vec2 pos, CONSUMABLE type) {
	auto entity = Entity();

	// Initilaize the position, scale, and physics components (more to be changed/added)
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = pos;

	motion.scale = vec2({ PICKUP_BB_WIDTH, PICKUP_BB_HEIGHT });

	Consumable& consumable = registry.consumables.emplace(entity);
	consumable.type = type;
	
	RenderRequest& rr = registry.renderRequests.emplace(entity);
	rr.used_effect = EFFECT_ASSET_ID::TEXTURED;
	rr.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	rr.used_layer = RENDER_LAYER_ID::SPRITE;

	switch (type) {
		case CONSUMABLE::REDPOT:
			rr.used_texture = TEXTURE_ASSET_ID::POTION_RED;
			break;
		case CONSUMABLE::BLUPOT:
			rr.used_texture = TEXTURE_ASSET_ID::POTION_BLUE;
			break;
		case CONSUMABLE::YELPOT:
			rr.used_texture = TEXTURE_ASSET_ID::POTION_YELLOW;
			break;
		case CONSUMABLE::INSTANT:
			rr.used_texture = TEXTURE_ASSET_ID::POTION_RED;
			break;
		default:
			rr.used_texture = TEXTURE_ASSET_ID::POTION_RED;
			break;
	}

	auto& interactable = registry.interactables.emplace(entity);
	interactable.type = INTERACT_TYPE::PICKUP;
	registry.hidables.emplace(entity);

	return entity;
}

Entity createEnemyHPBacking(vec2 position, Entity parent)
{
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::LINE,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE,
		RENDER_LAYER_ID::HP_BACKING});
	registry.colors.insert(entity, {0,0,0});

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2(100, 8) * vec2(ui_scale, ui_scale);

	HPDisplay& hp_display = registry.hpDisplays.emplace(entity);
	hp_display.parent = parent;

	return entity;
}

Entity createEnemyHPFill(vec2 position, Entity parent)
{
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::LINE,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE,
		RENDER_LAYER_ID::HP_FILL });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2(0, 8) * vec2(ui_scale, ui_scale);

	HPDisplay& hp_display = registry.hpDisplays.emplace(entity);
	hp_display.parent = parent;

	return entity;
}