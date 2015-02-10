#
#
#

TARGET=tests

# paths to dependencies

CM3_LIB_DIR = libopencm3

# toolchain

CROSS_COMPILE ?= /home/matsi/devel/tools/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux-2013.05-24/bin/arm-none-linux-gnueabi-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

# project files

INC_DIR = $(TARGET)/include
SRC_DIR = $(TARGET)/src
OBJ_DIR = out

# flags

CFLAGS = -g -O2 -Wall -nostartfiles -nostdlib
CFLAGS += -mcpu=cortex-m4 -mthumb -mthumb-interwork
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp

CFLAGS += -I$(INC_DIR)
CFLAGS += -I$(CM3_LIB_DIR)/include

CFLAGS += -DSTM32F4

LDFLAGS = -T$(TARGET)/stm32f4-test.ld
LIBGCC  = $(shell $(CC) -mthumb -mcpu=cortex-m4 -print-libgcc-file-name)

# project sources

TEST_SRCS += \
	$(SRC_DIR)/main.c


# libopencm3 sources

TEST_SRCS += \
	$(CM3_LIB_DIR)/lib/cm3/vector.c \
	$(CM3_LIB_DIR)/lib/stm32/common/gpio_common_all.c \
	$(CM3_LIB_DIR)/lib/stm32/common/gpio_common_f0234.c \
	$(CM3_LIB_DIR)/lib/stm32/common/rcc_common_all.c \
	$(CM3_LIB_DIR)/lib/stm32/f4/pwr.c \
	$(CM3_LIB_DIR)/lib/stm32/common/flash_common_f234.c \
	$(CM3_LIB_DIR)/lib/stm32/common/pwr_common_all.c \
	$(CM3_LIB_DIR)/lib/cm3/assert.c \
	$(CM3_LIB_DIR)/lib/stm32/f4/rcc.c

#

TEST_OBJS := $(TEST_SRCS:.c=.o)
TEST_OBJS := $(TEST_OBJS:.s=.o)

TEST_OBJS := $(addprefix $(OBJ_DIR)/,$(TEST_OBJS))

# rules

all: $(OBJ_DIR)/$(TARGET).elf $(OBJ_DIR)/$(TARGET).hex $(OBJ_DIR)/$(TARGET).bin

$(OBJ_DIR)/$(TARGET).elf: $(TEST_OBJS) $(TEST_INCS) $(LIBGCC) $(TARGET)/stm32f4-test.ld
	$(LD) $(LDFLAGS) $(TEST_OBJS) $(LIBGCC) -o $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJ_DIR)

.PHONY: clean
