################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DMA_HW/Src/hw_main.c 

OBJS += \
./DMA_HW/Src/hw_main.o 

C_DEPS += \
./DMA_HW/Src/hw_main.d 


# Each subdirectory must supply rules for building sources it contributes
DMA_HW/Src/%.o DMA_HW/Src/%.su DMA_HW/Src/%.cyclo: ../DMA_HW/Src/%.c DMA_HW/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/Homework_DMA/DMA_HW/Inc" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DMA_HW-2f-Src

clean-DMA_HW-2f-Src:
	-$(RM) ./DMA_HW/Src/hw_main.cyclo ./DMA_HW/Src/hw_main.d ./DMA_HW/Src/hw_main.o ./DMA_HW/Src/hw_main.su

.PHONY: clean-DMA_HW-2f-Src

