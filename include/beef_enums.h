#ifndef BEEF_ENUMS_H
#define BEEF_ENUMS_H

#ifndef __KERNEL__
#include <stdint.h>
#endif

#define BEEF_HW_NAME   "beef"
#define BEEF_VENDOR_ID 0xbeef
#define BEEF_DEVICE_ID 0x0001

/* mmio regs */
enum beef_regs {
    BEEF_DMA_BASE_ADDR,   /* dma base address                                 */
    BEEF_DMA_REGION_SIZE, /* dma region size                                  */
    BEEF_DMA_BASE_OFFSET, /* offset from base to perform operations           */
    BEEF_DMA_OP_SIZE,     /* size of the dma operation                        */
    BEEF_DMA_BUF_OFFSET,  /* offset from device dma_buf to perform operations */
    BEEF_CMD,
    BEEF_INT,
    BEEF_IRQ_RAISE,       /* writing to this makes the device raise the number written */
    __beef_num_reg
};

enum beef_ops {
    BEEF_READ,
    BEEF_WRITE,
    __beef_num_ops
};

enum beef_irqs {
    BEEF_INT_BUF_AVAIL,
    BEEF_INT_SHOE,
    BEEF_INT_HAT,
    BEEF_INT_BANANA,
    __beef_int_count
};

/* operations implemented in beef's ioctl chrdev */
enum beef_ioctls {
    BEEF_IOCTL_REG_WRITE,
    BEEF_IOCTL_REG_READ,
    __beef_ioctl_count
};

/* what userspace passes/gets from ioctl methods */
struct beef_ioc_reg {
    uint32_t reg;
    uint64_t val;
};

#endif /* BEEF_ENUMS_H */
