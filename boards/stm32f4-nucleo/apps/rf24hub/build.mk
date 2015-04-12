#
# example: nrf24 sensor hub
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/apps/rf24hub

## sources

RF24HUB_SRCS := \
	main.c \
	radio.c \
	stdlib.c \
	printf.c \
	clock.c \

RF24HUB_OBJS := $(RF24HUB_SRCS:.c=.o)
RF24HUB_OBJS := $(addprefix $(OBJ_DIR)/,$(RF24HUB_OBJS))

## deps

LIBS = $(LIBCM3) $(LIBNRF24)

## flags

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F4

CFLAGS += -I$(PRJ_DIR)/include
CFLAGS += -I$(PRJ_DIR)/apps/rf24hub
CFLAGS += $(LIBCM3_INC) $(LIBNRF24_INC)

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-nucleo.ld

# Custom build rules

rf24hub: $(OBJ_DIR)/rf24hub.bin
	cp $(OBJ_DIR)/rf24hub.bin $(OBJ_DIR)/test.bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/rf24hub.elf: $(RF24HUB_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f4-nucleo.ld
	$(LD) $(LDFLAGS) $(RF24HUB_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
