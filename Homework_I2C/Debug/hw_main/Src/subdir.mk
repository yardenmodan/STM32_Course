################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hw_main/Src/hw_main.c 

OBJS += \
./hw_main/Src/hw_main.o 

C_DEPS += \
./hw_main/Src/hw_main.d 


# Each subdirectory must supply rules for building sources it contributes
hw_main/Src/%.o hw_main/Src/%.su hw_main/Src/%.cyclo: ../hw_main/Src/%.c hw_main/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/Homework_I2C/hw_main/Inc" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-hw_main-2f-Src

clean-hw_main-2f-Src:
	-$(RM) ./hw_main/Src/hw_main.cyclo ./hw_main/Src/hw_main.d ./hw_main/Src/hw_main.o ./hw_main/Src/hw_main.su

.PHONY: clean-hw_main-2f-Src

