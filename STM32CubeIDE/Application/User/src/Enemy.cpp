/*
 * Enemy.cpp
 */

#include "Enemy.hpp"
Enemy::Enemy() {
	// Khởi tạo kích thước mặc định cho đối tượng kẻ địch
	this->width = 32;
	this->height = 32;

	// Khởi tạo trạng thái hiển thị, vận tốc di chuyển và tốc độ di chuyển của kẻ địch
	this->displayStatus = IS_HIDDEN;
	this->velocityX = 0;
	this->velocityY = 0;
  this->moveRate = MOVE_RATE;

  // Khởi tạo tốc độ bắn đạn của kẻ địch
  this->fireRate = FIRE_RATE;
}

Enemy::~Enemy() {

}

bool Enemy::update(uint8_t dt) {
	// Kiểm tra xem kẻ địch có nằm ngoài khu vực màn hình hiển thị không
	if(this->coordinateX < -this->width || this->coordinateX > 240 ||
			this->coordinateY < -this->height || this->coordinateX > 320) {
		// Nếu kẻ địch nằm ngoài màn hình, đặt trạng thái hiển thị thành "SHOULD_HIDE"
		this->displayStatus = SHOULD_HIDE;

		// Đặt lại tốc độ bắn đạn và tốc độ di chuyển để chuẩn bị cho lần xuất hiện kế tiếp
		this->fireRate = FIRE_RATE;
		this->moveRate = MOVE_RATE;
		return false;
	}

	// Giảm tốc độ di chuyển của kẻ địch bằng thời gian đã trôi qua
	this->moveRate -= dt;

	// Nếu tốc độ di chuyển còn lớn hơn 0, kẻ địch không thể di chuyển trong lần cập nhật này
	if(this->moveRate > 0) return false;

	// Cập nhật vị trí mới của kẻ địch dựa vào vận tốc hiện tại
	this->coordinateX += this->velocityX;
	this->coordinateY += this->velocityY;

	// Đặt lại tốc độ di chuyển để chuẩn bị cho lần di chuyển tiếp theo
	this->moveRate = MOVE_RATE;

	// Trả về true để cho biết kẻ địch vẫn còn hiển thị và trong khu vực màn hình
	return true;
}

bool Enemy::updateBullet(uint8_t dt) {
	this->fireRate -= dt;
	if(this->fireRate > 0) return false;
	this->fireRate = FIRE_RATE;
	return true;
}

