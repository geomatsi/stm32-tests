#
# example: w1
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/drivers/w1
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp
VPATH += $(PRJ_DIR)/boards/$(PLAT)/apps/w1-test

## sources

APP_SRCS := \
	main.c \
	printf.c \
	w1core.c \
	ds18b20.c \

APP_OBJS := $(APP_SRCS:.c=.o)
APP_OBJS := $(addprefix $(OBJ_DIR)/,$(APP_OBJS))

## deps

LIBS = $(LIBCM3)

## flags

CFLAGS  = $(PFLAGS) -Wall -Werror -Os -DSTM32F0
CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC)
CFLAGS += -I$(PRJ_DIR)/boards/$(PLAT)/apps/w1-test

LDSCRIPT = $(PRJ_DIR)/ld/stm32f030r8-nucleo.ld
LDFLAGS =  $(PFLAGS) -nostartfiles -T$(LDSCRIPT) -Wl,--gc-sections

## rules

w1-test: $(OBJ_DIR)/w1-test.bin
	cp $(OBJ_DIR)/w1-test.bin $(OBJ_DIR)/test.bin
	$(OBJSIZE) $(OBJ_DIR)/w1-test.elf

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/w1-test.elf: $(APP_OBJS) $(LIBS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(APP_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
