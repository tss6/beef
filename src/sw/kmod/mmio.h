#ifndef BEEF_MMIO_H
#define BEEF_MMIO_H

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include "beef_enums.h"

#define BEEF_MMIO_SIZE 4096
#define BEEF_MMIO_BAR  0

struct beef_bar {
    u64 start, end, len;
    void __iomem *mmio;
};

struct beef_dma {
    void *cpu_addr;         /* kernel virtual address */
    dma_addr_t handle;      /* bus/physical address */
    size_t size;
};

struct beef_data {
    struct beef_bar bar;
    struct beef_dma dma;
    struct completion dma_done;
};

struct beef_data *beef_data_alloc(void);
void beef_data_free(struct beef_data *data);
int  beef_mmio_enable(struct pci_dev *pdev);
void beef_mmio_disable(struct pci_dev *pdev);
u64  beef_reg_read(struct beef_data *data, u32 reg);
void beef_reg_write(struct beef_data *data, u32 reg, u64 val);

#endif /* BEEF_MMIO_H */
