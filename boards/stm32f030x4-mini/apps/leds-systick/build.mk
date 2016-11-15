#
# example: LED blink
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp
VPATH += $(PRJ_DIR)/boards/$(PLAT)/apps/leds-systick

## sources

LEDS_SRCS := \
	main.c \
	stdlib.c \
	clock.c \

LEDS_OBJS := $(LEDS_SRCS:.c=.o)
LEDS_OBJS := $(addprefix $(OBJ_DIR)/,$(LEDS_OBJS))

## deps

LIBS = $(LIBCM3)

## flags

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F0
CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC)

LDSCRIPT = $(PRJ_DIR)/ld/stm32f030x4-mini.ld
LDFLAGS =  $(PFLAGS) -nostartfiles -T$(LDSCRIPT) -Wl,--gc-sections

## rules

leds-systick: $(OBJ_DIR)/leds-systick.bin
	cp $(OBJ_DIR)/leds-systick.bin $(OBJ_DIR)/test.bin
	$(OBJSIZE) $(OBJ_DIR)/leds-systick.elf

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/leds-systick.elf: $(LEDS_OBJS) $(LIBS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(LEDS_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
