obj-m += beef.o
beef-objs += kmod.o mmio.o irq.o dma.o
KDIR    := /lib/modules/6.12.74+deb13+1-cloud-amd64/build
PWD     := $(shell pwd)
INCLUDE := $(shell realpath $(PWD)/../../../include)

all:
	make -j$(shell nproc) -C $(KDIR) M=$(PWD) EXTRA_CFLAGS="-I$(INCLUDE)" modules
	$(MAKE) tidy
clean:
	make -C $(KDIR) M=$(PWD) clean
tidy:
	find . -maxdepth 1 -type f \
		! -name '*.c' \
		! -name '*.h' \
		! -name '*.ko' \
		! -name 'compile_commands.json' \
		! -name 'Makefile' \
		-delete
	rm -v *.mod.c
	rm -rf .cache
