#
#
#

ifeq ($(MAKECMDGOALS),nucleo-uart)

# Application custom source paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/apps/uart

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

UART_SRCS := \
	main_uart.c \
	stdlib.c \
	printf.c \
	clock.c \

UART_OBJS := $(UART_SRCS:.c=.o)
UART_OBJS := $(addprefix $(OBJ_DIR)/,$(UART_OBJS))

# Custom build rules

nucleo-uart: deps $(OBJ_DIR)/uart.bin
	cp $(OBJ_DIR)/uart.bin $(OBJ_DIR)/test.bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/uart.elf: $(UART_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f4-nucleo.ld
	$(LD) $(LDFLAGS) $(UART_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

endif
