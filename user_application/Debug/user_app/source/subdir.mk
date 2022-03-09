################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user_app/source/user_app.c 

OBJS += \
./user_app/source/user_app.o 

C_DEPS += \
./user_app/source/user_app.d 


# Each subdirectory must supply rules for building sources it contributes
user_app/source/%.o: ../user_app/source/%.c user_app/source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"F:/YouTube_Workspace/uCBootloader_Tutorial/user_application/user_app/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-user_app-2f-source

clean-user_app-2f-source:
	-$(RM) ./user_app/source/user_app.d ./user_app/source/user_app.o

.PHONY: clean-user_app-2f-source

