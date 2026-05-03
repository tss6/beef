#include "irq.h"
#include "hw/pci/msi.h"

void beef_irq_init(BeefState *dev, Error **errp)
{
    if (msi_init(&dev->pdev, 0, __beef_int_count, true, false, errp))
        beef_log("failed to init msi\n");
    beef_log("irq initialized\n");
}

void beef_irq_deinit(BeefState *dev)
{
    beef_irq_reset(dev);
    msi_uninit(&dev->pdev);
    beef_log("irq deinitialized\n");
}

void beef_irq_raise(BeefState *dev, unsigned int irq_nr)
{
    assert(irq_nr < __beef_int_count);
    dev->irq[irq_nr] = true;
    msi_notify(&dev->pdev, irq_nr);
    beef_log("irq %d raised\n", irq_nr);
}

void beef_irq_lower(BeefState *dev, unsigned int irq_nr)
{
    assert(irq_nr < __beef_int_count);
    dev->irq[irq_nr] = false;
    beef_log("irq %d lowered\n", irq_nr);
}

void beef_irq_reset(BeefState *dev)
{
    memset(&dev->irq, 0, sizeof dev->irq);
    beef_log("irq_reset: all lowered\n");
}

