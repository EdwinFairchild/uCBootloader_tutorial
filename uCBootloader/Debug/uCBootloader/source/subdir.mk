################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../uCBootloader/source/bootloader.c 

OBJS += \
./uCBootloader/source/bootloader.o 

C_DEPS += \
./uCBootloader/source/bootloader.d 


# Each subdirectory must supply rules for building sources it contributes
uCBootloader/source/%.o: ../uCBootloader/source/%.c uCBootloader/source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"F:/YouTube_Workspace/uCBootloader_Tutorial/uCBootloader/uCBootloader/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-uCBootloader-2f-source

clean-uCBootloader-2f-source:
	-$(RM) ./uCBootloader/source/bootloader.d ./uCBootloader/source/bootloader.o

.PHONY: clean-uCBootloader-2f-source

