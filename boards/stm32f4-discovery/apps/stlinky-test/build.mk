#
# example: stlinky terminal
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/apps/stlinky-test

## sources

STTEST_SRCS := \
	main.c \
	stdlib.c \
	printf.c \
	clock.c \
	delay.c \

STTEST_OBJS := $(STTEST_SRCS:.c=.o)
STTEST_OBJS := $(addprefix $(OBJ_DIR)/,$(STTEST_OBJS))

## deps

LIBS = $(LIBCM3) $(LIBSTLINKY)

## flags

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F4

CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC) $(LIBSTLINKY_INC)

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-discovery.ld

## rules

stlinky-test: $(OBJ_DIR)/stlinky-test.bin
	cp $(OBJ_DIR)/stlinky-test.bin $(OBJ_DIR)/test.bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/stlinky-test.elf: $(STTEST_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f4-discovery.ld
	$(LD) $(LDFLAGS) $(STTEST_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
