#include <linux/cdev.h>
#include <linux/fs.h>

#include "beef_enums.h"
#include "chrdev.h"
#include "fops.h"


/* from what i could understand, cafe uses xarray to have
 * many minors and track it all. im going to make it simple
 * and do just one... */
#define BEEF_CHRDEV_MINOR_CNT 1

static struct {
    struct class   *class;
    struct cdev     cdev;
    dev_t           devno;
    struct pci_dev *pdev;
} ctrl;


/* called from module_init() or in our case module_pci_driver()
 * sets up the major number and class. */
int beef_chrdev_init(void)
{
    int err;

    /* ask for a major number dynamically, register one minor from 0 (so just 0)
     * dynamic because it uses __register_chrdev_region(0, ...) internally
     * (see: fs/char_dev.c or LDD3 - ch03) */
    if ((err = alloc_chrdev_region(&ctrl.devno, 0, BEEF_CHRDEV_MINOR_CNT, BEEF_HW_NAME)))
        return err;

    /* create /sys/class/beef.
     * from what i understand this is what makes udev create /dev/beef
     * when device_create is called later */
    ctrl.class = class_create(BEEF_HW_NAME);
    if (IS_ERR(ctrl.class)) {
        err = PTR_ERR(ctrl.class);
        goto err_class_create;
    }

    /* setup file operations and register the cdev with the kernel */
    beef_init_fops(&ctrl.cdev);
    if ((err = cdev_add(&ctrl.cdev, ctrl.devno, BEEF_CHRDEV_MINOR_CNT)))
        goto err_cdev_add;

    return 0;

err_cdev_add:
    class_destroy(ctrl.class);

err_class_create:
    unregister_chrdev_region(ctrl.devno, BEEF_CHRDEV_MINOR_CNT);

    return err;
}

void beef_chrdev_deinit(void)
{
    cdev_del(&ctrl.cdev);
    class_destroy(ctrl.class);
    unregister_chrdev_region(ctrl.devno, BEEF_CHRDEV_MINOR_CNT);
}

/* called from probe - creates the actual /dev/beef entry
 * and stores the pdev so fops can find it later via beef_chrdev_pdev() */
int beef_chrdev_create(struct pci_dev *pdev)
{
    struct device *dev;

    /* important! */
    ctrl.pdev = pdev;

    /* actually creates /dev/beef */
    dev = device_create(ctrl.class, &pdev->dev, ctrl.devno, NULL, BEEF_HW_NAME);
    if (IS_ERR(dev)) {
        ctrl.pdev = NULL;
        return PTR_ERR(dev);
    }

    return 0;
}

/* destroys /dev/beef */
void beef_chrdev_destroy(struct pci_dev *pdev)
{
    device_destroy(ctrl.class, ctrl.devno);
    ctrl.pdev = NULL;
}

/* used by fops.c to get the pci_dev from an open file */
struct pci_dev *beef_chrdev_pdev(void)
{
    return ctrl.pdev;
}
