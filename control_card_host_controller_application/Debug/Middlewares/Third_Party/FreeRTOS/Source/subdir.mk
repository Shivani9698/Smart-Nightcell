################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
../Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
../Middlewares/Third_Party/FreeRTOS/Source/list.c \
../Middlewares/Third_Party/FreeRTOS/Source/queue.c \
../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
../Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
../Middlewares/Third_Party/FreeRTOS/Source/timers.c 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS/Source/croutine.d \
./Middlewares/Third_Party/FreeRTOS/Source/event_groups.d \
./Middlewares/Third_Party/FreeRTOS/Source/list.d \
./Middlewares/Third_Party/FreeRTOS/Source/queue.d \
./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.d \
./Middlewares/Third_Party/FreeRTOS/Source/tasks.d \
./Middlewares/Third_Party/FreeRTOS/Source/timers.d 

OBJS += \
./Middlewares/Third_Party/FreeRTOS/Source/croutine.o \
./Middlewares/Third_Party/FreeRTOS/Source/event_groups.o \
./Middlewares/Third_Party/FreeRTOS/Source/list.o \
./Middlewares/Third_Party/FreeRTOS/Source/queue.o \
./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.o \
./Middlewares/Third_Party/FreeRTOS/Source/tasks.o \
./Middlewares/Third_Party/FreeRTOS/Source/timers.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS/Source/%.o Middlewares/Third_Party/FreeRTOS/Source/%.su Middlewares/Third_Party/FreeRTOS/Source/%.cyclo: ../Middlewares/Third_Party/FreeRTOS/Source/%.c Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H7A3xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/usart_wrapper" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/mutex_t" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/gpio" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/real_time_clock" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/spi" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/nor_flash" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/external_eeprom" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/i2c" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/arduino_json" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/WiFi" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/semaphore" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/peripherals" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/cellular" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/downlink_management" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/adc" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/lorawan_stm32wl" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/radar_sensor" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/utilities" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/sht40_wrapper" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-FreeRTOS-2f-Source

clean-Middlewares-2f-Third_Party-2f-FreeRTOS-2f-Source:
	-$(RM) ./Middlewares/Third_Party/FreeRTOS/Source/croutine.cyclo ./Middlewares/Third_Party/FreeRTOS/Source/croutine.d ./Middlewares/Third_Party/FreeRTOS/Source/croutine.o ./Middlewares/Third_Party/FreeRTOS/Source/croutine.su ./Middlewares/Third_Party/FreeRTOS/Source/event_groups.cyclo ./Middlewares/Third_Party/FreeRTOS/Source/event_groups.d ./Middlewares/Third_Party/FreeRTOS/Source/event_groups.o ./Middlewares/Third_Party/FreeRTOS/Source/event_groups.su ./Middlewares/Third_Party/FreeRTOS/Source/list.cyclo ./Middlewares/Third_Party/FreeRTOS/Source/list.d ./Middlewares/Third_Party/FreeRTOS/Source/list.o ./Middlewares/Third_Party/FreeRTOS/Source/list.su ./Middlewares/Third_Party/FreeRTOS/Source/queue.cyclo ./Middlewares/Third_Party/FreeRTOS/Source/queue.d ./Middlewares/Third_Party/FreeRTOS/Source/queue.o ./Middlewares/Third_Party/FreeRTOS/Source/queue.su ./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.cyclo ./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.d ./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.o ./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.su ./Middlewares/Third_Party/FreeRTOS/Source/tasks.cyclo ./Middlewares/Third_Party/FreeRTOS/Source/tasks.d ./Middlewares/Third_Party/FreeRTOS/Source/tasks.o ./Middlewares/Third_Party/FreeRTOS/Source/tasks.su ./Middlewares/Third_Party/FreeRTOS/Source/timers.cyclo ./Middlewares/Third_Party/FreeRTOS/Source/timers.d ./Middlewares/Third_Party/FreeRTOS/Source/timers.o ./Middlewares/Third_Party/FreeRTOS/Source/timers.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-FreeRTOS-2f-Source

