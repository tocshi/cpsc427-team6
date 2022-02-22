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
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<Eatable> eatables;
	ComponentContainer<Deadly> deadlys;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;

	// =============================
	ComponentContainer<Damageable> damageables;
	ComponentContainer<Consumable> consumables;
	ComponentContainer<Equipable> equipables;
	ComponentContainer<Guardable> guardables;
	ComponentContainer<Interactable> interactables;
	ComponentContainer<Stat> stats;
	ComponentContainer<Queueable> queueables;
	ComponentContainer<Test> test;	// for testing
	ComponentContainer<Solid> solid;
	ComponentContainer<SlimeEnemy> slimeEnemies;
	ComponentContainer<Button> buttons;
	ComponentContainer<MenuItem> menuItems;
	ComponentContainer<Fog> fog;
	ComponentContainer<Camera> cameras;
	// add door component
	ComponentContainer<Door>door;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&eatables);
		registry_list.push_back(&deadlys);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);

		// =========================
		registry_list.push_back(&damageables);
		registry_list.push_back(&consumables);
		registry_list.push_back(&equipables);
		registry_list.push_back(&guardables);
		registry_list.push_back(&interactables);
		registry_list.push_back(&stats);
		registry_list.push_back(&queueables);
		registry_list.push_back(&test);
		registry_list.push_back(&solid);
		registry_list.push_back(&slimeEnemies);
		registry_list.push_back(&buttons);
		registry_list.push_back(&menuItems);
		registry_list.push_back(&fog);
		registry_list.push_back(&cameras);
		// door in registry list 
		registry_list.push_back(&door);
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