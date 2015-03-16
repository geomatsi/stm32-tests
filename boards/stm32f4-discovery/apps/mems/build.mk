#
#
#

ifeq ($(MAKECMDGOALS),discovery-mems)

# Application custom source paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/apps/mems

# Application dependencies

LIBS = $(LIBCM3) $(LIBSTLINKY)

# Application custom include paths

CFLAGS += -I$(PRJ_DIR)/include

# Application custom flags

CFLAGS += -mcpu=cortex-m4 -mfloat-abi=softfp
CFLAGS += -mthumb -mthumb-interwork
CFLAGS += -g -O2 -Wall

CFLAGS += -DSTM32F4

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-discovery.ld

# Application sources

MEMS_SRCS := \
	main_mems.c \
	stdlib.c \
	printf.c \
	clock.c \

MEMS_OBJS := $(MEMS_SRCS:.c=.o)
MEMS_OBJS := $(addprefix $(OBJ_DIR)/,$(MEMS_OBJS))

# Custom build rules

discovery-mems: deps $(OBJ_DIR)/mems.bin
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

endif
