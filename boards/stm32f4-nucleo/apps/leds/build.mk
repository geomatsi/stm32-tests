#
#
#

ifeq ($(MAKECMDGOALS),nucleo-leds)

# Application custom source paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/apps/leds

# Application dependencies

LIBS = $(LIBCM3)

# Application custom include paths

CFLAGS += -I$(PRJ_DIR)/include

# Application custom flags

CFLAGS += -mcpu=cortex-m4 -mfloat-abi=softfp
CFLAGS += -mthumb -mthumb-interwork
CFLAGS += -g -O2 -Wall

CFLAGS += -DSTM32F4

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-nucleo.ld

# Application sources

LEDS_SRCS := \
	main_leds.c \
	stdlib.c \
	clock.c \

LEDS_OBJS := $(LEDS_SRCS:.c=.o)
LEDS_OBJS := $(addprefix $(OBJ_DIR)/,$(LEDS_OBJS))

# Custom build rules

nucleo-leds: deps $(OBJ_DIR)/leds.bin
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

endif
