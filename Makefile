# =================== VARIABLES ======================
CC	:= arm-none-eabi-gcc
LD := arm-none-eabi-ld
CFLAGS := -ffreestanding -pedantic -Wall -Wextra -pedantic -mcpu=cortex-a8 -marm -mapcs-frame -g
LDFLAGS := -N -T linker.ld -nostdlib -static

BUILD_DIR := build
KERNEL_BUILD := ${BUILD_DIR}/kernel
USR_BUILD := ${BUILD_DIR}/usr

KERNEL_OBJS := ${BUILD_DIR}/kernel/kernel.o
TARGET := ${BUILD_DIR}/kernel.elf

# =================== RULES ======================
.PHONY: all kernel usr clean bear run-qemu run-qemu-debug

# BUILDING RULES
all: ${TARGET}

kernel:
	${MAKE} -C kernel BUILD_DIR=../${KERNEL_BUILD}

usr:
	${MAKE} -C usr BUILD_DIR=../${USR_BUILD}

${TARGET}: kernel usr
	${LD} ${LDFLAGS} -o $@  \
		${KERNEL_OBJS} $(shell find ${USR_BUILD} -name "*.o")

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

clean:
	rm -rf ${BUILD_DIR} 

bear:
	bear -- make ${TARGET}

# RUNNING RULES

run-qemu: ${TARGET}
	qemu-system-arm \
		-M versatilepb \
		-cpu cortex-a8 \
		-audio none \
		-nographic \
		-kernel ${TARGET}

run-qemu-debug: ${TARGET}
	qemu-system-arm \
		-M versatilepb \
		-cpu cortex-a8 \
		-audio none \
		-nographic \
		-kernel ${TARGET} \
		-s -S

