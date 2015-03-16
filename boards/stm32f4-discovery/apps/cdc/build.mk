#
#
#

ifeq ($(MAKECMDGOALS),discovery-cdc-usb)

# Application custom source paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/apps/cdc

# Application dependencies

LIBS = $(LIBCM3)

# Application custom include paths

CFLAGS += -I$(PRJ_DIR)/include

# Application custom flags

CFLAGS += -mcpu=cortex-m4 -mfloat-abi=softfp
CFLAGS += -mthumb -mthumb-interwork
CFLAGS += -g -O2 -Wall

CFLAGS += -DSTM32F4

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-discovery.ld

# Application sources

CDC_SRCS := \
	main_cdc.c \
	stdlib.c \
	printf.c \
	clock.c \

CDC_OBJS := $(CDC_SRCS:.c=.o)
CDC_OBJS := $(addprefix $(OBJ_DIR)/,$(CDC_OBJS))

# Custom build rules

discovery-cdc-usb: deps $(OBJ_DIR)/cdc.bin
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

endif
