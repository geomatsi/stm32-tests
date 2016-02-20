#
# example: LED blink
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/apps/leds

## sources

LEDS_SRCS := \
	main_leds.c \
	stdlib.c \
	clock.c \
	delay.c \

LEDS_OBJS := $(LEDS_SRCS:.c=.o)
LEDS_OBJS := $(addprefix $(OBJ_DIR)/,$(LEDS_OBJS))

## deps

LIBS = $(LIBCM3)

## flags

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F4

CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC)

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-nucleo.ld

## rules

leds: $(OBJ_DIR)/leds.bin
	cp $(OBJ_DIR)/leds.bin $(OBJ_DIR)/test.bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/leds.elf: $(LEDS_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f4-nucleo.ld
	$(LD) $(LDFLAGS) $(LEDS_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
