################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ex3/Src/Tools.c \
../ex3/Src/ex3.c 

OBJS += \
./ex3/Src/Tools.o \
./ex3/Src/ex3.o 

C_DEPS += \
./ex3/Src/Tools.d \
./ex3/Src/ex3.d 


# Each subdirectory must supply rules for building sources it contributes
ex3/Src/%.o ex3/Src/%.su ex3/Src/%.cyclo: ../ex3/Src/%.c ex3/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/UART/ex3/Inc" -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ex3-2f-Src

clean-ex3-2f-Src:
	-$(RM) ./ex3/Src/Tools.cyclo ./ex3/Src/Tools.d ./ex3/Src/Tools.o ./ex3/Src/Tools.su ./ex3/Src/ex3.cyclo ./ex3/Src/ex3.d ./ex3/Src/ex3.o ./ex3/Src/ex3.su

.PHONY: clean-ex3-2f-Src

