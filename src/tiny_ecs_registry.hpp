#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Inventory> inventories;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<Damageable> damageables;
	ComponentContainer<Consumable> consumables;
	ComponentContainer<Equipment> equipment;
	ComponentContainer<Guardable> guardables;
	ComponentContainer<Interactable> interactables;
	ComponentContainer<Stats> stats;
	ComponentContainer<Stats> basestats;
	ComponentContainer<Queueable> queueables;
	ComponentContainer<Test> test;	// for testing
	ComponentContainer<Solid> solid;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<KeyIcon> keyIcons;
	ComponentContainer<ActionButton> actionButtons;
	ComponentContainer<GuardButton> guardButtons;
	ComponentContainer<Button> buttons;
	ComponentContainer<Pointer> pointers;
	ComponentContainer<DescriptionDialog> descriptionDialogs;
	ComponentContainer<ArtifactIcon> artifactIcons;
	ComponentContainer<MenuItem> menuItems;
	ComponentContainer<ModeVisualization> modeVisualizationObjects;
	ComponentContainer<EpRange> epRange;
	ComponentContainer<Fog> fog;
	ComponentContainer<Camera> cameras;
	ComponentContainer<Text> texts;
	ComponentContainer<TextTimer> textTimers;
	ComponentContainer<SquishTimer> squishTimers;
	ComponentContainer<WobbleTimer> wobbleTimers;
	ComponentContainer<ProjectileTimer> projectileTimers;
	ComponentContainer<Test> hidden;
	ComponentContainer<Test> hidables;
	ComponentContainer<TileUV> tileUVs;
	ComponentContainer<Collidable> collidables;
	// add door component
	ComponentContainer<Door>door;
	ComponentContainer<Artifact> artifacts;
	ComponentContainer<AnimationData> animations;
	ComponentContainer<Sign> signs;
	ComponentContainer<StatusContainer> statuses;
	ComponentContainer<KnockBack> knockbacks;
	ComponentContainer<AttackCard> attackCards;
	ComponentContainer<ItemCard> itemCards;
	ComponentContainer<AttackDialog> attackDialogs;
	ComponentContainer<Icon> icons;
	ComponentContainer<Spritesheet> spritesheets;
	ComponentContainer<RoomTransitionTimer> roomTransitions;
	ComponentContainer<FadeInTimer> fadeins;
	ComponentContainer<LoadingTimer> loadingTimers;
	ComponentContainer<Switch> switches;  
	ComponentContainer<Chest> chests;
	ComponentContainer<DamageTextTimer> damageText;
	ComponentContainer<StatsText> statsText;
	ComponentContainer<ExpandTimer> expandTimers;
	ComponentContainer<HPDisplay> hpDisplays;
	ComponentContainer<EnemyHPBar> enemyHPBars;
	ComponentContainer<BossHPBar> bossHPBars;
	ComponentContainer<ShadowContainer> shadowContainers;
	ComponentContainer<Shadow> shadows;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&inventories);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&damageables);
		registry_list.push_back(&consumables);
		registry_list.push_back(&equipment);
		registry_list.push_back(&guardables);
		registry_list.push_back(&interactables);
		registry_list.push_back(&stats);
		registry_list.push_back(&basestats);
		registry_list.push_back(&queueables);
		registry_list.push_back(&test);
		registry_list.push_back(&solid);
		registry_list.push_back(&enemies);
		registry_list.push_back(&keyIcons);
		registry_list.push_back(&actionButtons);
		registry_list.push_back(&guardButtons);
		registry_list.push_back(&buttons);
		registry_list.push_back(&artifactIcons);
		registry_list.push_back(&modeVisualizationObjects);
		registry_list.push_back(&menuItems);
		registry_list.push_back(&epRange);
		registry_list.push_back(&fog);
		registry_list.push_back(&cameras);
		registry_list.push_back(&texts);
		registry_list.push_back(&textTimers);
		registry_list.push_back(&squishTimers);
		registry_list.push_back(&wobbleTimers);
		registry_list.push_back(&projectileTimers);
		registry_list.push_back(&signs);
		registry_list.push_back(&hidden);
		registry_list.push_back(&hidables);
		registry_list.push_back(&pointers);
		registry_list.push_back(&descriptionDialogs);
		// door in registry list 
		registry_list.push_back(&door);
		// add artifact to list
		registry_list.push_back(&artifacts);
		registry_list.push_back(&tileUVs);
		registry_list.push_back(&collidables);
		registry_list.push_back(&animations);
		registry_list.push_back(&statuses);
		registry_list.push_back(&knockbacks);
		registry_list.push_back(&attackCards);
		registry_list.push_back(&attackDialogs);
		registry_list.push_back(&itemCards);
		registry_list.push_back(&icons);
		registry_list.push_back(&spritesheets);
		registry_list.push_back(&roomTransitions);
		registry_list.push_back(&fadeins);
		registry_list.push_back(&loadingTimers);
		registry_list.push_back(&switches);
		registry_list.push_back(&chests);
		registry_list.push_back(&damageText);
		registry_list.push_back(&statsText);
		registry_list.push_back(&expandTimers);
		registry_list.push_back(&hpDisplays);
		registry_list.push_back(&enemyHPBars);
		registry_list.push_back(&bossHPBars);
		registry_list.push_back(&shadowContainers);
		registry_list.push_back(&shadows);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;