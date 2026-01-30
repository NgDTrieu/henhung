/*
 * Entity.h
 */

#ifndef APPLICATION_USER_SRC_ENTITY_HPP_
#define APPLICATION_USER_SRC_ENTITY_HPP_

#include <stdint.h>

const uint8_t IS_SHOWN = 1;
const uint8_t SHOULD_SHOW = 2;
const uint8_t IS_HIDDEN = 3;
const uint8_t SHOULD_HIDE = 4;

class Entity {
public:
	Entity();
	short coordinateX;
	short coordinateY;
	short width;
	short height;
	short velocityX;
	short velocityY;
	short displayStatus;
	short moveRate;
	short fireRate;
	void updateDisplayStatus(uint8_t status);
	void updateCoordinate(uint16_t x, uint16_t y);
	void updateVelocity(uint16_t x, uint16_t y);
	virtual bool update();
	virtual ~Entity();
	static bool isCollide(Entity &entity1, Entity &entity2);
};

#endif /* APPLICATION_USER_SRC_ENTITY_HPP_ */
