################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../csapp.c \
../main.c \
../mem_allocator.c \
../memlib.c 

OBJS += \
./csapp.o \
./main.o \
./mem_allocator.o \
./memlib.o 

C_DEPS += \
./csapp.d \
./main.d \
./mem_allocator.d \
./memlib.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


