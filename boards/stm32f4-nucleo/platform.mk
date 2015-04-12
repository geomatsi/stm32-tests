#
# arch: arm
# chip: stm32f401re
# platform: stm32f401re-nucleo
# description:
#

## toolchain

CROSS_COMPILE = arm-none-linux-gnueabi

CC		= $(CROSS_COMPILE)-gcc
LD		= $(CROSS_COMPILE)-ld
OBJCOPY = $(CROSS_COMPILE)-objcopy

## platform identifiers

CHIP		= stm32f4
CHIP_LCM3	= stm32/f4

## platform-specific compile flags

PFLAGS = \
	-mcpu=cortex-m4		\
	-mfloat-abi=softfp	\
	-mthumb				\
	-mthumb-interwork

## projects for stm32f4-nucleo

TARGETS = "leds uart rf24cli rf24hub"

ifeq ($(MAKECMDGOALS), leds)
include $(PRJ_DIR)/boards/$(PLAT)/apps/leds/build.mk
endif

ifeq ($(MAKECMDGOALS), uart)
include $(PRJ_DIR)/boards/$(PLAT)/apps/uart/build.mk
endif

ifeq ($(MAKECMDGOALS), rf24cli)
include $(PRJ_DIR)/boards/$(PLAT)/apps/rf24cli/build.mk
endif

ifeq ($(MAKECMDGOALS), rf24hub)
include $(PRJ_DIR)/boards/$(PLAT)/apps/rf24hub/build.mk
endif

## platform-specific flash rules

upload:
	st-flash erase
	sleep 2
	st-flash write $(OBJ_DIR)/test.bin 0x8000000
