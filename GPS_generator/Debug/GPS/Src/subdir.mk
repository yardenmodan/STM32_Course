################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../GPS/Src/gps.cpp 

OBJS += \
./GPS/Src/gps.o 

CPP_DEPS += \
./GPS/Src/gps.d 


# Each subdirectory must supply rules for building sources it contributes
GPS/Src/%.o GPS/Src/%.su GPS/Src/%.cyclo: ../GPS/Src/%.cpp GPS/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/GPS_generator/Common" -I"C:/msys64/mingw64/include" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-GPS-2f-Src

clean-GPS-2f-Src:
	-$(RM) ./GPS/Src/gps.cyclo ./GPS/Src/gps.d ./GPS/Src/gps.o ./GPS/Src/gps.su

.PHONY: clean-GPS-2f-Src

