#
# example: LED blink
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp
VPATH += $(PRJ_DIR)/boards/$(PLAT)/apps/leds

## sources

LEDS_SRCS := \
	main.c \
	stdlib.c \
	clock.c \
	delay.c \

LEDS_OBJS := $(LEDS_SRCS:.c=.o)
LEDS_OBJS := $(addprefix $(OBJ_DIR)/,$(LEDS_OBJS))

## deps

LIBS = $(LIBCM3)

## flags

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F1

CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC)

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f103-mini.ld

## rules

leds: $(OBJ_DIR)/leds.bin
	cp $(OBJ_DIR)/leds.bin $(OBJ_DIR)/test.bin
	$(OBJSIZE) $(OBJ_DIR)/leds.elf

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/leds.elf: $(LEDS_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f103-mini.ld
	$(LD) $(LDFLAGS) $(LEDS_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
