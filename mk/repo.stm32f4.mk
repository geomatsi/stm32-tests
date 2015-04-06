#
#
#

## Platform definitions

ARCH	= arm
CHIP	= stm32f4
PLAT	= stm32f4-nucleo
# libopencm3 platform name
LCM3	= stm32/f4

# project and output directories

OBJ_DIR = $(shell pwd)/out
PRJ_DIR = $(shell pwd)/tests

## default build target: print info

all: info

info:
	@echo "ARCH = $(ARCH)"
	@echo "CHIP = $(CHIP)"
	@echo "PLAT = $(PLAT)"
	@echo "BUILD TARGETS = $(TARGETS)"

## dependencies

# libopencm3

CM3_LIB_DIR		= libopencm3
LIBCM3_INC		= -I$(CM3_LIB_DIR)/include
LIBCM3			= $(CM3_LIB_DIR)/lib/libopencm3_$(CHIP).a

# libnrf24

NRF24_LIB_DIR	= libnrf24
LIBNRF24_INC	= -I$(NRF24_LIB_DIR)/include
LIBNRF24		= $(NRF24_LIB_DIR)/libnrf24_$(CHIP).a

# libstlinky

STLINKY_LIB_DIR	= libstlinky
LIBSTLINKY_INC	= -I$(STLINKY_LIB_DIR)/include
LIBSTLINKY		= $(STLINKY_LIB_DIR)/libstlinky_$(CHIP).a

## platform specific definitions

include $(PRJ_DIR)/boards/$(PLAT)/platform.mk

## build rules for dependencies

deps: libopencm3 libnrf24 libstlinky

libnrf24:
	make -C libnrf24 \
		CROSS_COMPILE=$(CROSS_COMPILE)	\
		TARGET=$(CHIP) \
		PLT_FLAGS="$(PFLAGS)"

libopencm3:
	make -C libopencm3 \
		FP_FLAGS="-mfloat-abi=soft" \
		PREFIX=$(CROSS_COMPILE) \
		TARGETS="$(LCM3)"

libstlinky:
	make -C libstlinky \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		TARGET=$(CHIP) \
		CFG_FLAGS=-DCONFIG_LIB_PRINTF \
		PLT_FLAGS="$(PFLAGS)"

## clean rules

clean:
	rm -rf $(OBJ_DIR)

distclean:
	make -C libopencm3 clean
	make -C libnrf24 clean
	make -C libstlinky clean
	rm -rf $(OBJ_DIR)

.PHONY: libopencm3
.PHONY: libnrf24
.PHONY: libstlinky
.PHONY: distclean
.PHONY: clean
