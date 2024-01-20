################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ex_uart/Src/Tools.c \
../ex_uart/Src/ex_uart.c 

OBJS += \
./ex_uart/Src/Tools.o \
./ex_uart/Src/ex_uart.o 

C_DEPS += \
./ex_uart/Src/Tools.d \
./ex_uart/Src/ex_uart.d 


# Each subdirectory must supply rules for building sources it contributes
ex_uart/Src/%.o ex_uart/Src/%.su ex_uart/Src/%.cyclo: ../ex_uart/Src/%.c ex_uart/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/UART/ex_uart/Inc" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ex_uart-2f-Src

clean-ex_uart-2f-Src:
	-$(RM) ./ex_uart/Src/Tools.cyclo ./ex_uart/Src/Tools.d ./ex_uart/Src/Tools.o ./ex_uart/Src/Tools.su ./ex_uart/Src/ex_uart.cyclo ./ex_uart/Src/ex_uart.d ./ex_uart/Src/ex_uart.o ./ex_uart/Src/ex_uart.su

.PHONY: clean-ex_uart-2f-Src

