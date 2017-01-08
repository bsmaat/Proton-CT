################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/ActionInitialization.cc \
../src/DetectorConstruction.cc \
../src/DetectorHit.cc \
../src/DetectorMessenger.cc \
../src/ModelRunAction.cc \
../src/PrimaryGeneratorAction.cc \
../src/SensitiveDetector.cc 

OBJS += \
./src/ActionInitialization.o \
./src/DetectorConstruction.o \
./src/DetectorHit.o \
./src/DetectorMessenger.o \
./src/ModelRunAction.o \
./src/PrimaryGeneratorAction.o \
./src/SensitiveDetector.o 

CC_DEPS += \
./src/ActionInitialization.d \
./src/DetectorConstruction.d \
./src/DetectorHit.d \
./src/DetectorMessenger.d \
./src/ModelRunAction.d \
./src/PrimaryGeneratorAction.d \
./src/SensitiveDetector.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/billy/geant/geant4.10.2-install/include -I/home/billy/geant/geant4.10.2-install/include/Geant4 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


