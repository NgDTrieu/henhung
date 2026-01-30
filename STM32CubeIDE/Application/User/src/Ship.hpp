/*
 * Ship.h
 */

#ifndef APPLICATION_USER_SRC_SHIP_HPP_
#define APPLICATION_USER_SRC_SHIP_HPP_

#include <BitmapDatabase.hpp>
#include <touchgfx/widgets/Image.hpp>
#include "Entity.hpp"

class Ship: public Entity {
public:
	Ship();
	virtual ~Ship();
	bool update(uint8_t dt);
	int MOVE_RATE = 400;
	int FIRE_RATE = 10000;
	int VELOCITY = 3;
	void updateVelocityX(short X);
	void updateVelocityY(short Y);
	void changeShipImage();
	void updateShipHp(short change);
	bool updateBullet(uint8_t dt);
	short lives = 3;

};

#endif /* APPLICATION_USER_SRC_SHIP_HPP_ */
