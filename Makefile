QEMU_SRC    := qemu
QEMU_BUILD  := $(QEMU_SRC)/build
HW_SRC      := src/hw
HW_DST      := $(QEMU_SRC)/hw/beef
KMOD_DIR    := src/sw/kmod
VM_IMAGE    := deb13.qcow2
INCLUDE_DIR := $(abspath include)
NPROC       := $(shell nproc)
TARGET_LIST := x86_64-softmmu

.PHONY: all qemu kmod run clean compile_commands

all: qemu kmod


# QEMU
# idk a good way to make this clone-ready without keeping a fork...
# so we copy it on build time only and patch it

# copy hw source into qemu tree
$(HW_DST): $(wildcard $(HW_SRC)/*)
	cp -r $(HW_SRC) $(HW_DST)


# patch to add device paths
qemu-patch: $(HW_DST)
	grep -q "subdir('beef')" $(QEMU_SRC)/hw/meson.build || echo "subdir('beef')" >> $(QEMU_SRC)/hw/meson.build
	grep -q "source beef/Kconfig" $(QEMU_SRC)/hw/Kconfig || echo "source beef/Kconfig" >> $(QEMU_SRC)/hw/Kconfig
	grep -q "CONFIG_BEEF" $(QEMU_SRC)/configs/devices/x86_64-softmmu.mak || \
		echo "CONFIG_BEEF=y" >> $(QEMU_SRC)/configs/devices/x86_64-softmmu.mak


# configure (only if no build/)
$(QEMU_BUILD)/build.ninja: qemu-patch
	mkdir -p $(QEMU_BUILD)
	cd $(QEMU_BUILD) && ../configure \
		--target-list=$(TARGET_LIST) \
		--enable-debug \
		--extra-cflags="-I$(INCLUDE_DIR)"

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
		-drive file=$(VM_IMAGE),format=qcow2 \
		-device beef \
		-enable-kvm \
		-m 1G \
		-nographic \
		$(QEMU_EXTRA)


# merge compile_commands
compile_commands: qemu kmod
	jq -s '[.[][]]' \
		$(QEMU_BUILD)/compile_commands.json \
		$(KMOD_DIR)/compile_commands.json \
		> compile_commands.json

clean:
	$(MAKE) -C $(KMOD_DIR) clean
	ninja -C $(QEMU_BUILD) clean
	rm -rf $(HW_DST)
	rm -f compile_commands.json
