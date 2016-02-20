#
# example: CDC usb-to-serial
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/apps/cdc

## sources

CDC_SRCS := \
	main_cdc.c \
	stdlib.c \
	printf.c \

CDC_OBJS := $(CDC_SRCS:.c=.o)
CDC_OBJS := $(addprefix $(OBJ_DIR)/,$(CDC_OBJS))

## deps

LIBS = $(LIBCM3)

## flags

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F4

CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC)

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-discovery.ld

## rules

cdc: $(OBJ_DIR)/cdc.bin
	cp $(OBJ_DIR)/cdc.bin $(OBJ_DIR)/test.bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/cdc.elf: $(CDC_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f4-discovery.ld
	$(LD) $(LDFLAGS) $(CDC_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
