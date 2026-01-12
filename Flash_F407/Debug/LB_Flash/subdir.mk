################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LB_Flash/CRC.c \
../LB_Flash/Flash_control.c \
../LB_Flash/bootloader.c \
../LB_Flash/bootloder_jumpto_app.c 

OBJS += \
./LB_Flash/CRC.o \
./LB_Flash/Flash_control.o \
./LB_Flash/bootloader.o \
./LB_Flash/bootloder_jumpto_app.o 

C_DEPS += \
./LB_Flash/CRC.d \
./LB_Flash/Flash_control.d \
./LB_Flash/bootloader.d \
./LB_Flash/bootloder_jumpto_app.d 


# Each subdirectory must supply rules for building sources it contributes
LB_Flash/%.o LB_Flash/%.su LB_Flash/%.cyclo: ../LB_Flash/%.c LB_Flash/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/Knnn/STM32CubeIDE/workspace_1.15.1/Flash_F407/LB_Flash" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-LB_Flash

clean-LB_Flash:
	-$(RM) ./LB_Flash/CRC.cyclo ./LB_Flash/CRC.d ./LB_Flash/CRC.o ./LB_Flash/CRC.su ./LB_Flash/Flash_control.cyclo ./LB_Flash/Flash_control.d ./LB_Flash/Flash_control.o ./LB_Flash/Flash_control.su ./LB_Flash/bootloader.cyclo ./LB_Flash/bootloader.d ./LB_Flash/bootloader.o ./LB_Flash/bootloader.su ./LB_Flash/bootloder_jumpto_app.cyclo ./LB_Flash/bootloder_jumpto_app.d ./LB_Flash/bootloder_jumpto_app.o ./LB_Flash/bootloder_jumpto_app.su

.PHONY: clean-LB_Flash

