################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ex2/Src/Tools.c \
../ex2/Src/ex2.c 

OBJS += \
./ex2/Src/Tools.o \
./ex2/Src/ex2.o 

C_DEPS += \
./ex2/Src/Tools.d \
./ex2/Src/ex2.d 


# Each subdirectory must supply rules for building sources it contributes
ex2/Src/%.o ex2/Src/%.su ex2/Src/%.cyclo: ../ex2/Src/%.c ex2/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/UART/ex2/Inc" -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ex2-2f-Src

clean-ex2-2f-Src:
	-$(RM) ./ex2/Src/Tools.cyclo ./ex2/Src/Tools.d ./ex2/Src/Tools.o ./ex2/Src/Tools.su ./ex2/Src/ex2.cyclo ./ex2/Src/ex2.d ./ex2/Src/ex2.o ./ex2/Src/ex2.su

.PHONY: clean-ex2-2f-Src

