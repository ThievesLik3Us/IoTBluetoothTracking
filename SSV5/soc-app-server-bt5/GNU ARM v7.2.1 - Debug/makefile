################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include gecko_sdk_3.0.0/util/third_party/printf/src/subdir.mk
-include gecko_sdk_3.0.0/util/third_party/printf/subdir.mk
-include gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/subdir.mk
-include gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/subdir.mk
-include gecko_sdk_3.0.0/util/third_party/mbedtls/library/subdir.mk
-include gecko_sdk_3.0.0/util/third_party/crypto/sl_component/se_manager/src/subdir.mk
-include gecko_sdk_3.0.0/util/silicon_labs/silabs_core/memory_manager/subdir.mk
-include gecko_sdk_3.0.0/protocol/bluetooth/src/subdir.mk
-include gecko_sdk_3.0.0/protocol/bluetooth/lib/EFR32BG22/GCC/subdir.mk
-include gecko_sdk_3.0.0/platform/service/system/src/subdir.mk
-include gecko_sdk_3.0.0/platform/service/sleeptimer/src/subdir.mk
-include gecko_sdk_3.0.0/platform/service/power_manager/src/subdir.mk
-include gecko_sdk_3.0.0/platform/service/mpu/src/subdir.mk
-include gecko_sdk_3.0.0/platform/service/iostream/src/subdir.mk
-include gecko_sdk_3.0.0/platform/service/device_init/src/subdir.mk
-include gecko_sdk_3.0.0/platform/radio/rail_lib/plugin/rail_util_pti/subdir.mk
-include gecko_sdk_3.0.0/platform/radio/rail_lib/plugin/pa-conversions/subdir.mk
-include gecko_sdk_3.0.0/platform/emlib/src/subdir.mk
-include gecko_sdk_3.0.0/platform/emdrv/nvm3/src/subdir.mk
-include gecko_sdk_3.0.0/platform/common/toolchain/src/subdir.mk
-include gecko_sdk_3.0.0/platform/common/src/subdir.mk
-include gecko_sdk_3.0.0/platform/bootloader/api/subdir.mk
-include gecko_sdk_3.0.0/platform/Device/SiliconLabs/EFR32BG22/Source/GCC/subdir.mk
-include gecko_sdk_3.0.0/platform/Device/SiliconLabs/EFR32BG22/Source/subdir.mk
-include gecko_sdk_3.0.0/hardware/board/src/subdir.mk
-include gecko_sdk_3.0.0/app/bluetooth/common/ota_dfu/subdir.mk
-include autogen/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: soc-bt5-server.axf

# Tool invocations
soc-bt5-server.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m33 -mthumb -T "C:\Users\Owner\SimplicityStudio\v5_workspace\soc-bt5-server/autogen/linkerfile.ld" -Xlinker --gc-sections -Xlinker -Map="soc-bt5-server.map" -mfpu=fpv5-sp-d16 -mfloat-abi=hard --specs=nano.specs -o soc-bt5-server.axf -Wl,--start-group "./app.o" "./app_properties.o" "./main.o" "./autogen/gatt_db.o" "./autogen/sl_bluetooth.o" "./autogen/sl_board_default_init.o" "./autogen/sl_device_init_clocks.o" "./autogen/sl_event_handler.o" "./autogen/sl_iostream_handles.o" "./autogen/sl_iostream_init_usart_instances.o" "./autogen/sl_power_manager_handler.o" "./gecko_sdk_3.0.0/app/bluetooth/common/ota_dfu/sl_ota_dfu.o" "./gecko_sdk_3.0.0/hardware/board/src/sl_board_control_stub.o" "./gecko_sdk_3.0.0/hardware/board/src/sl_board_init.o" "./gecko_sdk_3.0.0/platform/Device/SiliconLabs/EFR32BG22/Source/GCC/startup_efr32bg22.o" "./gecko_sdk_3.0.0/platform/Device/SiliconLabs/EFR32BG22/Source/system_efr32bg22.o" "./gecko_sdk_3.0.0/platform/bootloader/api/btl_interface.o" "./gecko_sdk_3.0.0/platform/bootloader/api/btl_interface_storage.o" "./gecko_sdk_3.0.0/platform/common/src/sl_slist.o" "./gecko_sdk_3.0.0/platform/common/toolchain/src/sl_memory.o" "./gecko_sdk_3.0.0/platform/emdrv/nvm3/src/nvm3_default_common_linker.o" "./gecko_sdk_3.0.0/platform/emdrv/nvm3/src/nvm3_hal_flash.o" "./gecko_sdk_3.0.0/platform/emdrv/nvm3/src/nvm3_lock.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_assert.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_burtc.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_cmu.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_core.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_emu.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_gpio.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_msc.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_rtcc.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_se.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_system.o" "./gecko_sdk_3.0.0/platform/emlib/src/em_usart.o" "./gecko_sdk_3.0.0/platform/radio/rail_lib/plugin/pa-conversions/pa_conversions_efr32.o" "./gecko_sdk_3.0.0/platform/radio/rail_lib/plugin/pa-conversions/pa_curves_efr32.o" "./gecko_sdk_3.0.0/platform/radio/rail_lib/plugin/rail_util_pti/sl_rail_util_pti.o" "./gecko_sdk_3.0.0/platform/service/device_init/src/sl_device_init_dcdc_s2.o" "./gecko_sdk_3.0.0/platform/service/device_init/src/sl_device_init_emu_s2.o" "./gecko_sdk_3.0.0/platform/service/device_init/src/sl_device_init_hfxo_s2.o" "./gecko_sdk_3.0.0/platform/service/device_init/src/sl_device_init_lfrco.o" "./gecko_sdk_3.0.0/platform/service/iostream/src/sl_iostream.o" "./gecko_sdk_3.0.0/platform/service/iostream/src/sl_iostream_uart.o" "./gecko_sdk_3.0.0/platform/service/iostream/src/sl_iostream_usart.o" "./gecko_sdk_3.0.0/platform/service/mpu/src/sl_mpu.o" "./gecko_sdk_3.0.0/platform/service/power_manager/src/sl_power_manager.o" "./gecko_sdk_3.0.0/platform/service/power_manager/src/sl_power_manager_debug.o" "./gecko_sdk_3.0.0/platform/service/power_manager/src/sl_power_manager_hal_s2.o" "./gecko_sdk_3.0.0/platform/service/sleeptimer/src/sl_sleeptimer.o" "./gecko_sdk_3.0.0/platform/service/sleeptimer/src/sl_sleeptimer_hal_burtc.o" "./gecko_sdk_3.0.0/platform/service/sleeptimer/src/sl_sleeptimer_hal_prortc.o" "./gecko_sdk_3.0.0/platform/service/sleeptimer/src/sl_sleeptimer_hal_rtcc.o" "./gecko_sdk_3.0.0/platform/service/system/src/sl_system_init.o" "./gecko_sdk_3.0.0/platform/service/system/src/sl_system_process_action.o" "./gecko_sdk_3.0.0/protocol/bluetooth/src/sl_bt_mbedtls_context.o" "./gecko_sdk_3.0.0/util/silicon_labs/silabs_core/memory_manager/sl_malloc.o" "./gecko_sdk_3.0.0/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager.o" "./gecko_sdk_3.0.0/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_cipher.o" "./gecko_sdk_3.0.0/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_entropy.o" "./gecko_sdk_3.0.0/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_hash.o" "./gecko_sdk_3.0.0/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_key_derivation.o" "./gecko_sdk_3.0.0/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_key_handling.o" "./gecko_sdk_3.0.0/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_signature.o" "./gecko_sdk_3.0.0/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_util.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/aes.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/bignum.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/ccm.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/cipher.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/cipher_wrap.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/cmac.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/ctr_drbg.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/ecdh.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/ecdsa.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/ecp.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/ecp_curves.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/entropy.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/error.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/platform_util.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/sha1.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/sha256.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/sha512.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/library/threading.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/ba414ep_config.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/ba431_config.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/cryptodma_internal.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/cryptolib_types.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_aes.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_blk_cipher.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_dh_alg.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_ecc_curves.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_ecc_keygen_alg.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_ecdsa_alg.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_hash.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_math.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_memcmp.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_memcpy.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_primitives.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_rng.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc/src/sx_trng.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc_aes.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc_ccm.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc_cmac.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc_ecp.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc_gcm.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc_management.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc_sha.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/cryptoacc_trng.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/entropy_rail.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/radioaes.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/radioaes_aes.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/radioaes_ble.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/shax.o" "./gecko_sdk_3.0.0/util/third_party/mbedtls/sl_crypto/src/sl_mbedtls.o" "./gecko_sdk_3.0.0/util/third_party/printf/printf.o" "./gecko_sdk_3.0.0/util/third_party/printf/src/iostream_printf.o" "C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.0//platform/emdrv/nvm3/lib/libnvm3_CM33_gcc.a" "C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.0//protocol/bluetooth/lib/EFR32BG22/GCC/libbluetooth.a" "C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.0//platform/radio/rail_lib/autogen/librail_release/librail_efr32xg22_gcc_release.a" "C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.0//protocol/bluetooth/lib/EFR32BG22/GCC/binapploader.o" -lgcc -lc -lm -lnosys -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: soc-bt5-server.hex'
	arm-none-eabi-objcopy -O ihex "soc-bt5-server.axf" "soc-bt5-server.hex"
	@echo ' '

	@echo 'Building bin file: soc-bt5-server.bin'
	arm-none-eabi-objcopy -O binary "soc-bt5-server.axf" "soc-bt5-server.bin"
	@echo ' '

	@echo 'Building s37 file: soc-bt5-server.s37'
	arm-none-eabi-objcopy -O srec "soc-bt5-server.axf" "soc-bt5-server.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "soc-bt5-server.axf" -A
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) soc-bt5-server.axf
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
