override KERNEL := kernel.elf
override ISO := limine-cd.iso
override IMG := limine-cd.img
override IMG_RAW := raw.img
override LIMINECFG := limine.cfg
override GDBCFG := debug.gdb

# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.

CC := /usr/bin/cc
LD := /usr/bin/ld
LMN := limine/limine-deploy.exe
BOOTEFI := ./limine/BOOTX64.EFI
ASMC := nasm

# This are specific to my setup, please modify them!!!!
########################################################################
QEMU := "/mnt/c/Program Files/qemu/qemu-system-x86_64.exe"
GDB := "/mnt/c/Users/85562/crossgdb/gdb-12.1/gdb/gdb"
CMDNEWSCREEN := cmd.exe /c start cmd /c wsl -e
MNTDIR := /mnt/bloodmoon

WSLHOSTIP := $(shell ipconfig.exe | grep 'vEthernet (WSL)' -a -A4 | tail -n1 | cut -d":" -f 2 | tail -n1 | sed -e 's/\s*//g')
########################################################################

KERNEL_ENTRY := _start

QFLAGS ?= -cpu qemu64 -machine q35 -m 512 -boot d -cdrom 
QFLAGSEXP ?= -cpu qemu64 -machine q35 -m 512 -boot d -d cpu_reset -drive if=pflash,format=raw,unit=0,file=./OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=./OVMFbin/OVMF_VARS-pure-efi.fd -net none -drive file=

CFLAGS ?= -O2 -g -Wall -Wextra -Wpedantic -pipe -std=c11
NASMFLAGS ?= -F dwarf -g
LDFLAGS ?=
CPPFLAGS ?=
GDBFLAGS ?=  --command=$(GDBCFG)
GDBPORT ?= 1234

ABSDIR := $(shell pwd)
LMNDIR := limine
SRCDIR := src
BUILDHOME := build
BUILDDIR := build/bin
OBJDIR := build/lib
ISOBUILDDIR := build/iso_root
ISODIR := build/image
PROGSDIR := progs/sources
PROGSBUILDDIR := progs/export

PROGS := $(wildcard $(PROGSDIR)/*)

SYSDIR := $(MNTDIR)/system
DATDIR := $(MNTDIR)/data

LMNREPO := https://github.com/limine-bootloader/limine.git
LMNBRCH := v3.0-branch-binary

DIRS := $(wildcard $(SRCDIR)/*)

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Check that given variables are set and all have non-empty values,
# die with an error otherwise.
#
# Params:
#   1. Variable name(s) to test.
#   2. (optional) Error message to print.
check_defined = \
    $(strip $(foreach 1,$1, \
        $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
    $(if $(value $1),, \
      $(error Undefined $1$(if $2, ($2))))

override CFLAGS +=       \
    -I.                  \
    -std=c11             \
    -ffreestanding       \
    -fno-stack-protector \
    -fno-stack-check     \
    -fno-pie             \
    -fno-pic             \
    -m64                 \
    -march=x86-64        \
    -mabi=sysv           \
    -mno-80387           \
    -mno-red-zone        \
	-g                   \
    -mcmodel=kernel      \
    -MMD
override CFLAGS += $(CEXTRA)

override LDFLAGS +=         \
    -nostdlib               \
    -static                 \
    -z max-page-size=0x1000 \
    -T linker.ld

override NASMFLAGS += \
    -f elf64

override CFILES :=$(call rwildcard,$(SRCDIR),*.c)        
override ASFILES := $(call rwildcard,$(SRCDIR),*.S)
override NASMFILES := $(call rwildcard,$(SRCDIR),*.asm)
override OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CFILES))
override OBJS += $(patsubst $(SRCDIR)/%.S, $(OBJDIR)/%_s.o, $(ASFILES))
override OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(NASMFILES))
override HEADER_DEPS := $(CFILES:.c=.d) $(ASFILES:.S=.d)

# You may comment @make run in this target if you intend
# to use virtualbox or real hardware.
all:
	@echo "Building kernel..."
	@make kernel
	@echo "Building ISO..."
	@make buildimg
	@echo "Running QEMU..."
	@make run
	@echo "Done!"

kernel: $(OBJS) link

$(OBJDIR)/io/interrupts.o: $(SRCDIR)/io/interrupts.c
#	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	@$(CC) $(CFLAGS) -mgeneral-regs-only -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
#	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
#	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	@$(ASMC) $^ -felf64 -o $@

$(OBJDIR)/%_s.o: $(SRCDIR)/%.S
#	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	@$(ASMC) $(NASMFLAGS) $^ -f elf64 -o $@

link: 
#	@ echo !==== LINKING $^
	@$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)

clean:
	@rm -rf $(OBJDIR)/*
	@rm -rf $(OBJS)
	@rm -f $(ISOBUILDDIR)/$(KERNEL)
	@rm -f $(ISOBUILDDIR)/$(LIMINECFG)
	@rm -f $(BUILDDIR)/$(KERNEL)
	@rm -f $(ISODIR)/$(ISO)

setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(ISOBUILDDIR)
	@mkdir -p $(ISODIR)
	@dd if=/dev/zero of=$(ISODIR)/$(IMG_RAW) bs=4096 count=65527
	@git clone $(LMNREPO) --branch=$(LMNBRCH) --depth=1
	@cp -v $(LMNDIR)/limine.sys $(LMNDIR)/limine-cd.bin $(LMNDIR)/limine-cd-efi.bin $(ISOBUILDDIR)
	@echo file $(ABSDIR)/$(BUILDDIR)/$(KERNEL) > debug.gdb
	@echo target remote $(WSLHOSTIP):$(GDBPORT) >> debug.gdb
	@echo set disassembly-flavor intel >> debug.gdb
	@echo b $(KERNEL_ENTRY) >> debug.gdb
	@echo c >> debug.gdb

cleansetup:
	@rm -rf $(BUILDHOME)
	@rm -f debug.gdb
	@rm -rf $(LMNDIR)

buildimg:
	@cp -v limine.cfg $(BUILDDIR)/$(KERNEL) $(ISOBUILDDIR)
	@xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-cd-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        $(ISOBUILDDIR) -o $(ISODIR)/$(ISO)
	$(LMN) $(ISODIR)/$(ISO)

buildimgexp:
	@cp $(ISODIR)/$(IMG_RAW) $(ISODIR)/$(IMG)
	@mkfs.fat -F32 $(ISODIR)/$(IMG)
	@mmd -i $(ISODIR)/$(IMG) ::/EFI
	@mmd -i $(ISODIR)/$(IMG) ::/EFI/BOOT
	@mcopy -i $(ISODIR)/$(IMG) $(BOOTEFI) ::/EFI/BOOT
	@mcopy -i $(ISODIR)/$(IMG) ./startup.nsh ::
	@mcopy -i $(ISODIR)/$(IMG) $(BUILDDIR)/$(KERNEL) ::
	@mcopy -i $(ISODIR)/$(IMG) ./limine.cfg ::

buildimggpt:

	$(eval LOOP_DEV_PATH := $(shell losetup -a | grep $(IMG) | cut -d: -f1))
	@echo Loop device path: $(LOOP_DEV_PATH)
	
	@sudo sgdisk --new 1::+100M --typecode 0:ef00 $(LOOP_DEV_PATH)
	@sudo sgdisk --new 2 $(LOOP_DEV_PATH)
	@sudo partprobe $(LOOP_DEV_PATH)
	@sudo mkfs.fat -n PATATA -F32 $(LOOP_DEV_PATH)p1
	@sudo mkfs.fat -n PETETE -F32 $(LOOP_DEV_PATH)p2
	@sudo mkdir $(MNTDIR)
	@sudo mkdir $(SYSDIR)
	@sudo mkdir $(DATDIR)

	@sudo mount $(LOOP_DEV_PATH)p1 $(SYSDIR)
	@sudo mount $(LOOP_DEV_PATH)p2 $(DATDIR)

	@echo Disk mounted, creating system files
	@sudo mkdir -p $(SYSDIR)/efi/boot
	@sudo mkdir -p $(SYSDIR)/test

	@sudo cp $(BOOTEFI) $(SYSDIR)/efi/boot
	@sudo cp ./startup.nsh $(SYSDIR)
	@sudo cp $(BUILDDIR)/$(KERNEL) $(SYSDIR)
	@sudo cp ./limine.cfg $(SYSDIR)
	@sudo cp ./test/test.fake $(SYSDIR)/test

	@echo System files created, creating data files
	@sudo cp -r $(PROGSBUILDDIR) $(DATDIR)
	
	@echo Files created, finishing...
	@sudo umount $(DATDIR)
	@sudo umount $(SYSDIR)
	@sudo rm -rf $(MNTDIR)
	@sudo losetup -d $(LOOP_DEV_PATH)

run:
	$(QEMU) $(QFLAGS) $(ISODIR)/$(ISO)

run_exp:
	$(QEMU) $(QFLAGSEXP)$(ISODIR)/$(IMG)

gpt:
	@make kernel
	@echo "Building GPT..."
#This is required to be before buildimggpt!
	@cp $(ISODIR)/$(IMG_RAW) $(ISODIR)/$(IMG)
	@sudo losetup -f $(ISODIR)/$(IMG)
# Due to eval weird behaviour
	@make buildimggpt
	@echo "Running GPT QEMU..."
	@make run_exp
	@echo "Done!"

exp:
	@make kernel
	@echo "Building experimental ISO..."
	@make buildimgexp
	@echo "Running experimental QEMU..."
	@make run_exp
	@echo "Done!"

progs:
	$(foreach prog,$(PROGS),$(MAKE) -C $(prog); rsync -a $(prog)/build/bin/ $(PROGSBUILDDIR);)


cprogs:
	$(foreach prog,$(PROGS),$(MAKE) -C $(prog) clean;)
	@rm -rf $(PROGSBUILDDIR)/*

.PHONY: progs

debugpt:
	@make kernel
	@echo "Building GPT..."
#This is required to be before buildimggpt!
	@cp $(ISODIR)/$(IMG_RAW) $(ISODIR)/$(IMG)
	@sudo losetup -f $(ISODIR)/$(IMG)
# Due to eval weird behaviour
	@make buildimggpt
	@echo "Running GPT QEMU..."
	$(CMDNEWSCREEN) $(GDB) $(GDBFLAGS) &
	$(QEMU) -S -s $(QFLAGSEXP)$(ISODIR)/$(IMG)

debuge:
	@make kernel
	@echo "Building experimental ISO..."
	@make buildimgexp
	@echo "Running experimental QEMU..."
	$(CMDNEWSCREEN) $(GDB) $(GDBFLAGS) &
	$(QEMU) -S -s $(QFLAGSEXP)$(ISODIR)/$(IMG)

debug:
	@make CEXTRA=-O0 kernel
	@make buildimg
	$(CMDNEWSCREEN) $(GDB) $(GDBFLAGS) &
	$(QEMU) -S -s $(QFLAGS) $(ISODIR)/$(ISO)