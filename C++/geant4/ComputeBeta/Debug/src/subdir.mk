################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/ActionInitialization.cc \
../src/DetectorConstruction.cc \
../src/DetectorHit.cc \
../src/ExN02PhysicsList.cc \
../src/ModelRunAction.cc \
../src/PrimaryGeneratorAction.cc \
../src/SteppingAction.cc \
../src/TrackerSD.cc 

OBJS += \
./src/ActionInitialization.o \
./src/DetectorConstruction.o \
./src/DetectorHit.o \
./src/ExN02PhysicsList.o \
./src/ModelRunAction.o \
./src/PrimaryGeneratorAction.o \
./src/SteppingAction.o \
./src/TrackerSD.o 

CC_DEPS += \
./src/ActionInitialization.d \
./src/DetectorConstruction.d \
./src/DetectorHit.d \
./src/ExN02PhysicsList.d \
./src/ModelRunAction.d \
./src/PrimaryGeneratorAction.d \
./src/SteppingAction.d \
./src/TrackerSD.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/billy/geant/geant4.10.2-install/include/Geant4 -I/home/billy/geant/geant4.10.2-install/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


