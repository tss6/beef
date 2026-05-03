#ifndef BEEF_DMA_H
#define BEEF_DMA_H

#include <linux/pci.h>

/* 64 bit support */
#define BEEF_DMA_MASK DMA_BIT_MASK(64)

int beef_dma_enable(struct pci_dev *pdev);
int beef_dma_alloc(struct pci_dev *pdev);
void beef_dma_free(struct pci_dev *pdev);

#endif /* BEEF_DMA_H */
