#
#
#

ifeq ($(MAKECMDGOALS),nucleo-rf24cli)

# Application custom source paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/apps/rf24cli

# Application dependencies

LIBS = $(LIBCM3) $(LIBNRF24)

# Application custom include paths

CFLAGS += -I$(PRJ_DIR)/include
CFLAGS += -I$(PRJ_DIR)/apps/rf24cli

# Application custom flags

CFLAGS += -mcpu=cortex-m4 -mfloat-abi=softfp
CFLAGS += -mthumb -mthumb-interwork
CFLAGS += -g -O2 -Wall

CFLAGS += -DSTM32F4

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-nucleo.ld

# Application sources

RF24CLI_SRCS := \
	main.c \
	radio.c \
	stdlib.c \
	printf.c \
	clock.c \

RF24CLI_OBJS := $(RF24CLI_SRCS:.c=.o)
RF24CLI_OBJS := $(addprefix $(OBJ_DIR)/,$(RF24CLI_OBJS))

# Custom build rules

nucleo-rf24cli: deps $(OBJ_DIR)/rf24cli.bin
	cp $(OBJ_DIR)/rf24cli.bin $(OBJ_DIR)/test.bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/rf24cli.elf: $(RF24CLI_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f4-nucleo.ld
	$(LD) $(LDFLAGS) $(RF24CLI_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

endif
