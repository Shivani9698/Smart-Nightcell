################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../usart_wrapper/usart_class.cpp 

OBJS += \
./usart_wrapper/usart_class.o 

CPP_DEPS += \
./usart_wrapper/usart_class.d 


# Each subdirectory must supply rules for building sources it contributes
usart_wrapper/%.o usart_wrapper/%.su usart_wrapper/%.cyclo: ../usart_wrapper/%.cpp usart_wrapper/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H7A3xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/usart_wrapper" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/mutex_t" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/gpio" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/real_time_clock" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/spi" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/nor_flash" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/external_eeprom" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/i2c" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/arduino_json" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/WiFi" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/semaphore" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/peripherals" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/cellular" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/downlink_management" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/adc" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/lorawan_stm32wl" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/radar_sensor" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/utilities" -I"D:/ANTZ Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card/control_card_host_controller_application/sht40_wrapper" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-usart_wrapper

clean-usart_wrapper:
	-$(RM) ./usart_wrapper/usart_class.cyclo ./usart_wrapper/usart_class.d ./usart_wrapper/usart_class.o ./usart_wrapper/usart_class.su

.PHONY: clean-usart_wrapper

