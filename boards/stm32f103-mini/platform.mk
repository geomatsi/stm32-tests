#
# arch: arm cortex-m3
# chip: stm32f103c8t6
# platform: stm32f103-mini
# description: stm32f103x minimal system core board
#

## toolchain

CROSS_COMPILE = arm-none-linux-gnueabi

CC		= $(CROSS_COMPILE)-gcc
LD		= $(CROSS_COMPILE)-ld
OBJCOPY = $(CROSS_COMPILE)-objcopy
OBJSIZE = $(CROSS_COMPILE)-size

## platform identifiers

CHIP = stm32f1

## platform dependencies

LIBCM3_TARGET	= stm32/f1
LIBCM3_FPFLAGS	="-mfloat-abi=soft"

deps: libopencm3

## platform-specific compile flags

PFLAGS = \
	-mcpu=cortex-m3		\
	-mfloat-abi=softfp	\
	-mthumb				\
	-mthumb-interwork

## projects for stm32f4-nucleo

TARGETS = "leds"

ifeq ($(MAKECMDGOALS), leds)
include $(PRJ_DIR)/boards/$(PLAT)/apps/leds/build.mk
endif

## platform-specific flash rules

upload:
	openocd -f $(PRJ_DIR)/boards/$(PLAT)/scripts/openocd.cfg -c 'program ()'

debug:
	openocd -f $(PRJ_DIR)/boards/$(PLAT)/scripts/openocd.cfg -c 'attach ()'
