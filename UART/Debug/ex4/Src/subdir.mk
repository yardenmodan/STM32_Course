################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ex4/Src/Tools.c \
../ex4/Src/ex4.c 

OBJS += \
./ex4/Src/Tools.o \
./ex4/Src/ex4.o 

C_DEPS += \
./ex4/Src/Tools.d \
./ex4/Src/ex4.d 


# Each subdirectory must supply rules for building sources it contributes
ex4/Src/%.o ex4/Src/%.su ex4/Src/%.cyclo: ../ex4/Src/%.c ex4/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/UART/ex4/Inc" -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ex4-2f-Src

clean-ex4-2f-Src:
	-$(RM) ./ex4/Src/Tools.cyclo ./ex4/Src/Tools.d ./ex4/Src/Tools.o ./ex4/Src/Tools.su ./ex4/Src/ex4.cyclo ./ex4/Src/ex4.d ./ex4/Src/ex4.o ./ex4/Src/ex4.su

.PHONY: clean-ex4-2f-Src

