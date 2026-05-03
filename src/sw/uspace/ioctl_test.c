#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include "../../../include/beef_enums.h"

#define BEEF_DEV "/dev/beef"

static int fd;

static int reg_write(uint32_t reg, uint64_t val)
{
    struct beef_ioc_reg req = { .reg = reg, .val = val };
    if (ioctl(fd, BEEF_IOCTL_REG_WRITE, &req) < 0) {
        perror("ioctl write");
        return -1;
    }
    return 0;
}

static int reg_read(uint32_t reg, uint64_t *val)
{
    struct beef_ioc_reg req = { .reg = reg, .val = 0 };
    if (ioctl(fd, BEEF_IOCTL_REG_READ, &req) < 0) {
        perror("ioctl read");
        return -1;
    }
    *val = req.val;
    return 0;
}

int main(void)
{
    fd = open(BEEF_DEV, O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    uint64_t val;

    printf("reading BEEF_DMA_OP_SIZE...\n");
    reg_read(BEEF_DMA_OP_SIZE, &val);
    printf("read: %#lx\n", val);

    printf("writing 0xbeefbeef...\n");
    reg_write(BEEF_DMA_OP_SIZE, 0xbeefbeef);

    printf("reading BEEF_DMA_OP_SIZE...\n");
    reg_read(BEEF_DMA_OP_SIZE, &val);
    printf("read: %#lx\n", val);

    reg_write(BEEF_IRQ_RAISE, BEEF_INT_SHOE);
    sleep(1);
    reg_write(BEEF_IRQ_RAISE, BEEF_INT_HAT);
    sleep(1);
    reg_write(BEEF_IRQ_RAISE, BEEF_INT_BANANA);
    sleep(1);

    close(fd);
    return 0;
}
