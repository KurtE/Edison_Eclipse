################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BioloidEX.cpp \
../src/CommanderEx.cpp \
../src/PWM.cpp \
../src/Print.cpp \
../src/RoboClaw.cpp \
../src/SDRFunctions.cpp \
../src/Stream.cpp \
../src/Wiring.cpp \
../src/WrapperSerial.cpp \
../src/diyxbee.cpp \
../src/dxl_hal.cpp \
../src/dynamixel.cpp \
../src/msound.cpp \
../src/speak.cpp 

OBJS += \
./src/BioloidEX.o \
./src/CommanderEx.o \
./src/PWM.o \
./src/Print.o \
./src/RoboClaw.o \
./src/SDRFunctions.o \
./src/Stream.o \
./src/Wiring.o \
./src/WrapperSerial.o \
./src/diyxbee.o \
./src/dxl_hal.o \
./src/dynamixel.o \
./src/msound.o \
./src/speak.o 

CPP_DEPS += \
./src/BioloidEX.d \
./src/CommanderEx.d \
./src/PWM.d \
./src/Print.d \
./src/RoboClaw.d \
./src/SDRFunctions.d \
./src/Stream.d \
./src/Wiring.d \
./src/WrapperSerial.d \
./src/diyxbee.d \
./src/dxl_hal.d \
./src/dynamixel.d \
./src/msound.d \
./src/speak.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	i586-poky-linux-g++ -I.. -I"C:\Users\Kurt\iotdk-ide-win\From-Edison\include" -I"C:\Users\Kurt\iotdk-ide-win/devkit-x86/sysroots/i586-poky-linux/usr/include/c++" -I"C:\Users\Kurt\iotdk-ide-win/devkit-x86/sysroots/i586-poky-linux/usr/include" -I"C:\Users\Kurt\iotdk-ide-win/devkit-x86/sysroots/i586-poky-linux/usr/include/c++/i586-poky-linux" -Os -g3 -Wall --sysroot="C:\Users\Kurt\iotdk-ide-win\devkit-x86\sysroots\i586-poky-linux" -m32 -march=i586 -c -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


