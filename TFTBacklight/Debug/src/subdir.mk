################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/iotdk-c-template.c 

OBJS += \
./src/iotdk-c-template.o 

C_DEPS += \
./src/iotdk-c-template.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	i586-poky-linux-gcc -I"/home/todor/Desktop/iotdk-ide-linux/devkit-x86/sysroots/i586-poky-linux/usr/include" -Os -g3 -Wall -Os -g --sysroot=/home/todor/Desktop/iotdk-ide-linux/devkit-x86/sysroots/i586-poky-linux -m32 -march=i586 -c -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


