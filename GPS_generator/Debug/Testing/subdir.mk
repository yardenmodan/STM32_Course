################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Testing/test_main.cpp 

OBJS += \
./Testing/test_main.o 

CPP_DEPS += \
./Testing/test_main.d 


# Each subdirectory must supply rules for building sources it contributes
Testing/%.o Testing/%.su Testing/%.cyclo: ../Testing/%.cpp Testing/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I"C:/Users/yarde/STM32CubeIDE/workspace_1.13.1/GPS_generator/Common" -I"C:/msys64/mingw64/include" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Testing

clean-Testing:
	-$(RM) ./Testing/test_main.cyclo ./Testing/test_main.d ./Testing/test_main.o ./Testing/test_main.su

.PHONY: clean-Testing

