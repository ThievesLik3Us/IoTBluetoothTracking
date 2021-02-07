################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../platform/emdrv/dmadrv/src/dmadrv.c 

OBJS += \
./platform/emdrv/dmadrv/src/dmadrv.o 

C_DEPS += \
./platform/emdrv/dmadrv/src/dmadrv.d 


# Each subdirectory must supply rules for building sources it contributes
platform/emdrv/dmadrv/src/dmadrv.o: ../platform/emdrv/dmadrv/src/dmadrv.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DHAL_CONFIG=1' '-D__StackLimit=0x20000000' '-DNVM3_DEFAULT_NVM_SIZE=24576' '-D__HEAP_SIZE=0xD00' '-D__STACK_SIZE=0x800' '-DEFR32BG22C224F512IM40=1' -I"C:\Projects\IoTrack\SSV4\soc-app-client" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\CMSIS\Include" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\nvm3\inc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emlib\src" -I"C:\Projects\IoTrack\SSV4\soc-app-client\hardware\kit\common\bsp" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emlib\inc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\hardware\kit\common\drivers" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\Device\SiliconLabs\EFR32BG22\Include" -I"C:\Projects\IoTrack\SSV4\soc-app-client\protocol\bluetooth\ble_stack\inc\soc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\dmadrv\inc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\uartdrv\inc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\hardware\kit\common\halconfig" -I"C:\Projects\IoTrack\SSV4\soc-app-client\protocol\bluetooth\ble_stack\inc\common" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\service\sleeptimer\src" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\gpiointerrupt\inc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\dmadrv\src" -I"C:\Projects\IoTrack\SSV4\soc-app-client\app\bluetooth\common\util" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\sleep\src" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\radio\rail_lib\protocol\ieee802154" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\halconfig\inc\hal-config" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\nvm3\src" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\common\inc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\Device\SiliconLabs\EFR32BG22\Source\GCC" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\radio\rail_lib\protocol\ble" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\service\sleeptimer\config" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\Device\SiliconLabs\EFR32BG22\Source" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\radio\rail_lib\common" -I"C:\Projects\IoTrack\SSV4\soc-app-client\hardware\kit\EFR32BG22_BRD4184A\config" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\gpiointerrupt\src" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\service\sleeptimer\inc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\radio\rail_lib\chip\efr32\efr32xg2x" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\bootloader\api" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\common\inc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\emdrv\sleep\inc" -I"C:\Projects\IoTrack\SSV4\soc-app-client\platform\bootloader" -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv5-sp-d16 -mfloat-abi=hard -MMD -MP -MF"platform/emdrv/dmadrv/src/dmadrv.d" -MT"platform/emdrv/dmadrv/src/dmadrv.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


