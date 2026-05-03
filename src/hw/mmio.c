#include "mmio.h"
#include "dma.h"
#include "irq.h"

#define BEEF_MMIO_BAR_NUM 0
#define BEEF_MMIO_SIZE 4096
#define BEEF_MMIO_ACCESS_EXP 3
#define BEEF_MMIO_ACCESS_SIZE (1<<BEEF_MMIO_ACCESS_EXP)
#define BEEF_MMIO_NAME "beef_mmio"

static uint64_t beef_mmio_read(void *opaque, hwaddr addr, unsigned size)
{
    uint32_t idx = addr >> BEEF_MMIO_ACCESS_EXP;

    if (idx >= __beef_num_reg) {
        beef_log("invalid mmio read addr %#"PRIx64" idx=%u\n", addr, idx);
        return 0;
    }

    uint64_t data = ((BeefState *)opaque)->reg[idx];
    beef_log("got mmio read reg=%u. has %#"PRIx64"\n", idx, data);

    return data;
}

static void beef_mmio_write(void *opaque, hwaddr addr, uint64_t data, unsigned size)
{
    uint32_t idx = addr >> BEEF_MMIO_ACCESS_EXP;

    if (idx >= __beef_num_reg) {
        beef_log("invalid mmio write addr %#"PRIx64" idx=%u\n", addr, idx);
        return;
    }

    if (idx == BEEF_INT) {
        beef_irq_lower(opaque, data);
        return;
    }

    if (idx == BEEF_IRQ_RAISE) {
        if (data >= __beef_int_count) {
            beef_log("irq_raise: invalid vector %"PRIu64"\n", data);
            return;
        }
        beef_irq_raise(opaque, data);
        return;
    }

    /* if writing to the cmd register, trigger the op requested */
    if (idx == BEEF_CMD){
        switch(data) {
            case BEEF_READ:{
                beef_log("got dma read\n");
                if (beef_dma_read(opaque) == MEMTX_OK)
                    beef_irq_raise(opaque, BEEF_INT_BUF_AVAIL);
            } break;
            case BEEF_WRITE:{
                beef_log("got dma write\n");
                if (beef_dma_write(opaque) == MEMTX_OK)
                    beef_irq_raise(opaque, BEEF_INT_BUF_AVAIL);
            } break;
        }

        /* maybe it would be nice for the driver to know the last ran command
        * that would be done by writing the cmd code into the cmd register,
        * instead of it just triggering the operation and remaining 0 */
        return;
    }

    beef_log("got mmio write reg=%u. wrote %#"PRIx64"\n", idx, data);
    ((BeefState *)opaque)->reg[idx] = data;
}

const MemoryRegionOps beef_mmio_ops = {
    .read = beef_mmio_read,
    .write = beef_mmio_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = BEEF_MMIO_ACCESS_SIZE,
        .max_access_size = BEEF_MMIO_ACCESS_SIZE,
    },
    .impl = {
        .min_access_size = BEEF_MMIO_ACCESS_SIZE,
        .max_access_size = BEEF_MMIO_ACCESS_SIZE,
    },
};

void beef_mmio_init(BeefState *dev, Error **errp)
{
  memory_region_init_io(&dev->mmio, OBJECT(dev), &beef_mmio_ops, dev,
                        BEEF_MMIO_NAME, BEEF_MMIO_SIZE);
  pci_register_bar(&dev->pdev, BEEF_MMIO_BAR_NUM,
                   PCI_BASE_ADDRESS_SPACE_MEMORY, &dev->mmio);

  beef_log("mmio initialized\n");
}
