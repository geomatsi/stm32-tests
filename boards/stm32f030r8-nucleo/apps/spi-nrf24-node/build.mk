#
# example: custom basic printf implementation
#

## paths

VPATH += $(TOP_DIR)/nanopb

VPATH += $(PRJ_DIR)/common
VPATH += $(PRJ_DIR)/boards/$(PLAT)/bsp
VPATH += $(PRJ_DIR)/boards/$(PLAT)/apps/spi-nrf24-node
VPATH += $(PRJ_DIR)/boards/$(PLAT)/apps/spi-nrf24-node/protobuf

## protobuf

PROTOC ?= protoc
PROTOBUF_SRC = $(PRJ_DIR)/boards/$(PLAT)/apps/spi-nrf24-node/protobuf

## sources

APP_SRCS :=		\
	main.c		\
	clock.c		\
	printf.c	\
	stdlib.c	\
	pb_common.c	\
	pb_encode.c	\
	msg.pb.c	\

APP_OBJS := $(APP_SRCS:.c=.o)
APP_OBJS := $(addprefix $(OBJ_DIR)/,$(APP_OBJS))

## deps

LIBS = $(LIBCM3) $(LIBNRF24)

## flags

CFLAGS  = $(PFLAGS) -Wall -Werror -Os -DSTM32F0 -DPB_BUFFER_ONLY
CFLAGS += $(LIBCM3_INC) $(LIBNRF24_INC) $(NANOPB_INC)
CFLAGS += -I$(PRJ_DIR)/include
CFLAGS += -I$(OBJ_DIR)/nanopb

LDSCRIPT = $(PRJ_DIR)/ld/stm32f030r8-nucleo.ld
LDFLAGS =  $(PFLAGS) -nostartfiles -T$(LDSCRIPT) -Wl,--gc-sections

## rules

spi-nrf24-node: $(OBJ_DIR)/spi-nrf24-node.bin
	cp $(OBJ_DIR)/spi-nrf24-node.bin $(OBJ_DIR)/test.bin
	$(OBJSIZE) $(OBJ_DIR)/spi-nrf24-node.elf

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

$(OBJ_DIR)/spi-nrf24-node.elf: $(APP_OBJS) $(LIBS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) $(APP_OBJS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: %.c $(OBJ_DIR)/msg.pb.h
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: %.s $(OBJ_DIR)/msg.pb.h
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.pb.c $(OBJ_DIR)/%.pb.h: $(PROTOBUF_SRC)/%.proto
	mkdir -p $(OBJ_DIR)/nanopb
	$(PROTOC) --proto_path=$(PROTOBUF_SRC) -o $(OBJ_DIR)/$*.pb $(PROTOBUF_SRC)/$*.proto
	python $(TOP_DIR)/nanopb/generator/nanopb_generator.py $(OBJ_DIR)/$*.pb
	# FIXME: why msg.pb.h is removed unless stored in another directory ?
	mv $(OBJ_DIR)/msg.pb.h $(OBJ_DIR)/nanopb
