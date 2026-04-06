Lately, I've been watching lots of LinuxFoundation, LCA and, KernelRecepies
conferences on Youtube. This got me exposed to many ARM and embedded topics.
When Linux started, server was the most important focus. Right now, however,
it appears to be embedded.

I like learning by doing. So, I started chating with ChatGPT to give me some
labs for experimentation.
Eventually, I ended up on https://github.com/singpolyma/singpolyma-kernel.
This project is an adapted version of this kernel.

## Getting started
You need the ARM cross tool chain for building the project.
On Arch Linux, run the following to install it from the AUR using `paru`:
```
paru -S arm-none-eabi-binutils arm-none-eabi-gcc arm-none-eabi-gdb
```
It is present on other distros through their package manager.


Next, clone the repo and run:

```bash
make build/kernel.elf
```

This will build  the kernel and put it under `build/kernel.elf` directory.

To run it, you need `qemu-system-arm` installed. Then run:
```bash
make run-qemu
```

For debugging, you can run:
```bash
make run-qemu-debug
```
This makes qemu open a debugging socket that you can use for remote debugging
via gdb:
```bash
arm-none-eabi-gdb build/kernel.elf

# from the gdb prompt, run this
target remote :1234
```
