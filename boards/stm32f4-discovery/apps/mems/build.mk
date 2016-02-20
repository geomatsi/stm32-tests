#
# example: MEMS sensor
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/apps/mems

## sources

MEMS_SRCS := \
	main_mems.c \
	stdlib.c \
	printf.c \
	clock.c \
	delay.c \

MEMS_OBJS := $(MEMS_SRCS:.c=.o)
MEMS_OBJS := $(addprefix $(OBJ_DIR)/,$(MEMS_OBJS))

## deps

LIBS = $(LIBCM3) $(LIBSTLINKY)

## flags

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F4

CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC) $(LIBSTLINKY_INC)

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-discovery.ld

## rules

mems: $(OBJ_DIR)/mems.bin
	cp $(OBJ_DIR)/mems.bin $(OBJ_DIR)/test.bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/mems.elf: $(MEMS_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f4-discovery.ld
	$(LD) $(LDFLAGS) $(MEMS_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
