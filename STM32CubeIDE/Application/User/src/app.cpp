/*
 * app.cpp
 */
#include "app.hpp"
#include <cmsis_os2.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "stack_macros.h"

// Khởi tạo đối tượng trò chơi và các mảng đạn và kẻ địch
Game gameInstance;
Bullet shipBullet[MAX_BULLET];
Bullet enemyBullet[MAX_BULLET];
Enemy enemy[MAX_ENEMY];

// Các hằng số và biến dùng để quản lý tốc độ xuất hiện của kẻ địch
#define MAX_SPAWN_RATE 15000
int spawnRate = MAX_SPAWN_RATE; // Quyết định tần suất tạo lại một enemy mới
int spawnSeed = 0;

// Biến để lưu trữ vị trí của tàu và các cờ để điều khiển trạng thái trò chơi
uint16_t sx, sy;
bool shouldEndGame;
bool shouldStopTask;


// Các hàm cập nhật vị trí của kẻ địch, đạn tàu và đạn kẻ địch
void updateEnemy(uint8_t dt);
void updateShipBullet(uint8_t dt);
void updateEnemyBullet(uint8_t dt);

// Biến lưu thời gian đã trôi qua
uint8_t dt = 0;

// Nhiệm vụ của task game
void gameTask(void *argument) {
	int loopCount = 0;

	// Khởi tạo vị trí và trạng thái ban đầu cho đạn và kẻ địch
	for(int i=0; i<MAX_BULLET;i++) {
		shipBullet[i].updateCoordinate(-50, -50);
		enemyBullet[i].updateCoordinate(-50, -50);
		shipBullet[i].updateVelocity(0, -5);
		enemyBullet[i].updateVelocity(0, 3);
		shipBullet[i].updateDisplayStatus(IS_HIDDEN);
		enemyBullet[i].updateDisplayStatus(IS_HIDDEN);
	}

	for(int i=0; i<MAX_ENEMY;i++) {
		enemy[i].updateCoordinate(-50, -50);
		enemy[i].updateDisplayStatus(IS_HIDDEN);
	}

//	uint32_t previousTick = 0, currentTick = 0;
	for(;;) {
		loopCount++;
		// Nếu cờ kết thúc trò chơi và dừng task được đặt, thoát khỏi vòng lặp
		if(shouldEndGame == true && shouldStopTask == true) break;

		// Nếu cờ kết thúc trò chơi đã được đặt, tiếp tục vòng lặp
		else if (shouldEndGame) {
		    osDelay(0);      // hoặc osThreadYield();
		    continue;
		}

		// Xác định thời gian đã trôi qua từ lần cập nhật trước đó
		if(loopCount >= 2) {
			dt = 1;
			loopCount = 0;
		} else {
			dt = 0;
		}

		/*  UPDATE POSITIONS */

		// Cập nhật vị trí tàu và các đối tượng trong trò chơi
		sx = gameInstance.ship.coordinateX;
		sy = gameInstance.ship.coordinateY;

		// update enemy
		updateEnemy(dt);

		// update ship position
		gameInstance.ship.update(dt);

		// update ship bullet position
		updateShipBullet(dt);

		// update enemy bullet position
		updateEnemyBullet(dt);


		/*  CHECK COLLISIONS */

		// check enemy and ship collision

		// Kiểm tra va chạm giữa các đối tượng
		// Kiểm tra va chạm giữa tàu và kẻ địch
		for(int i=0;i<MAX_ENEMY;i++) {
			if(enemy[i].displayStatus != IS_SHOWN) continue;
			if(Entity::isCollide(enemy[i], gameInstance.ship)) {
				gameInstance.ship.updateShipHp(-1);
				enemy[i].updateDisplayStatus(SHOULD_HIDE);
				gameInstance.ship.updateCoordinate(104, 260);
				break;
			}
		}

		// Kiểm tra va chạm giữa kẻ địch và đạn của tàu
		for(int i=0;i<MAX_ENEMY;i++) {
			if(enemy[i].displayStatus != IS_SHOWN) continue;
			for(int j=0;j<MAX_BULLET;j++) {
				if(shipBullet[j].displayStatus != IS_SHOWN) continue;
				if(Entity::isCollide(enemy[i], shipBullet[j])) {
					enemy[i].updateDisplayStatus(SHOULD_HIDE);
					shipBullet[j].updateDisplayStatus(SHOULD_HIDE);
					gameInstance.updateScore(5);
					break;
				}
			}
		}

		// Kiểm tra va chạm giữa tàu và đạn của kẻ địch
		for(int i=0;i<MAX_BULLET;i++) {
			if(enemyBullet[i].displayStatus != IS_SHOWN) continue;
			if(Entity::isCollide(enemyBullet[i], gameInstance.ship)) {
				gameInstance.ship.updateShipHp(-1);
				enemyBullet[i].updateDisplayStatus(SHOULD_HIDE);
				gameInstance.ship.updateCoordinate(104, 260);
				break;
			}
		}

		// Nếu tàu hỏng hết máu, đặt cờ kết thúc trò chơi
		if (gameInstance.ship.lives <= 0) {
		    shouldEndGame = true;
		}

	}
}

// Cập nhật vị trí của kẻ địch
void updateEnemy(uint8_t dt) {
	spawnRate -=dt;
	for(int i=0;i<MAX_ENEMY;i++) {
		// Nếu kẻ địch đang hiển thị, tiếp tục cập nhật vị trí
		if(enemy[i].displayStatus != IS_HIDDEN) {
			enemy[i].update(dt);
			continue;
		}
		// Nếu một đối tượng đã ẩn -> tạo lại nó
		if(spawnRate > 0) continue; // Chưa đến lúc tạo lại

		uint16_t ex = 0;
		uint16_t ey = 0;
		uint16_t vx;
		uint16_t vy = 0;

		// Tạo mới kẻ địch từ vị trí và hướng di chuyển đã được xác định bởi spawnSeed
		if(spawnSeed%2) {
			ex = -32;
			vx = 1;
		} else {
			ex = 240;
			vx = -1;
		}

		// hàng chẵn và hàng lẻ
//		switch(spawnSeed) {
//		case 0:
//			ey = 42;
//			break;
//		case 1:
//			ey = 80;
//			break;
//		case 2:
//			ey = 120;
//			break;
//		case 3:
//			ey = 160;
//			break;
//		default:
//			break;
//		}
//		// Xác định hướng di chuyển dọc của kẻ địch bằng cách cập nhật lại spawnSeed
//		if(spawnSeed >= 3) {
//			spawnSeed = 0;
//		} else {
//			spawnSeed++;
//		}

		switch (spawnSeed) {
		case 0:
		    ey = 42;   // hàng 1 (bạn có thể đổi số này)
		    break;
		case 1:
		    ey = 80;  // hàng 2 (bạn có thể đổi số này)
		    break;
		default:
		    ey = 42;
		    break;
		}

		// chỉ quay 0 và 1
		spawnSeed = (spawnSeed >= 1) ? 0 : (spawnSeed + 1);

		// Cập nhật thông tin của kẻ địch và hiển thị lên màn hình
		enemy[i].updateCoordinate(ex, ey);
		enemy[i].updateVelocity(vx, vy);
		enemy[i].updateDisplayStatus(SHOULD_SHOW);

		// Đặt lại spawnRate để đợi cho lần xuất hiện kế tiếp
		spawnRate = MAX_SPAWN_RATE;
	}
}

// Cập nhật vị trí của đạn từ tàu
void  updateShipBullet(uint8_t dt) {
	// kiem tra tau con het dan chua
	bool shouldFire = gameInstance.ship.updateBullet(dt);
	for(int i=0; i<MAX_BULLET;i++) {
			if(shipBullet[i].displayStatus != IS_HIDDEN) {
				shipBullet[i].update(dt);
				continue;
			}

			if(shouldFire) {
				shipBullet[i].updateCoordinate(sx, sy - 16);
				shipBullet[i].updateDisplayStatus(SHOULD_SHOW);
				shouldFire = false;
			}
		}
}

// Cập nhật vị trí của đạn từ kẻ địch

void updateEnemyBullet(uint8_t dt) {
	for(int j=0; j<MAX_ENEMY;j++) {
		if(enemy[j].displayStatus != IS_SHOWN) continue;

		// kiem tra ke dich con dan khong
		if(enemy[j].updateBullet(dt) == false) continue;

		for(int i=0; i<MAX_BULLET;i++) {
			if(enemyBullet[i].displayStatus != IS_HIDDEN) continue;

			// Tạo mới đạn từ kẻ địch và hiển thị lên màn hình
			enemyBullet[i].updateCoordinate(enemy[j].coordinateX, enemy[j].coordinateY + 16);
			enemyBullet[i].updateDisplayStatus(SHOULD_SHOW);
			break;
		}
	}

	// Cập nhật vị trí của các đạn từ kẻ địch đã xuất hiện
	for(int i=0; i<MAX_BULLET;i++) {
		if(enemyBullet[i].displayStatus != IS_SHOWN) continue;
		enemyBullet[i].update(dt);
	}

}
