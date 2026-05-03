#include "dma.h"
#include "mmio.h"

/* simple enough */
int beef_dma_enable(struct pci_dev *pdev) {
  return dma_set_mask_and_coherent(&pdev->dev, BEEF_DMA_MASK);
}

int beef_dma_alloc(struct pci_dev *pdev)
{
    struct beef_data *data = pci_get_drvdata(pdev);

    /* alloc 1 page, according to pedro */
    data->dma.size = PAGE_SIZE;

    data->dma.cpu_addr = dma_alloc_coherent(&pdev->dev, data->dma.size,
                                            &data->dma.handle, GFP_KERNEL);
    if (!data->dma.cpu_addr)
        return -ENOMEM;

    return 0;
}

/* free the page */
void beef_dma_free(struct pci_dev *pdev)
{
    struct beef_data *data = pci_get_drvdata(pdev);

    if (!data->dma.cpu_addr)
        return;

    dma_free_coherent(&pdev->dev, data->dma.size,
                      data->dma.cpu_addr, data->dma.handle);

    data->dma.cpu_addr = NULL;
}
