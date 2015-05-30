#
# example: FreeRTOS + libopencm3
#

FREERTOS_DIR = $(PRJ_DIR)/../FreeRTOSV7.3.0/FreeRTOS

## paths

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/bsp
VPATH += $(PRJ_DIR)/boards/stm32f4-discovery/apps/freertos-demo

VPATH += $(FREERTOS_DIR)/Source
VPATH += $(FREERTOS_DIR)/Source/portable/MemMang
VPATH += $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F

## sources

RTOS_SRCS := \
	main.c \
	stdlib.c \

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

CFLAGS  = $(PFLAGS) -Wall -O2 -DSTM32F4

CFLAGS += -I$(PRJ_DIR)/include $(LIBCM3_INC)
CFLAGS	+= -I$(PRJ_DIR)/boards/$(PLAT)/apps/freertos-demo
CFLAGS += -I$(FREERTOS_DIR)/Source/include -I$(FREERTOS_DIR)/Source/portable/GCC/ARM_CM4F

LDFLAGS = -T$(PRJ_DIR)/ld/stm32f4-discovery.ld

## rules

freertos-demo: $(OBJ_DIR)/freertos-demo.bin
	cp $(OBJ_DIR)/freertos-demo.bin $(OBJ_DIR)/test.bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/freertos-demo.elf: $(RTOS_OBJS) $(LIBS) $(PRJ_DIR)/ld/stm32f4-discovery.ld
	$(LD) $(LDFLAGS) $(RTOS_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $^
