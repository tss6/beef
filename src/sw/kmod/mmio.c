#include "mmio.h"
#include "magic.h"

struct beef_data *beef_data_alloc(void)
{
    /* zero it out */
    struct beef_data *ptr = kzalloc(sizeof(struct beef_data), GFP_KERNEL);
    if (!ptr) return NULL;

    init_completion(&ptr->dma_done);

    return ptr;
}

void beef_data_free(struct beef_data *data) { kfree(data); }

int beef_mmio_enable(struct pci_dev *pdev)
{
    struct beef_data *data = pci_get_drvdata(pdev);
    struct device *dev = &pdev->dev;
    int err;

    /* get ownership of a PCI bar region so nobody else uses it */
    run_check(dev, pci_request_region(pdev, BEEF_MMIO_BAR, BEEF_HW_NAME),
        err_pci_request_region);

    /* map bar region into (virtual kernel) memory */
    if (!(data->bar.mmio = pci_iomap(pdev, BEEF_MMIO_BAR, BEEF_MMIO_SIZE))) {
        err = -ENOMEM;
        dev_err(dev, "pci_iomap() returned NULL\n");
        goto err_pci_iomap;
    }

    /*  now, if i undestand correctly:
     *  *(u64*)data->bar.mmio[i] == *BeefState->reg[i]
     */

    data->bar.start = pci_resource_start(pdev, BEEF_MMIO_BAR);
    data->bar.end   = pci_resource_end  (pdev, BEEF_MMIO_BAR);
    data->bar.len   = pci_resource_len  (pdev, BEEF_MMIO_BAR);
    return 0;

err_pci_iomap:
    pci_release_region(pdev, BEEF_MMIO_BAR);

err_pci_request_region:

    return err;
}

void beef_mmio_disable(struct pci_dev *pdev)
{
    struct beef_data *data = pci_get_drvdata(pdev);
    pci_iounmap(pdev, data->bar.mmio);
    /* always nice */
    data->bar.mmio = NULL;
    data->bar.start = data->bar.end = data->bar.len = 0;
    pci_release_region(pdev, BEEF_MMIO_BAR);
}

/* since each device register is 64bits... if i change that
 * in qemu i need to remember to change it here too */
inline u64 beef_reg_read(struct beef_data *data, u32 reg)
{
    return readq(data->bar.mmio + reg * sizeof(u64));
}

inline void beef_reg_write(struct beef_data *data, u32 reg, u64 val)
{
    writeq(val, data->bar.mmio + reg * sizeof(u64));
}
