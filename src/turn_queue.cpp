#include "turn_queue.hpp"

void TurnQueue::setUpQueue(std::vector<Entity> entities) {
	// first we need to set up the EntityStatsStruct
	std::vector<EntityStatsStruct> esVec;
	for (Entity e : entities) {
		Stats s = registry.stats.get(e);
		esVec.push_back(EntityStatsStruct(e, s));
	}
	// now sort
	std::sort(esVec.begin(), esVec.end(), greater_than_speed());
	
	// iterate through the vector
	for (int i = 0; i < esVec.size(); i++) {
		push(esVec[i].entity);
	}
}

void TurnQueue::push(Entity entity) {
	Node* n = new Node;
	n->e = entity;
	length++;
	// if the turn queue is empty place in front
	if (!head) {
		head = n;
		tail = n;
		return;
	}
	// otherwise place in the back
	tail->next = n;
	n->prev = tail;
	tail = n;
}

void TurnQueue::addNewEntity(Entity entity) {
	// get the speed of the entity
	Stats s = registry.stats.get(entity);
	// get the player speed
	int playerSpeed;
	for (Entity player : registry.players.entities) {
		playerSpeed = registry.stats.get(player).speed;
	}

	// if entity is slower than player, just place it in the back
	if (playerSpeed > s.speed) {
		push(entity);
	}
	// else we need to iterate from the front and add the enemy where appropriate
	// increment size because we are not calling push
	Node* n = new Node;
	n->e = entity;
	length++;
	// first check head
	int headSpeed = registry.stats.get(head->e).speed;
	if (s.speed > headSpeed) {
		head->prev = n;
		n->next = head;
		head = n;
		return;
	}

	// if not we continue until we find the right place
	Node* current = head->next;
	while (current) {
		int currentSpeed = registry.stats.get(current->e).speed;
		// if the added entity is faster, place it here
		if (s.speed > currentSpeed) {
			n->prev = current->prev;
			current->prev->next = n;
			n->next = current;
			current->prev = n;
			return;
		}
		// iterate again
		current = current->next;
	}

	// we reached the end so it must be added to the tail
	tail->next = n;
	n->prev = tail;
	tail = n;
}

void TurnQueue::removeEntity(Entity e) {
	// decrement length
	length--;
	// if head is the entity to be removed
	if (head->e == e) {
		// if head is the only one
		if (tail == head) {
			tail = NULL;
			delete head;
			head = NULL;
			return;
		}
		Node* next = head->next;
		// set the next nodes prev to be null
		next->prev = NULL;
		// delete head
		delete head;
		// set next to head
		head = next;
		return;
	}
	// if tail is the entity to be removed
	if (tail->e == e) {
		// if tail is the only one
		if (tail == head) {
			tail = NULL;
			delete tail;
			head = NULL;
			return;
		}
		Node* prev = tail->prev;
		// set the prev nodes next to be null
		prev->next = NULL;
		// delete tail
		delete tail;
		// set next to head
		tail = prev;
		return;
	}
	// iterate through queue
	Node* current = head->next;
	while (current) {
		// if current is the entity to be removed
		if (current->e == e) {
			// set the current previous one to point to the next
			current->prev->next = current->next;
			// set the current next to point to the previous
			current->next->prev = current->prev;
			// delete the current
			delete current;
			return;
		}
		current = current->next;
	}
}

Entity TurnQueue::getNext() {
	// get the entity
	Entity next = head->e;
	// remove entity from list
	removeEntity(next);
	// return the entity
	return next;
}

void TurnQueue::putBackEntity(Entity entity) {
	// push entity to back of list
	push(entity);
}

int TurnQueue::size() {
	return length;
}

void TurnQueue::restructure() {
	// get the player speed
	int playerSpeed;
	for (Entity player : registry.players.entities) {
		playerSpeed = registry.stats.get(player).speed;
	}
	// now have two vectors, one for entites slower than player, one for faster than player
	std::vector<EntityStatsStruct> esSlower;
	std::vector<EntityStatsStruct> esFaster;
	while (length > 0) {
		Entity e = getNext();
		Stats s = registry.stats.get(e);
		// if speed is greater, add to greater vector
		if (s.speed > playerSpeed) {
			esFaster.push_back(EntityStatsStruct(e, s));
		}
		else { // else it is slower
			esSlower.push_back(EntityStatsStruct(e, s));
		}
	}
	// sort both vectors
	std::sort(esSlower.begin(), esSlower.end(), greater_than_speed());
	std::sort(esFaster.begin(), esFaster.end(), greater_than_speed());
	// now go through slower and put in queue first
	for (EntityStatsStruct es : esSlower) {
		push(es.entity);
	}
	// now go through faster and put in queue
	for (EntityStatsStruct es : esFaster) {
		push(es.entity);
	}
}