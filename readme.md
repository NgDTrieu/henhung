# 1. Giới thiệu về game Space Invaders
Game **Space Invaders** này là một trò chơi bắn súng không gian.
Luật chơi:
- Người chơi điều khiển một con tàu vũ trụ, di chuyển sang trái và phải để bắn hạ kẻ địch để ghi điểm
- Người chơi có 3 mạng, mỗi khi bị trúng đạn hoặc va chạm với kẻ thù, người chơi sẽ mất 1 mạng.
- Khi người chơi hết mạng, trò chơi kết thúc.

# 2. Logic hoạt động của hệ thống
## Cấu hình phần cứng
- Các chân PE2 và PE3 được cấu hình ở chế độ Input Pull-up Resistor.  
- 2 đầu của button, một đầu được nối với chân PE2/PE3, đầu còn lại nối với GND
- Nguyên lý hoạt động:
  - Trạng thái nghỉ (Nhả nút): Do có điện trở kéo lên, chân GPIO nhận mức điện áp cao
  - Trạng thái kích hoạt (Nhấn nút): Mạch được nối thông xuống đất, kéo mức điện áp tại chân GPIO xuống mức thấp
- Module PCM (DAC) theo sơ đồ sau:
    - **BCK (Bit Clock)** → **PB13** *(I2S2_CK)*
    - **LCK / LRCK / WS (Word Select)** → **PB12** *(I2S2_WS)*
    - **DIN (Serial Data)** → **PB15** *(I2S2_SD)*
    - **GND** → **GND**
    - **VIN** → **5V**
    - **XSMT / XMT (Enable/Unmute)** → **3V3**
    - **FMT** → **GND** *(chọn chuẩn I2S/Philips)*

## Cấu hình phần mềm
Trong file main.c, hàm main sẽ gọi tới osKernelStart(); chuyển quyền điều khiển CPU cho bộ lập lịch (scheduler) của freeRTOS. 
Hệ điều hành khi này sẽ thực hiện chạy StartDefaultTask(); và GUI_Task(); đã được khởi tạo bởi osThreadNew():

```c
// Line 229 main.c
  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of GUI_Task */
  GUI_TaskHandle = osThreadNew(TouchGFX_Task, NULL, &GUI_Task_attributes);
```
StartDefaultTask sẽ liên tục đọc trạng thái của các chân PE2, PE3 mỗi 100ms để gửi tín hiệu vào Queue cho TouchGFX_Task xử lý.
```c
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	uint32_t count1, count2, count3, count4;
	uint8_t x1,x2,x3,x4;
	for(;;)
	{
		//pg2
		count1 = osMessageQueueGetCount(Queue1Handle);
		if(count1 < 1){
			if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == GPIO_PIN_RESET){
					x1 = 'R';
			} else {
					x1 = 'N';
			}
			osMessageQueuePut(Queue1Handle, &x1, 0 , 100);
		}
		// code tương tự với PE3 cho tín hiệu sang trái
  osDelay(100);
	}
  /* USER CODE END 5 */
}
```
  
Khi người dùng nhấn vào nút Start trong màn hình Menu Screen, chuyển sang màn hình chơi game `GameScreenView`. Cụ thể: 
- Ngay sau khi khởi tạo `GameScreenView`, hàm `GameScreenView::setupScreen()` được gọi, trong hàm này thực hiện khởi tạo task game `gameTask` để quản lý các logic game.
```c
void GameScreenView::setupScreen()
{
	GameScreenViewBase::setupScreen();
  
  // Previous Code .....
	
  const osThreadAttr_t gameTask_attributes = {
	  .name = "gameTask", // Tên của task là "gameTask"
	  .stack_size = 8192 * 2,  // Kích thước của stack được cấp phát cho task là 8192 * 2 bytes
	  .priority = (osPriority_t) osPriorityNormal, // Ưu tiên của task được đặt là osPriorityNormal (ưu tiên bình thường)
	};

	  // Tạo một task mới có các thuộc tính đã được khai báo trước đó
	gameTaskHandle = osThreadNew(gameTask, NULL, &gameTask_attributes);
	shouldEndGame = false;
	shouldStopTask = false;
	shouldStopScreen = false;
}
```
- Logic hiển thị chính của màn hình chơi game được thực hiện ở trong hàm `handleTickEvent()`:
```c
void GameScreenView::handleTickEvent() {
	GameScreenViewBase::handleTickEvent();
  // Nếu cờ shouldEndGame được bật thì hiển thị menu kết thúc và score.
	if (shouldEndGame && !shouldStopScreen) {
	    shouldStopTask = true;
	    add(menu_button);
	    Unicode::snprintf(score_holderBuffer, SCORE_HOLDER_SIZE, "%d", gameInstance.score);
	    add(score_holder);
	    menu_button.invalidate();
	    score_holder.invalidate();
	    shouldStopScreen = true;

	    if (gameTaskHandle != NULL) {
	        osThreadTerminate(gameTaskHandle);
	        gameTaskHandle = NULL;
	    }
	    return;
	}


	// Lấy dữ liệu từ queue(được tạo bởi defaultTask), cập nhật vận tốc và hình ảnh của tàu.
	uint8_t res = 0;
	uint32_t count = osMessageQueueGetCount(Queue1Handle);
	if(count > 0) {
			osMessageQueueGet(Queue1Handle,&res, NULL, osWaitForever);
			if(res == 'R') {
				gameInstance.ship.updateVelocityX(gameInstance.ship.VELOCITY);
				shipImage.setBitmap(touchgfx::Bitmap(BITMAP_SHIP_RIGHT_ID));
				osMessageQueueReset(Queue2Handle);
			} else if(res == 'N'){
				gameInstance.ship.updateVelocityX(0);
				shipImage.setBitmap(touchgfx::Bitmap(BITMAP_SHIP_MAIN_ID));
			}
	}
	
  // Logic dịch chuyển sang trái tương tự
  // ................................................

	// update player position
	shipImage.moveTo(gameInstance.ship.coordinateX, gameInstance.ship.coordinateY);

	// render ship bullet
	for(int i=0; i<MAX_BULLET;i++) {
		switch(shipBullet[i].displayStatus){
		case IS_HIDDEN:
			break;
		case IS_SHOWN:
			shipBulletImage[i].moveTo(shipBullet[i].coordinateX, shipBullet[i].coordinateY);
			break;
		case SHOULD_SHOW:
			shipBulletImage[i].moveTo(shipBullet[i].coordinateX, shipBullet[i].coordinateY);
			add(shipBulletImage[i]);
			shipBullet[i].updateDisplayStatus(IS_SHOWN);
			break;
		case SHOULD_HIDE:
			remove(shipBulletImage[i]);
			shipBullet[i].updateDisplayStatus(IS_HIDDEN);
			break;
		default:
			break;
		}
	}
  // Thực hiện tương tự  để hiển thị enemy và enemy bullet
  // .....................................................

	// check if player is out of health and should end game
	if(gameInstance.ship.lives != hearts) {
		hearts = gameInstance.ship.lives;
		// reset hearts
		heart_03.setAlpha(80);
		heart_02.setAlpha(80);
		heart_01.setAlpha(80);
		if(hearts >= 1) heart_03.setAlpha(255);
		if(hearts >= 2) heart_02.setAlpha(255);
		if(hearts >= 3) heart_01.setAlpha(255);
		// If player is out of health
		if(hearts < 1) {
			shouldStopTask = true;
			invalidate();
		}
	}

	Unicode::snprintf(score_boardBuffer, SCORE_BOARD_SIZE, "%d", gameInstance.score);
	invalidate();
}
```

# 3. Phần `Application/User/src` (logic game) và cơ chế đồng bộ hiển thị
Trong project này, thư mục **`STM32CubeIDE/Application/User/src`** đóng vai trò là **"Game Engine" (logic + trạng thái)**.

## 3.1. File `app.cpp` 
File `app.cpp` chứa hàm **`gameTask()`**. Hàm này **không chạy ngay khi bật nguồn**, mà chỉ chạy khi:
- Người chơi bấm **Start** từ MenuScreen
- `GameScreenView::setupScreen()` tạo thread:

```c
// gameTask chỉ chạy sau khi GameScreen tạo thread
gameTaskHandle = osThreadNew(gameTask, NULL, &gameTask_attributes);
```

Khi game kết thúc (game over), GUI sẽ dừng thread `gameTask` bằng:
```c
osThreadTerminate(gameTaskHandle);
```

> Lưu ý: `gameTask()` chạy song song (luân phiên CPU) với GUI_Task do FreeRTOS scheduler quản lý.

## 3.2. Các đối tượng trong game và vai trò từng file
### a) `Entity.hpp/cpp` (lớp cơ sở)
- Chứa các thuộc tính chung: `coordinateX`, `coordinateY`, `width`, `height`, `velocityX`, `velocityY`, `moveRate`, `fireRate`
- Có hàm **collision AABB**:
```c
bool Entity::isCollide(Entity* other)
```
- Quan trọng nhất: **`displayStatus`** dùng để đồng bộ hiển thị với TouchGFX.

### b) `Enemy.hpp/cpp`
- Update di chuyển theo nhịp `MOVE_RATE` (bộ đếm `moveRate`)
- Update bắn theo nhịp `FIRE_RATE` (bộ đếm `fireRate`)
- Khi ra khỏi màn hình hoặc bị bắn trúng sẽ đổi `displayStatus` để ẩn.

### c) `Bullet.hpp/cpp`
- Di chuyển theo nhịp `MOVE_RATE`
- Ra khỏi màn hình thì chuyển sang trạng thái ẩn.

### d) `Ship.hpp/cpp`
- Di chuyển theo nhịp `MOVE_RATE`, giới hạn trong màn hình
- Bắn theo nhịp `FIRE_RATE`
- Quản lý `lives` (mạng)

### e) `Game.hpp/cpp`
- Lưu trạng thái tổng: `score` và `ship`
- Có hàm cập nhật điểm.

## 3.3. Vòng đời của object theo `displayStatus` (IS_HIDDEN/SHOULD_SHOW/IS_SHOWN/SHOULD_HIDE)
Đây là cơ chế quan trọng nhất để **gameTask (thread logic)** và **TouchGFX (thread GUI)** phối hợp mà không bị crash.

### (1) IS_HIDDEN
- Object đang ẩn, UI chưa add widget lên màn hình.
- Trạng thái khởi tạo ban đầu trong `gameTask()`:
  - enemy/bullet đặt ra ngoài màn hình
  - `displayStatus = IS_HIDDEN`

### (2) IS_HIDDEN → SHOULD_SHOW (logic yêu cầu hiển thị)
Xảy ra khi **spawn/bắn** trong `app.cpp`:
- Spawn enemy trong `updateEnemy(dt)`:
  - set tọa độ spawn `ex, ey` (X/Y)
  - set velocity `vx, vy`
  - `enemy[i].displayStatus = SHOULD_SHOW`
- Spawn bullet (ship/enemy):
  - set tọa độ theo ship/enemy
  - `bullet[k].displayStatus = SHOULD_SHOW`

> `SHOULD_SHOW` chỉ là "yêu cầu". Việc add widget bắt buộc phải do GUI thread làm.

### (3) SHOULD_SHOW → IS_SHOWN (GUI thực sự add widget)
Trong `GameScreenView::handleTickEvent()`:
- Nếu thấy `displayStatus == SHOULD_SHOW`:
  - `add(spriteWidget)`
  - đổi `displayStatus = IS_SHOWN`

### (4) IS_SHOWN (đang hiển thị)
- `gameTask` liên tục update `coordinateX/Y` của object
- `handleTickEvent()` mỗi tick sẽ `moveTo(x,y)` để sprite chạy theo.

### (5) IS_SHOWN → SHOULD_HIDE (logic yêu cầu ẩn)
Xảy ra khi:
- Bullet ra khỏi màn hình (`Bullet::update`)
- Enemy ra khỏi màn hình (`Enemy::update`) hoặc bị bắn trúng (collision trong `gameTask`)
- Enemy/bullet va chạm ship (collision trong `gameTask`)

Khi đó `gameTask` chỉ set:
```c
obj.displayStatus = SHOULD_HIDE;
```

### (6) SHOULD_HIDE → IS_HIDDEN (GUI remove widget)
Trong `handleTickEvent()`:
- Nếu thấy `displayStatus == SHOULD_HIDE`:
  - `remove(spriteWidget)`
  - đổi `displayStatus = IS_HIDDEN`

# 4. Phân tích chi tiết `app.cpp` (spawn/update/collision)
Trong `app.cpp`, logic game được chia thành các hàm nhỏ để cập nhật từng nhóm đối tượng.

## 4.1. Khởi tạo (init) trong `gameTask()`
Ngay khi thread `gameTask` bắt đầu, code sẽ:
- Reset điểm: `gameInstance.score = 0`
- Reset tàu: vị trí ban đầu và `lives = 3`
- Khởi tạo **pool cố định** (không cấp phát động):
  - `Enemy enemy[MAX_ENEMY]`
  - `Bullet shipBullet[MAX_BULLET]`
  - `Bullet enemyBullet[MAX_BULLET]`

Các object được khởi tạo ở trạng thái ẩn:
- Tọa độ đưa ra ngoài màn hình (ví dụ `(-50, -50)`)
- `displayStatus = IS_HIDDEN`

Tốc độ bay của đạn được set ngay lúc init:
```c
shipBullet[i].updateVelocity(0, -5); // đạn tàu bay lên (Y âm)
enemyBullet[i].updateVelocity(0,  5); // đạn địch bay xuống (Y dương)
```

## 4.2. Spawn enemy: `updateEnemy(dt)`
Enemy được spawn theo nhịp `spawnRate`:
- Mỗi lần `dt = 1` thì `spawnRate--`
- Khi `spawnRate <= 0` thì spawn 1 enemy và reset `spawnRate = MAX_SPAWN_RATE`

Vị trí spawn gồm 2 phần:
- **X (ex):** spawn từ trái hoặc phải màn hình
  - từ trái: `ex = -32`, `vx = +1`
  - từ phải: `ex = 240`, `vx = -1`
- **Y (ey):** quyết định enemy thuộc hàng nào (pixel theo chiều dọc)

`spawnSeed` được dùng để tạo pattern (trái/phải và hàng):
```c
if (spawnSeed % 2) { ex = -32; vx =  1; }
else               { ex = 240; vx = -1; }

switch (spawnSeed) {
  case 0: ey = 42;  break;
  case 1: ey = 80;  break;
  case 2: ey = 120; break;
  case 3: ey = 160; break;
}
```

Khi muốn đổi số hàng:
- 2 hàng: chỉ dùng 2 giá trị `ey` và cho `spawnSeed` quay vòng 0..1
- 3/4 hàng: thêm các giá trị `ey` tương ứng

## 4.3. Đạn của tàu: `updateShipBullet(dt)`
Tàu bắn theo nhịp `Ship::updateBullet(dt)`:
- `fireRate -= dt`
- Khi `fireRate <= 0` thì cho phép bắn và reset `fireRate = FIRE_RATE`

Khi tới lúc bắn:
- tìm viên `shipBullet[k]` đang `IS_HIDDEN`
- set vị trí theo tàu: `(sx, sy - 16)`
- set `displayStatus = SHOULD_SHOW`

Sau đó, các viên `shipBullet` đang `IS_SHOWN` sẽ được gọi `Bullet::update(dt)` để tự di chuyển.

## 4.4. Đạn của enemy: `updateEnemyBullet(dt)`
Với mỗi enemy đang `IS_SHOWN`:
- gọi `enemy[j].updateBullet(dt)` để xem đã tới lúc bắn chưa
- nếu tới lúc bắn thì tìm 1 viên `enemyBullet[k]` đang `IS_HIDDEN` để spawn tại:
  - `(enemyX, enemyY + 16)`
  - `displayStatus = SHOULD_SHOW`

## 4.5. Va chạm (collision) và cộng/trừ mạng
Sau mỗi vòng update, `gameTask` kiểm tra va chạm AABB:

### a) Enemy chạm ship
- `ship.lives--`
- `enemy.displayStatus = SHOULD_HIDE`
- reset ship về vị trí ban đầu

### b) ShipBullet trúng enemy
- `shipBullet.displayStatus = SHOULD_HIDE`
- `enemy.displayStatus = SHOULD_HIDE`
- `score += 5`

### c) EnemyBullet trúng ship
- `ship.lives--`
- `enemyBullet.displayStatus = SHOULD_HIDE`
- reset ship về vị trí ban đầu

## 4.6. Game Over
Khi `ship.lives <= 0`:
- `shouldEndGame = true`

Sau đó `GameScreenView::handleTickEvent()` sẽ:
- hiển thị `menu_button` + `score_holder`
- terminate thread `gameTask`

# Luồng phát nhạc – Space Invaders (STM32)

## 1. `music_data.h`

Chứa **toàn bộ dữ liệu nhạc** để phát. Chỉ có dữ liệu.

**`music_file`:**  
- Mảng byte `music_file[]` – file WAV đã chuyển thành mảng C (nằm trong Flash).  
- Là **nguồn nhạc**: từng mẫu 16-bit xếp liên tiếp. Code **đọc từ đây** rồi đưa vào buffer để phát.

Để phát nhạc phải **copy từ `music_file`** ra buffer rồi đẩy ra loa – việc đó làm trong **main.c**, nhờ **DMA** và **I2S**.


## 2. I2S (Inter-IC Sound)

- Nhận từng **mẫu âm thanh** (số 16-bit) theo **nhịp cố định** (vd. 48 000 mẫu/giây = 48 kHz).  
- Xuất ra **chân ngoại vi** để nối tới DAC/loa.  
- **Trong project:**: STM32 **tạo** xung clock và **gửi** dữ liệu ra. I2S **phát** những gì được **đổ vào** (từ buffer). Nguồn đổ vào đó là **RAM** (`i2s_audio_buffer`), và việc đổ từ RAM vào I2S do **DMA** đảm nhiệm.


## 3. DMA (Direct Memory Access)

```
music_file (Flash)  →  [ CPU copy qua Audio_Fill_Buffer ]  →  i2s_audio_buffer (RAM)
                                                                      │
                                                                      ▼
                                                              DMA đọc từng mẫu
                                                                      │
                                                                      ▼
                                                              I2S nhận, xuất ra chân
                                                                      │
                                                                      ▼
                                                                     Loa
```

- **CPU:** copy từ `music_file` vào buffer (gọi `Audio_Fill_Buffer`).  
- **DMA:** đọc từ `i2s_audio_buffer` (RAM), ghi vào thanh ghi Data I2S, đúng tốc độ 48 kHz — tức copy từ buffer ra I2S.  
- **I2S:** nhận từ DMA, đẩy ra loa.


## 4. `main.c` và luồng phát nhạc (ghép chung)

### 4.1. Define và biến (đầu `main.c`)

```c
#define AUDIO_BUFFER_SIZE   4096   // Kích thước buffer (byte)
// ...
int16_t i2s_audio_buffer[AUDIO_BUFFER_SIZE / 2];  // 2048 phần tử = 4096 byte
uint32_t music_cursor = 0;
```

- **`AUDIO_BUFFER_SIZE`:** Buffer 4096 byte, chia **2 nửa** (mỗi nửa 2048 byte) để double-buffering.  
- **`i2s_audio_buffer`:** Vùng RAM chứa dữ liệu **sắp được DMA gửi ra I2S**. Vừa là nơi **DMA đọc**, vừa là nơi ta **ghi** khi fill từ `music_file`.  

Mọi bước sau đều xoay quanh việc **fill buffer** và **DMA gửi buffer** ra I2S.

### 4.2. Hàm `Audio_Fill_Buffer` (main.c)

```c
void Audio_Fill_Buffer(uint8_t *dest, uint32_t len)
{
    if (music_cursor + len > music_file_len) {
        uint32_t remaining = music_file_len - music_cursor;
        memcpy(dest, &music_file[music_cursor], remaining);
        music_cursor = HEADER_OFFSET;
        memcpy(dest + remaining, &music_file[music_cursor], len - remaining);
        music_cursor += (len - remaining);
    } else {
        memcpy(dest, &music_file[music_cursor], len);
        music_cursor += len;
    }
}
```

**Luồng:** Gọi (1) khởi động → fill **cả** buffer; (2) Half callback → fill **nửa 1**; (3) Full callback → fill **nửa 2**.


### 4.3. Khởi tạo I2S – `MX_I2S2_Init` (main.c)

```c
hi2s2.Instance = SPI2;
hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_48K;
// ...
```

- Cấu hình **I2S2** (SPI2): Master phát, chuẩn Philips, **16-bit**, **48 kHz**.  
- **Trong luồng:** Đây là bước **chuẩn bị** trước khi phát. I2S chỉ bắt đầu “phát” khi có **DMA đổ dữ liệu** vào (qua `HAL_I2S_Transmit_DMA`).

### 4.4. Khởi động phát (trong `main`, trước khi tạo RTOS)

1. **`Audio_Fill_Buffer(..., 4096)`**  
   Copy **4096 byte** từ `music_file` vào **cả** `i2s_audio_buffer`. Lần đầu tiên buffer đầy đủ dữ liệu để phát.

2. **`HAL_I2S_Transmit_DMA(&hi2s2, ..., 2048)`**  
   - Nói với HAL: dùng **DMA** để gửi **2048 mẫu 16-bit** (= 4096 byte) từ `i2s_audio_buffer` ra **I2S2**.  
   - HAL sẽ cấu hình DMA **circular**: gửi hết buffer rồi **tự quay lại** gửi từ đầu, lặp mãi.  
   - Đồng thời bật **ngắt Half** (gửi xong 2048 byte đầu) và **ngắt Full** (gửi xong cả 4096 byte).  

### 4.5. Hai callback (main.c)

```c
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI2) {
        Audio_Fill_Buffer((uint8_t*)&i2s_audio_buffer[0], AUDIO_BUFFER_SIZE / 2);
    }
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI2) {
        Audio_Fill_Buffer((uint8_t*)&i2s_audio_buffer[AUDIO_BUFFER_SIZE / 4], AUDIO_BUFFER_SIZE / 2);
    }
}
```

- **Half callback:**  
  - DMA vừa gửi xong **nửa đầu** buffer (byte 0→2047) ra I2S.  
  - Nửa 1 đã được DMA “xài xong”. Trước khi DMA quay vòng và gửi lại nửa 1, ta phải **ghi đè** bằng dữ liệu mới.

- **Full callback:**  
  - DMA vừa gửi xong **nửa sau** buffer (byte 2048→4095), tức **hết** buffer.  
  - Nửa 2 vừa gửi xong. Ta cập nhật nửa 2 để lần DMA quay vòng tới sẽ phát đúng nhạc tiếp theo.


### 4.6. Luồng, sơ đồ và vai trò

| Bước | Việc xảy ra | main.c / phần cứng |
|------|-------------|--------------------|
| **0. Chuẩn bị** | `music_file` trong Flash; buffer 2 nửa; I2S, DMA init. | Define, biến, `MX_I2S2_Init`, `MX_DMA_Init`, MSP. |
| **1. Khởi động** | Cursor đầu file, fill **cả** buffer, bật DMA. | `USER CODE 2`: `music_cursor = ...`, `Audio_Fill_Buffer(...)`, `HAL_I2S_Transmit_DMA(...)`. |
| **2. DMA gửi nửa 1** | Buffer → I2S → loa. Chưa callback. | DMA + I2S nền. |
| **3. Xong nửa 1** | Ngắt Half → fill **nửa 1**. | `HAL_I2S_TxHalfCpltCallback` → `Audio_Fill_Buffer` (nửa 1). |
| **4. DMA gửi nửa 2** | Tiếp tục gửi nửa 2. | DMA + I2S nền. |
| **5. Xong cả buffer** | Ngắt Full → fill **nửa 2**. | `HAL_I2S_TxCpltCallback` → `Audio_Fill_Buffer` (nửa 2). |
| **6. Lặp** | DMA circular quay đầu buffer. | Lặp từ bước 2. |

---

# Đóng Góp
- Nguyễn Đăng Phúc: Hiển thị Giao diện (UI)
- Nguyễn Đức Triệu: Logic game (Backend)
- Trần Vương Hoàng: Ghép nối mạch và modulo âm thanh


