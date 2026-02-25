################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../one_wire_sensor/one_wire_sensor.cpp 

OBJS += \
./one_wire_sensor/one_wire_sensor.o 

CPP_DEPS += \
./one_wire_sensor/one_wire_sensor.d 


# Each subdirectory must supply rules for building sources it contributes
one_wire_sensor/%.o one_wire_sensor/%.su one_wire_sensor/%.cyclo: ../one_wire_sensor/%.cpp one_wire_sensor/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H7A3xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/usart_wrapper" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/mutex_t" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/gpio" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/real_time_clock" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/spi" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/nor_flash" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/external_eeprom" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/i2c" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/arduino_json" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/WiFi" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/semaphore" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/one_wire_sensor" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/peripherals" -I"E:/ANTZ_Systems/Golf_Cart_Monitoring_Sardar_Patel_Zoological_Park_Kevadia/host_controller_application_firmware/cellular" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-one_wire_sensor

clean-one_wire_sensor:
	-$(RM) ./one_wire_sensor/one_wire_sensor.cyclo ./one_wire_sensor/one_wire_sensor.d ./one_wire_sensor/one_wire_sensor.o ./one_wire_sensor/one_wire_sensor.su

.PHONY: clean-one_wire_sensor

