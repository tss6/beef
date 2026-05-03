#ifndef BEEF_IRQ_H
#define BEEF_IRQ_H

#include <linux/pci.h>
#include "beef_enums.h"

int beef_irq_enable(struct pci_dev *pdev);
void beef_irq_disable(struct pci_dev *pdev);

#endif /* BEEF_IRQ_H */
