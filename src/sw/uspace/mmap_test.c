#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include "../../../include/beef_enums.h"

#define BEEF_DEV      "/dev/beef"
#define BEEF_BAR_SIZE 4096

/* omg i got to use it! */
static volatile uint64_t *bar;

static void reg_write(uint32_t reg, uint64_t val)
{
    bar[reg] = val;
}

static uint64_t reg_read(uint32_t reg)
{
    return bar[reg];
}

int main(void)
{
    int fd = open(BEEF_DEV, O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    bar = mmap(NULL, BEEF_BAR_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (bar == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    uint64_t val;

    printf("reading BEEF_DMA_OP_SIZE...\n");
    val = reg_read(BEEF_DMA_OP_SIZE);
    printf("read: %#lx\n", val);

    printf("writing 0xbeefbeef...\n");
    reg_write(BEEF_DMA_OP_SIZE, 0xbeefbeef);

    printf("reading BEEF_DMA_OP_SIZE...\n");
    val = reg_read(BEEF_DMA_OP_SIZE);
    printf("read: %#lx\n", val);

    reg_write(BEEF_IRQ_RAISE, BEEF_INT_SHOE);
    sleep(1);
    reg_write(BEEF_IRQ_RAISE, BEEF_INT_HAT);
    sleep(1);
    reg_write(BEEF_IRQ_RAISE, BEEF_INT_BANANA);
    sleep(1);

    munmap((void *)bar, BEEF_BAR_SIZE);
    close(fd);
    return 0;
}
