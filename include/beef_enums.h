#ifndef BEEF_ENUMS_H
#define BEEF_ENUMS_H

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
    __beef_num_reg
};

enum beef_ops {
    BEEF_READ,
    BEEF_WRITE,
    __beef_num_ops
};

enum beef_irqs {
    BEEF_INT_BUF_AVAIL,
    __beef_int_count
};

#endif /* BEEF_ENUMS_H */
