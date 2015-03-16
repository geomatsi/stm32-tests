#
#
#

ifeq ($(MAKECMDGOALS),discovery-rf24srv)

# Application custom source paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/apps/rf24srv

# Application dependencies

LIBS = $(LIBCM3) $(LIBNRF24)

# Application custom include paths

CFLAGS += -I$(PRJ_DIR)/include
CFLAGS += -I$(PRJ_DIR)/apps/rf24srv

# Application custom flags

CFLAGS += -mcpu=cortex-m4 -mfloat-abi=softfp
CFLAGS += -mthumb -mthumb-interwork
CFLAGS += -g -O2 -Wall

CFLAGS += -DSTM32F4

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-discovery.ld

# Application sources

RF24SRV_SRCS := \
	main.c \
	radio.c \
	stdlib.c \
	printf.c \
	clock.c \

RF24SRV_OBJS := $(RF24SRV_SRCS:.c=.o)
RF24SRV_OBJS := $(addprefix $(OBJ_DIR)/,$(RF24SRV_OBJS))

# Custom build rules

discovery-rf24srv: deps $(OBJ_DIR)/rf24srv.bin
	cp $(OBJ_DIR)/rf24srv.bin $(OBJ_DIR)/test.bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/rf24srv.elf: $(RF24SRV_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f4-discovery.ld
	$(LD) $(LDFLAGS) $(RF24SRV_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

endif
