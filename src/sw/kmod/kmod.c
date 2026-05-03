#include "kmod.h"
#include "magic.h"
#include "mmio.h"
#include "dma.h"
#include "irq.h"

/* runs on each matching device found */
static int beef_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    struct device *dev = &pdev->dev;
    struct beef_data *data;
    int err;

    dev_notice(dev, "probing!\n");

    err = ((data = beef_data_alloc()) ? 0 : -ENOMEM);
    run_check(dev, err, err_beef_data_alloc);

    /* same as pdev->dev.driver_data = data (?) */
    pci_set_drvdata(pdev, data);

    /* what does this actually do? ok enable, but in what way? and how? */
    run_check(dev, pci_enable_device(pdev), err_pci_enable_device);

    /* dont forget to set mestre do onibus for dma */
    pci_set_master(pdev);

    run_check(dev, beef_mmio_enable(pdev), err_beef_mmio_enable);
    run_check(dev, beef_irq_enable(pdev), err_beef_irq_enable);
    run_check(dev, beef_dma_enable(pdev), err_beef_dma_enable);
    run_check(dev, beef_dma_alloc(pdev), err_beef_dma_alloc);

    dev_notice(dev, "device probed, ok!\n");
    return 0;

err_beef_dma_alloc:
err_beef_dma_enable:
err_beef_irq_enable:
    beef_irq_disable(pdev);
    beef_mmio_disable(pdev);

err_beef_mmio_enable:
    pci_disable_device(pdev);

err_pci_enable_device:
    beef_data_free(data);
    pci_set_drvdata(pdev, NULL);

err_beef_data_alloc:
    dev_err(dev, "failed to probe beef device\n");
    return err;
}


/* runs on each matching device that got unplugged */
static void beef_remove(struct pci_dev *pdev)
{
    struct device *dev = &pdev->dev;
    beef_irq_disable(pdev);
    beef_dma_free(pdev);
    beef_mmio_disable(pdev);
    pci_disable_device(pdev);
    beef_data_free(pci_get_drvdata(pdev));
    pci_set_drvdata(pdev, NULL);
    dev_notice(dev, "device removed!\n");
}

/* setting what devices the driver supports */
static struct pci_device_id id_tbl[] = {
    { PCI_DEVICE(BEEF_VENDOR_ID, BEEF_DEVICE_ID) },
    {}
};
MODULE_DEVICE_TABLE(pci, id_tbl);

/* register the driver */
static struct pci_driver beef_driver = {
    .name = BEEF_HW_NAME,
    .id_table = id_tbl,
    .probe = beef_probe,
    .remove = beef_remove,
};

/* used when _init and _exit do nothing besides registering, reducing
 * boilerplate. idk if i will use any so i will leave it here as an
 * idea for later...
 *
 * i used it lol */
module_pci_driver(beef_driver);
