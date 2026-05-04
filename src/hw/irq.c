#include "irq.h"
#include "beef_enums.h"
#include "hw/pci/msix.h"

void beef_irq_init(BeefState *dev, Error **errp)
{
    beef_log("going to initialize msix, int_count=%d\n", __beef_int_count);
    if (msix_init_exclusive_bar(&dev->pdev, __beef_int_count, BEEF_MSIX_BAR_NUM, errp)) {
        beef_log("failed to init msix\n");
        return;
    }

/* from pci/msix.h:
 * PCI spec suggests that devices make it possible for software to configure
 * less vectors than supported by the device, but does not specify a standard
 * mechanism for devices to do so.
 *
 * We support this by asking devices to declare vectors software is going to
 * actually use, and checking this on the notification path. Devices that
 * don't want to follow the spec suggestion can declare all vectors as used. */

    for (int i = 0; i < __beef_int_count; i++)
        msix_vector_use(&dev->pdev, i);

    beef_log("irq initialized\n");
}

void beef_irq_deinit(BeefState *dev)
{
    beef_irq_reset(dev);

    /* have to remove it like before i guess (?)*/
    msix_unuse_all_vectors(&dev->pdev);

    msix_uninit_exclusive_bar(&dev->pdev);
    beef_log("irq deinitialized\n");
}

void beef_irq_raise(BeefState *dev, unsigned int vector)
{
    assert(vector < __beef_int_count);

    /* hm, do i have to check this? */
    if (!msix_enabled(&dev->pdev)){
        beef_log("irq %d raise skipped, not enabled :p\n", vector);
        return;
    }

    dev->irq[vector] = true;
    msix_notify(&dev->pdev, vector);
    beef_log("irq %d raised\n", vector);
}

/* this is implicit so we just keep track */
void beef_irq_lower(BeefState *dev, unsigned int vector)
{
    assert(vector < __beef_int_count);
    dev->irq[vector] = false;
    beef_log("irq %d lowered\n", vector);
}

void beef_irq_reset(BeefState *dev)
{
    msix_reset(&dev->pdev);
    memset(&dev->irq, 0, sizeof dev->irq);
    beef_log("irq_reset: all lowered\n");
}

