#include "magic.h"
#include "mmio.h"
#include "irq.h"

static irqreturn_t beef_handler_buf_avail(int irq, void *data)
{
    struct pci_dev *pdev = (struct pci_dev *)data;
    struct beef_data *dev_data = pci_get_drvdata(pdev);

    dev_notice(&pdev->dev, "got buf_avail interrupt!\n");

    complete(&dev_data->dma_done);

    /* write to the device register anouncing the operation has succeeded */
    beef_reg_write(dev_data, BEEF_INT, BEEF_INT_BUF_AVAIL);

    return IRQ_HANDLED;
}

static irqreturn_t beef_handler_shoe(int irq, void *data)
{
    struct pci_dev *pdev = (struct pci_dev *)data;
    dev_notice(&pdev->dev, "shoe!\n");
    beef_reg_write(pci_get_drvdata(pdev), BEEF_INT, BEEF_INT_SHOE);
    return IRQ_HANDLED;
}

static irqreturn_t beef_handler_hat(int irq, void *data)
{
    struct pci_dev *pdev = (struct pci_dev *)data;
    dev_notice(&pdev->dev, "hat!\n");
    beef_reg_write(pci_get_drvdata(pdev), BEEF_INT, BEEF_INT_HAT);
    return IRQ_HANDLED;
}

static irqreturn_t beef_handler_banana(int irq, void *data)
{
    struct pci_dev *pdev = (struct pci_dev *)data;
    dev_notice(&pdev->dev, "banana!\n");
    beef_reg_write(pci_get_drvdata(pdev), BEEF_INT, BEEF_INT_BANANA);
    return IRQ_HANDLED;
}

static irqreturn_t (*const handlers[__beef_int_count])(int, void*) = {
    beef_handler_buf_avail,
    beef_handler_shoe,
    beef_handler_hat,
    beef_handler_banana,
};

static void beef_irq_free(struct pci_dev *pdev, int i)
{
    int irqn;
    while (--i >= 0)
         if ((irqn = pci_irq_vector(pdev, i)) >= 0)
            free_irq(irqn, pdev);
}

void beef_irq_disable(struct pci_dev *pdev)
{
    beef_irq_free(pdev, __beef_int_count);

    pci_free_irq_vectors(pdev);
}

int beef_irq_enable(struct pci_dev *pdev)
{
    struct device *dev = &pdev->dev;
    int err, i = 0;

    run_check(dev,
        pci_alloc_irq_vectors(pdev, __beef_int_count, __beef_int_count, PCI_IRQ_MSI),
        err_pci_alloc_irq_vectors
    );

    /* register the handlers for each interrupt vector */
    for (; i < __beef_int_count; i++) {
        int irqn;

        run_check(dev, irqn = pci_irq_vector(pdev, i), err_pci_irq_vector);

        run_check(dev,
            request_irq(irqn, handlers[i], i, BEEF_HW_NAME, pdev),
            err_request_irq
        );
    }

    return 0;

err_request_irq:
    beef_irq_free(pdev, i);

err_pci_irq_vector:
    pci_free_irq_vectors(pdev);

err_pci_alloc_irq_vectors:
    return err;
}

