#
# example: FreeRTOS + libopencm3
#

FREERTOS_DIR = $(PRJ_DIR)/../FreeRTOSV8.2.1/FreeRTOS
#FREERTOS_DIR = /home/matsi/code/os/FreeRTOSv10.0.0/FreeRTOS

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp
VPATH += $(PRJ_DIR)/boards/$(PLAT)/apps/freertos-test

VPATH += $(FREERTOS_DIR)/Source
VPATH += $(FREERTOS_DIR)/Source/portable/MemMang
VPATH += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM0

## sources

RTOS_SRCS := \
	printf.c \
	main.c \
	misc.c \
	init.c \
	leds.c \
	uart.c \

RTOS_SRCS += \
	list.c \
	queue.c \
	tasks.c \
	timers.c \
	croutine.c \
	heap_1.c \
	port.c

RTOS_OBJS := $(RTOS_SRCS:.c=.o)
RTOS_OBJS := $(addprefix $(OBJ_DIR)/,$(RTOS_OBJS))

## deps

LIBS = $(LIBCM3)

## flags

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F0

CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC)
CFLAGS	+= -I$(PRJ_DIR)/boards/$(PLAT)/apps/freertos-test
CFLAGS += -I$(FREERTOS_DIR)/Source/include -I$(FREERTOS_DIR)/Source/portable/GCC/ARM_CM0

LDSCRIPT = $(PRJ_DIR)/ld/stm32f030r8-nucleo.ld
LDFLAGS =  $(PFLAGS) -nostartfiles -T$(LDSCRIPT) -Wl,--gc-sections

## rules

freertos-test: $(OBJ_DIR)/freertos-test.bin
	cp $(OBJ_DIR)/freertos-test.bin $(OBJ_DIR)/test.bin
	$(OBJSIZE) $(OBJ_DIR)/freertos-test.elf

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/freertos-test.elf: $(RTOS_OBJS) $(LIBS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(RTOS_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
