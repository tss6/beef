#ifndef BEEF_CHRDEV_H
#define BEEF_CHRDEV_H

#include <linux/pci.h>

int  beef_chrdev_init(void);
void beef_chrdev_deinit(void);
int  beef_chrdev_create(struct pci_dev *pdev);
void beef_chrdev_destroy(struct pci_dev *pdev);
struct pci_dev *beef_chrdev_pdev(void);

#endif
