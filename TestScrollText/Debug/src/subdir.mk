################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/iotdk-cpp-template.cpp 

OBJS += \
./src/iotdk-cpp-template.o 

CPP_DEPS += \
./src/iotdk-cpp-template.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	i586-poky-linux-g++ -I/home/todor/Desktop/iotdk-ide-linux/devkit-x86/sysroots/i586-poky-linux/usr/include -I/home/todor/Desktop/iotdk-ide-linux/devkit-x86/sysroots/i586-poky-linux/usr/include/c++ -I/home/todor/Desktop/iotdk-ide-linux/devkit-x86/sysroots/i586-poky-linux/usr/include/c++/i586-poky-linux -Os -g3 -Wall -Os -g --sysroot=/home/todor/Desktop/iotdk-ide-linux/devkit-x86/sysroots/i586-poky-linux -m32 -march=i586 -c -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


