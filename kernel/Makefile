# =================== VARIABLES ======================
CC	:= arm-none-eabi-gcc
LD := arm-none-eabi-ld
CFLAGS := -ffreestanding -pedantic -Wall -Wextra -pedantic \
		  -mcpu=cortex-a8 -marm -mapcs-frame -g \
		  -Iinclude/kernel -Iinclude -Iarch/arm/include -I../shared/include/kernel

LDFLAGS := -N -T ../linker/kernel.ld -nostdlib -static

BUILD_DIR ?= build

C_SRCS := $(shell find . -name "*.c")
S_SRCS := $(shell find . -name "*.S")

TARGET := ${BUILD_DIR}/kernel.o
ASM_OBJS := $(patsubst %.S,${BUILD_DIR}/%.o,$(S_SRCS))
C_OBJS := $(patsubst %.c,${BUILD_DIR}/%.o,$(C_SRCS))
OBJS := ${ASM_OBJS} ${C_OBJS}

# =================== RULES ======================
.PHONY: all clean

all: ${TARGET}

# rule for linking .o into kernel.o
${TARGET}: ${OBJS}
	${LD} ${LDFLAGS} -o $@ $^

# rule for C -> .o
${BUILD_DIR}/%.o: %.c
	@mkdir -p $(dir $@)
	${CC} ${CFLAGS} -c -o $@ $<

# rule for .S -> .o
${BUILD_DIR}/%.o: %.S | ${BUILD_DIR}
	@mkdir -p $(dir $@)
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	rm -rf ${BUILD_DIR} 

bear:
	bear -- make ${TARGET}
