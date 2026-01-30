/*
 * Entity.cpp
 */

#include "Entity.hpp"

Entity::Entity() {
	// TODO Auto-generated constructor stub
	this->displayStatus = IS_HIDDEN;
	this->width = 32;
	this->height = 32;
}

Entity::~Entity() {
	// TODO Auto-generated destructor stub
}

bool Entity::update() {
	// TODO Auto-generated destructor stub
	return false;
}

void Entity::updateDisplayStatus(uint8_t status) {
	this->displayStatus = status;
}
void Entity::updateCoordinate(uint16_t x, uint16_t y) {
	this->coordinateX = x;
	this->coordinateY = y;
}

void Entity::updateVelocity(uint16_t x, uint16_t y) {
	this->velocityX = x;
	this->velocityY = y;
}

bool Entity::isCollide(Entity &a, Entity &b) {
	if((a.coordinateX + a.width > b.coordinateX) && (b.coordinateX + b.width > a.coordinateX) &&
			(a.coordinateY + a.height > b.coordinateY) && (b.coordinateY + b.height > a.coordinateY))
		return true;
	return false;
}

