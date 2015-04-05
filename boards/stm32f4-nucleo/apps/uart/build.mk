#
# example: USART debug output
#

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-nucleo/apps/uart

## sources

UART_SRCS := \
	main_uart.c \
	stdlib.c \
	printf.c \
	clock.c \

UART_OBJS := $(UART_SRCS:.c=.o)
UART_OBJS := $(addprefix $(OBJ_DIR)/,$(UART_OBJS))

## deps

LIBS = $(LIBCM3)

## flags

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F4

CFLAGS += -I$(PRJ_DIR)/include
CFLAGS += $(LIBCM3_INC)

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-nucleo.ld

## rules

uart: $(OBJ_DIR)/uart.bin
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
