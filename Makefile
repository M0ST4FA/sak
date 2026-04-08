CC	:= arm-none-eabi-gcc
LD := arm-none-eabi-ld
CFLAGS := -ffreestanding -pedantic -Wall -Wextra -pedantic -mcpu=cortex-a8 -marm -mapcs-frame -g
LDFLAGS := -N -T linker.ld -nostdlib -static

BUILD_DIR := build

# SRC := $(wildcard *.c)
SRC :=

TARGET := ${BUILD_DIR}/kernel.elf
ASM-OBJS := ${BUILD_DIR}/bootstrap.o ${BUILD_DIR}/context_switch.o ${BUILD_DIR}/syscall.o
C-OBJS := ${BUILD_DIR}/kernel.o ${BUILD_DIR}/lib.o ${BUILD_DIR}/usr.o \
			${BUILD_DIR}/task.o ${BUILD_DIR}/panic.o ${BUILD_DIR}/timer.o \
			${BUILD_DIR}/sysent.o ${BUILD_DIR}/ipc.o
OBJS := ${ASM-OBJS} ${C-OBJS}

.PHONY: all clean run-qemu run-qemu-debug

all: ${TARGET}

# rule for linking .o into kernel.elf
${TARGET}: ${OBJS} ${SRC} | ${BUILD_DIR}
	${LD} ${LDFLAGS} -o $@ $^

# rule for C -> .o
${BUILD_DIR}/%.o: %.c | ${BUILD_DIR}
	${CC} ${CFLAGS} -c -o $@ $<

# rule for assembly -> .o
${BUILD_DIR}/%.o: %.S | ${BUILD_DIR}
	${CC} ${CFLAGS} -c -o $@ $<

# create directory if dones't exist
${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

clean:
	rm -rf ${BUILD_DIR} 

bear:
	bear -- make ${TARGET}

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

