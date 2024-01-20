################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ex1/Src/Tools.c \
../ex1/Src/ex1.c 

OBJS += \
./ex1/Src/Tools.o \
./ex1/Src/ex1.o 

C_DEPS += \
./ex1/Src/Tools.d \
./ex1/Src/ex1.d 


# Each subdirectory must supply rules for building sources it contributes
ex1/Src/%.o ex1/Src/%.su ex1/Src/%.cyclo: ../ex1/Src/%.c ex1/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/UART/ex1/Inc" -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ex1-2f-Src

clean-ex1-2f-Src:
	-$(RM) ./ex1/Src/Tools.cyclo ./ex1/Src/Tools.d ./ex1/Src/Tools.o ./ex1/Src/Tools.su ./ex1/Src/ex1.cyclo ./ex1/Src/ex1.d ./ex1/Src/ex1.o ./ex1/Src/ex1.su

.PHONY: clean-ex1-2f-Src

