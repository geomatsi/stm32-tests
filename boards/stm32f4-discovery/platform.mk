#
# arch: arm
# chip: stm32f4
# platform: stm32f4-discovery
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

## platform dependencies

NRF24_CFG_FLAGS = -DNRF24_LOG_LEVEL=3

LIBCM3_TARGET	= stm32/f4
LIBCM3_FPFLAGS	="-mfloat-abi=soft"

deps: libopencm3 libnrf24 libstlinky

## platform-specific compile flags

PFLAGS = \
	-mcpu=cortex-m4		\
	-mfloat-abi=softfp	\
	-mthumb				\
	-mthumb-interwork

## projects for stm32f4-discovery

TARGETS = "leds mems cdc rf24srv freertos-demo"

ifeq ($(MAKECMDGOALS), leds)
include $(PRJ_DIR)/boards/$(PLAT)/apps/leds/build.mk
endif

ifeq ($(MAKECMDGOALS), mems)
include $(PRJ_DIR)/boards/$(PLAT)/apps/mems/build.mk
endif

ifeq ($(MAKECMDGOALS), cdc)
include $(PRJ_DIR)/boards/$(PLAT)/apps/cdc/build.mk
endif

ifeq ($(MAKECMDGOALS), rf24srv)
include $(PRJ_DIR)/boards/$(PLAT)/apps/rf24srv/build.mk
endif

ifeq ($(MAKECMDGOALS), freertos-demo)
include $(PRJ_DIR)/boards/$(PLAT)/apps/freertos-demo/build.mk
endif

## platform-specific flash rules

upload:
	st-flash erase
	sleep 2
	st-flash write $(OBJ_DIR)/test.bin 0x8000000
