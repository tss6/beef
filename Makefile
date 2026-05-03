QEMU_SRC    := qemu
QEMU_BUILD  := $(QEMU_SRC)/build
HW_SRC      := src/hw
HW_DST      := $(QEMU_SRC)/hw/beef
KMOD_DIR    := src/sw/kmod
VM_IMAGE    := deb13.qcow2
ROOT        := $(abspath .)
INCLUDE_DIR := $(ROOT)/include
NPROC       := $(shell nproc)
TARGET_LIST := x86_64-softmmu

.PHONY: all qemu kmod run clean compile_commands

all: qemu kmod


# QEMU
# idk a good way to make this clone-ready without keeping a fork...
# so we copy it on build time only and patch it

# copy hw source into qemu tree
$(HW_DST): $(wildcard $(HW_SRC)/*)
	rsync -a --delete $(HW_SRC)/ $(HW_DST)/


# patch to add device paths
.qemu-patch: $(HW_DST)
	grep -q "subdir('beef')" $(QEMU_SRC)/hw/meson.build || echo "subdir('beef')" >> $(QEMU_SRC)/hw/meson.build
	grep -q "source beef/Kconfig" $(QEMU_SRC)/hw/Kconfig || echo "source beef/Kconfig" >> $(QEMU_SRC)/hw/Kconfig
	grep -q "CONFIG_BEEF" $(QEMU_SRC)/configs/devices/x86_64-softmmu.mak || \
		echo "CONFIG_BEEF=y" >> $(QEMU_SRC)/configs/devices/x86_64-softmmu.mak
	touch $@


# configure (only if no build/)
$(QEMU_BUILD)/build.ninja: .qemu-patch
	@if [ ! -f $@ ]; then \
		mkdir -p $(QEMU_BUILD); \
		cd $(QEMU_BUILD) && ../configure \
			--target-list=$(TARGET_LIST) \
			--enable-debug \
			--extra-cflags="-I$(INCLUDE_DIR)"; \
	fi

# build
qemu: $(QEMU_BUILD)/build.ninja
	ninja -C $(QEMU_BUILD) -j$(NPROC)


# kmod

# use makefile there :p
kmod:
	$(MAKE) -C $(KMOD_DIR) EXTRA_CFLAGS="-I$(INCLUDE_DIR)"


# VM

run: qemu
	$(QEMU_BUILD)/qemu-system-x86_64 \
		-M q35 \
		-accel kvm \
		-smp $(NPROC) \
		-m 2G \
		-boot c \
		-drive file=$(VM_IMAGE),format=qcow2,if=virtio,cache=writeback \
		-netdev user,id=net0,hostfwd=tcp::2222-:22 \
		-device virtio-net-pci,netdev=net0 \
		-device beef \
		-nographic \
		-serial mon:stdio \
		-D qemu-debug.log


# merge compile_commands
compile_commands: qemu kmod
	jq -s '[.[][]]' \
		$(QEMU_BUILD)/compile_commands.json \
		$(KMOD_DIR)/compile_commands.json \
	| jq 'map(if .file | startswith("../hw/beef/") then .file |= sub("../hw/beef/"; "$(ROOT)/src/hw/") else . end)' \
	  > compile_commands.json

clean:
	$(MAKE) -C $(KMOD_DIR) clean
	ninja -C $(QEMU_BUILD) clean
	rm -rf $(HW_DST)
	rm -f compile_commands.json
