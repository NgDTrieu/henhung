/*
 * Bullet.cpp
 */

#include "Bullet.hpp"

Bullet::Bullet() {
	// Khởi tạo kích thước mặc định cho đối tượng đạn
	this->width = 24;
	this->height = 24;

	// Khởi tạo trạng thái hiển thị và vận tốc của đạn
	this->displayStatus = IS_HIDDEN;
	this->velocityX = 0;
	this->velocityY = 0;

	// Khởi tạo tỷ lệ di chuyển của đạn (tính bằng mili giây)
	this->moveRate = MOVE_RATE;
}

Bullet::~Bullet() {

}

bool Bullet::update(uint8_t dt) {
	// Kiểm tra xem đạn có nằm ngoài khu vực màn hình hiển thị không
	if(this->coordinateX < -this->width || this->coordinateX > 240 ||
			this->coordinateY < -this->height || this->coordinateY > 320) {

		// Nếu đạn nằm ngoài màn hình, đặt trạng thái hiển thị thành "SHOULD_HIDE"
		this->displayStatus = SHOULD_HIDE;
		// Đặt lại tỷ lệ di chuyển để chuẩn bị cho lần di chuyển tiếp theo
		this->moveRate = MOVE_RATE;
		return false;
	}
	// Giảm tỷ lệ di chuyển của đạn bằng thời gian đã trôi qua
	this->moveRate -= dt;
	// Nếu tỷ lệ di chuyển còn lớn hơn 0, đạn không thể di chuyển trong lần cập nhật này
	if(this->moveRate > 0) return false;

	// Cập nhật vị trí mới của đạn dựa vào vận tốc hiện tại
	this->coordinateX += this->velocityX;
	this->coordinateY += this->velocityY;

	// Đặt lại tỷ lệ di chuyển để chuẩn bị cho lần di chuyển tiếp theo
	this->moveRate = MOVE_RATE;

	// Trả về true để cho biết đạn vẫn còn hiển thị và trong khu vực màn hình
	return true;
}

