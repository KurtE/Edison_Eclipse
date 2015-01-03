################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Adafruit_GFX.cpp \
../src/Adafruit_ILI9341.cpp \
../src/Adafruit_STMPE610.cpp \
../src/SimpleTransaction.cpp 

OBJS += \
./src/Adafruit_GFX.o \
./src/Adafruit_ILI9341.o \
./src/Adafruit_STMPE610.o \
./src/SimpleTransaction.o 

CPP_DEPS += \
./src/Adafruit_GFX.d \
./src/Adafruit_ILI9341.d \
./src/Adafruit_STMPE610.d \
./src/SimpleTransaction.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	i586-poky-linux-g++ -I"C:\Users\Kurt\Documents\GitHub\Edison_Eclipse\Edison_Incs_Libs\include" -I"C:\Users\Kurt\Documents\GitHub\Edison_Eclipse\ILI9341\src" -I"C:\Users\Kurt\Documents\GitHub\Edison_Eclipse\ArduinoSupport" -I"C:\Users\Kurt\iotdk-ide-win/devkit-x86/sysroots/i586-poky-linux/usr/include/c++" -I"C:\Users\Kurt\iotdk-ide-win/devkit-x86/sysroots/i586-poky-linux/usr/include" -I"C:\Users\Kurt\iotdk-ide-win/devkit-x86/sysroots/i586-poky-linux/usr/include/c++/i586-poky-linux" -Os -g3 -Wall --sysroot="C:\Users\Kurt\iotdk-ide-win\devkit-x86\sysroots\i586-poky-linux" -m32 -march=i586 -c -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


