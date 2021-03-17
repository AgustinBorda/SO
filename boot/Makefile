# Mac OS
TOOLPREFIX = x86_64-elf-

# Using native tools (e.g., on X86 Linux)
# TOOLPREFIX = 


CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m32 -gdwarf-2 -Wa,-divide
# FreeBSD ld wants ``elf_i386_fbsd''
LDFLAGS += -m $(shell $(LD) -V | grep elf_i386 2>/dev/null | head -n 1)

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

# 1. Compile bootprompt.c and bootasm.S
# 2. Link bootasm.o and bootprompt.o -> bootblock.o
# 3. Extract (objcopy) raw code (.text) section from bootblock ELF into bootblock
# 4. sign.pl: Check size (<510 bytes) and fill with zeroes and put boot magic number
#             at end of 512 boot block
bootblock: bootasm.S bootprompt.c
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -c bootprompt.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c bootasm.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o bootblock.o bootasm.o bootprompt.o
	$(OBJDUMP) -S bootblock.o > bootblock.asm
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock
	./sign.pl bootblock

QEMU = qemu-system-i386

# Run qemu with bootblock as disk drive 0 and 128MB of RAM
QEMUOPTS = -drive file=bootblock,index=0,media=disk,format=raw -m 128

qemu: bootblock
	$(QEMU) -serial mon:stdio $(QEMUOPTS)

qemu-nox: bootblock
	$(QEMU) -nographic -serial mon:stdio $(QEMUOPTS)

clean:
	rm *.o *.d *.asm bootblock
