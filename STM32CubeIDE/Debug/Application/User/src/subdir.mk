################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Application/User/src/Bullet.cpp \
../Application/User/src/Enemy.cpp \
../Application/User/src/Entity.cpp \
../Application/User/src/Game.cpp \
../Application/User/src/Ship.cpp \
../Application/User/src/app.cpp 

OBJS += \
./Application/User/src/Bullet.o \
./Application/User/src/Enemy.o \
./Application/User/src/Entity.o \
./Application/User/src/Game.o \
./Application/User/src/Ship.o \
./Application/User/src/app.o 

CPP_DEPS += \
./Application/User/src/Bullet.d \
./Application/User/src/Enemy.d \
./Application/User/src/Entity.d \
./Application/User/src/Game.d \
./Application/User/src/Ship.d \
./Application/User/src/app.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/src/%.o Application/User/src/%.su Application/User/src/%.cyclo: ../Application/User/src/%.cpp Application/User/src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F429xx -c -I../../Core/Inc -I../../Application/User/inc -I../../Drivers/CMSIS/Include -I../../Drivers/BSP -I../../TouchGFX/target -I../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../TouchGFX/App -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../TouchGFX/target/generated -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/gui/include -I../../TouchGFX/generated/videos/include -I../../Application/User/src -I../../Application/User/src -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-src

clean-Application-2f-User-2f-src:
	-$(RM) ./Application/User/src/Bullet.cyclo ./Application/User/src/Bullet.d ./Application/User/src/Bullet.o ./Application/User/src/Bullet.su ./Application/User/src/Enemy.cyclo ./Application/User/src/Enemy.d ./Application/User/src/Enemy.o ./Application/User/src/Enemy.su ./Application/User/src/Entity.cyclo ./Application/User/src/Entity.d ./Application/User/src/Entity.o ./Application/User/src/Entity.su ./Application/User/src/Game.cyclo ./Application/User/src/Game.d ./Application/User/src/Game.o ./Application/User/src/Game.su ./Application/User/src/Ship.cyclo ./Application/User/src/Ship.d ./Application/User/src/Ship.o ./Application/User/src/Ship.su ./Application/User/src/app.cyclo ./Application/User/src/app.d ./Application/User/src/app.o ./Application/User/src/app.su

.PHONY: clean-Application-2f-User-2f-src

