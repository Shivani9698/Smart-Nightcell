################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include usart_wrapper/subdir.mk
-include spi/subdir.mk
-include sht40_wrapper/subdir.mk
-include semaphore/subdir.mk
-include real_time_clock/subdir.mk
-include radar_sensor/subdir.mk
-include peripherals/subdir.mk
-include nor_flash/subdir.mk
-include mutex_t/subdir.mk
-include lorawan_stm32wl/subdir.mk
-include i2c/subdir.mk
-include gpio/subdir.mk
-include external_eeprom/subdir.mk
-include downlink_management/subdir.mk
-include cellular/subdir.mk
-include adc/subdir.mk
-include WiFi/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Drivers/STM32H7xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := marine_room_automation_control_card
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
marine_room_automation_control_card.elf \

MAP_FILES += \
marine_room_automation_control_card.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
marine_room_automation_control_card.list \

OBJCOPY_HEX += \
marine_room_automation_control_card.hex \

OBJCOPY_BIN += \
marine_room_automation_control_card.bin \


# All Target
all: main-build

# Main-build Target
main-build: marine_room_automation_control_card.elf secondary-outputs

# Tool invocations
marine_room_automation_control_card.elf marine_room_automation_control_card.map: $(OBJS) $(USER_OBJS) D:\ANTZ\ Systems\100_Acre_VIC_Water_Level_Monitoring_Control_Card\control_card_host_controller_application\STM32H7A3RITX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "marine_room_automation_control_card.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"D:\ANTZ Systems\100_Acre_VIC_Water_Level_Monitoring_Control_Card\control_card_host_controller_application\STM32H7A3RITX_FLASH.ld" -Wl,-Map="marine_room_automation_control_card.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

marine_room_automation_control_card.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "marine_room_automation_control_card.list"
	@echo 'Finished building: $@'
	@echo ' '

marine_room_automation_control_card.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "marine_room_automation_control_card.hex"
	@echo 'Finished building: $@'
	@echo ' '

marine_room_automation_control_card.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "marine_room_automation_control_card.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout marine_room_automation_control_card.bin marine_room_automation_control_card.elf marine_room_automation_control_card.hex marine_room_automation_control_card.list marine_room_automation_control_card.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
