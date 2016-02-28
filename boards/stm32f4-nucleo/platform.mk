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

## platform dependencies

NRF24_CFG_FLAGS = -DNRF24_LOG_LEVEL=3

LIBCM3_TARGET	= stm32/f4
LIBCM3_FPFLAGS	="-mfloat-abi=soft"

deps: libopencm3 libnrf24 nanopb

## platform-specific compile flags

PFLAGS = \
	-mcpu=cortex-m4		\
	-mfloat-abi=softfp	\
	-mthumb				\
	-mthumb-interwork

## projects for stm32f4-nucleo

TARGETS = "leds uart rf24cli rf24hub freertos-demo freertos-nrf24hub"

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

ifeq ($(MAKECMDGOALS), freertos-demo)
include $(PRJ_DIR)/boards/$(PLAT)/apps/freertos-demo/build.mk
endif

ifeq ($(MAKECMDGOALS), freertos-nrf24hub)
include $(PRJ_DIR)/boards/$(PLAT)/apps/freertos-nrf24hub/build.mk
endif

## platform-specific flash rules

upload:
	openocd -f $(PRJ_DIR)/boards/$(PLAT)/scripts/openocd-stlink.cfg -c 'program ()'

debug:
	openocd -f $(PRJ_DIR)/boards/$(PLAT)/scripts/openocd-stlink.cfg -c 'attach ()'
