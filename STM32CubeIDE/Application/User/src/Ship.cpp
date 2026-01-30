/*
 * Ship.cpp
 */

#include "Ship.hpp"


Ship::Ship() {
	// TODO Auto-generated constructor stub
	this->coordinateX = 104;
	this->coordinateY = 260;
	this->velocityX = 0;
	this->velocityY = 0;
	this->width = 28;
	this->height = 28;
	this->fireRate = FIRE_RATE;
	this->moveRate = MOVE_RATE;
}

Ship::~Ship() {
	// TODO Auto-generated destructor stub

}

bool Ship::update(uint8_t dt) {
	this->moveRate -= dt;
	if(this->moveRate > 0) return false;
	this->coordinateX += this->velocityX;
	this->coordinateY += this->velocityY;
	this->moveRate = MOVE_RATE;

	if(this->coordinateX < 0) this->coordinateX = 0;
	else if(this->coordinateX > 208) this->coordinateX = 208;
	if(this->coordinateY < 0) this->coordinateY = 0;
	else if(this->coordinateY > 288) this->coordinateY = 288;

	return true;
}

bool Ship::updateBullet(uint8_t dt) {
	this->fireRate -= dt;
	if(this->fireRate > 0) return false;
	this->fireRate = FIRE_RATE;
	return true;
}

void Ship::updateShipHp(short change) {
 this->lives += change;
}

void Ship::updateVelocityX(short X) {
	this->velocityX = X;
}
void Ship::updateVelocityY(short Y) {
	this->velocityY = Y;
}

