#
# example: low power modes
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp
VPATH += $(PRJ_DIR)/boards/$(PLAT)/apps/lpm-rtc-wkup

## sources

APP_SRCS :=	\
	main.c	\
	rtc.c

APP_OBJS := $(APP_SRCS:.c=.o)
APP_OBJS := $(addprefix $(OBJ_DIR)/,$(APP_OBJS))

## deps

LIBS = $(LIBCM3)

## flags

CFLAGS  = $(PFLAGS) -Wall -Werror -Os -DSTM32F0
CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC)

LDSCRIPT = $(PRJ_DIR)/ld/stm32f030x4-mini.ld
LDFLAGS =  $(PFLAGS) -nostartfiles -T$(LDSCRIPT) -Wl,--gc-sections

## rules

lpm-rtc-wkup: $(OBJ_DIR)/lpm-rtc-wkup.bin
	cp $(OBJ_DIR)/lpm-rtc-wkup.bin $(OBJ_DIR)/test.bin
	$(OBJSIZE) $(OBJ_DIR)/lpm-rtc-wkup.elf

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/lpm-rtc-wkup.elf: $(APP_OBJS) $(LIBS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(APP_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
