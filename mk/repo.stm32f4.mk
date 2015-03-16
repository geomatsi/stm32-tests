#
#
#

# Platform definitions

TARGET = stm32f4

# Toolchain

CROSS_COMPILE ?= /home/matsi/devel/tools/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux-2013.11-33/bin/arm-none-linux-gnueabi

CC		= $(CROSS_COMPILE)-gcc
LD		= $(CROSS_COMPILE)-ld
OBJCOPY = $(CROSS_COMPILE)-objcopy

# project and output directories

OBJ_DIR = $(shell pwd)/out
PRJ_DIR = $(shell pwd)/tests

# dependencies

CM3_LIB_DIR		= libopencm3
NRF24_LIB_DIR	= libnrf24
STLINKY_LIB_DIR	= libstlinky

# Common include paths

CFLAGS += -I$(CM3_LIB_DIR)/include
CFLAGS += -I$(NRF24_LIB_DIR)/include
CFLAGS += -I$(STLINKY_LIB_DIR)/include

# External libraries

LIBCM3		= $(CM3_LIB_DIR)/lib/libopencm3_$(TARGET).a
LIBNRF24	= $(NRF24_LIB_DIR)/libnrf24_$(TARGET).a
LIBSTLINKY	= $(STLINKY_LIB_DIR)/libstlinky_$(TARGET).a

# build rules for dependencies

deps: libopencm3 libnrf24 libstlinky

libnrf24:
	make -C libnrf24 CROSS_COMPILE=$(CROSS_COMPILE) TARGET=$(TARGET)

libopencm3:
	make -C libopencm3 FP_FLAGS="-mfloat-abi=soft" PREFIX=$(CROSS_COMPILE) TARGETS="stm32/f4"

libstlinky:
	make -C libstlinky CROSS_COMPILE=$(CROSS_COMPILE) TARGET=stm32f4 CONFIG_FLAGS=-DCONFIG_LIB_PRINTF

# include main project rules

include $(PRJ_DIR)/root.mk

# flash rules

flash:
	st-flash erase
	sleep 2
	st-flash write $(OBJ_DIR)/test.bin 0x8000000

# clean rules

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
