#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>

struct EntityStatsStruct {
	Entity entity;
	Stats stats;
	EntityStatsStruct(Entity e, Stats s) : entity(e), stats(s) {}
};

struct greater_than_speed {
	inline bool operator() (const EntityStatsStruct& struct1, const EntityStatsStruct& struct2)
	{
		return struct1.stats.speed > struct2.stats.speed;
	}
};

struct Node {
	Entity e;
	Node* next;
	Node* prev;
};

class TurnQueue {
public:
	void setUpQueue(std::vector<Entity> entities);
	void addNewEntity(Entity entity);
	void removeEntity(Entity entity);
	Entity getNext();
	void putBackEntity(Entity entity);
	int size();
	void restructure();
	void emptyQueue();
private:
	Node* head;
	Node* tail;
	int length = 0;
	void push(Entity entity);
};