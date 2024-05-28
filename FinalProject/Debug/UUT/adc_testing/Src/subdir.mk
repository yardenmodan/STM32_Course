################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UUT/adc_testing/Src/adc_testing.c 

OBJS += \
./UUT/adc_testing/Src/adc_testing.o 

C_DEPS += \
./UUT/adc_testing/Src/adc_testing.d 


# Each subdirectory must supply rules for building sources it contributes
UUT/adc_testing/Src/%.o UUT/adc_testing/Src/%.su UUT/adc_testing/Src/%.cyclo: ../UUT/adc_testing/Src/%.c UUT/adc_testing/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/FinalProject/UUT/uart_testin/Inc" -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/FinalProject/UUT/i2c_testing/Inc" -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/FinalProject/UUT/adc_testing/Inc" -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/FinalProject/UUT/server/Inc" -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/FinalProject/UUT/spi_testing/Inc" -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/FinalProject/UUT/Testing_Program/Inc" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/BSP/Components/lan8742 -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UUT-2f-adc_testing-2f-Src

clean-UUT-2f-adc_testing-2f-Src:
	-$(RM) ./UUT/adc_testing/Src/adc_testing.cyclo ./UUT/adc_testing/Src/adc_testing.d ./UUT/adc_testing/Src/adc_testing.o ./UUT/adc_testing/Src/adc_testing.su

.PHONY: clean-UUT-2f-adc_testing-2f-Src

