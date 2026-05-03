#include "dma.h"

/* the idea is to get the info from the mmio registers. from that, we either
 * read from the RAM to the device dma_buf or write from the dma_buf to RAM.
*
 * we have 2 addresses and 2 offsets:
 * - base    & base_off -> RAM
 * - dma_buf & buf_off  -> dev mem
 *
 * ...and the size of the dma
 *
 * since the config works for both read/write, the function that fetches
 * the register data and checks operation bounds is the same.
 */

struct BeefDmaInfo {
    uint64_t size, ram_addr;
    void *dev_addr;
};

static inline bool beef_dma_get_info(BeefState *dev, struct BeefDmaInfo *x)
{
    /* name too big to write */
    uint64_t size     = dev->reg[BEEF_DMA_OP_SIZE];
    uint64_t base     = dev->reg[BEEF_DMA_BASE_ADDR];
    uint64_t buf_off  = dev->reg[BEEF_DMA_BUF_OFFSET];
    uint64_t base_off = dev->reg[BEEF_DMA_BASE_OFFSET];

    /* bounds checking... no faith */
    if (buf_off > BEEF_DMA_BUFF_SIZE)
        return false;

    if (size > BEEF_DMA_BUFF_SIZE - buf_off)
        return false;

    if (base_off > UINT64_MAX - base)
        return false;

    x->size     = size;
    x->ram_addr = base + base_off;
    x->dev_addr = &dev->dma_buf[buf_off];

    return true;
}

MemTxResult beef_dma_read(BeefState *dev)
{
    struct BeefDmaInfo i;

    if (!beef_dma_get_info(dev, &i))
        return MEMTX_ERROR;

    return pci_dma_read(&dev->pdev, i.ram_addr, i.dev_addr, i.size);
}

MemTxResult beef_dma_write(BeefState *dev)
{
    struct BeefDmaInfo i;

    if (!beef_dma_get_info(dev, &i))
        return MEMTX_ERROR;

    return pci_dma_write(&dev->pdev, i.ram_addr, i.dev_addr, i.size);
}
