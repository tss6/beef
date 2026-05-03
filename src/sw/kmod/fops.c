#include "fops.h"
#include "chrdev.h"
#include "mmio.h"
#include "beef_enums.h"

/* TODO: see stuff about _IOW() and magic, maybe better */

/* read it, then copy from kernel to user mem */
static long beef_ioctl_read(struct beef_ioc_reg __user *reg, struct beef_data *data)
{
    struct beef_ioc_reg req;
    u64 val;

    /* copy to kernel space */
    if (copy_from_user(&req, reg, sizeof(req)))
        return -EFAULT;

    /* bounds check */
    if (req.reg >= __beef_num_reg)
        return -EINVAL;

    /* read the requested register */
    val = beef_reg_read(data, req.reg);

    /* copy the value to the user space mem */
    if (copy_to_user(&reg->val, &val, sizeof(u64)))
        return -EFAULT;

    return 0;
}


/* copy from user to kernel mem, then write to the reg */
static long beef_ioctl_write(struct beef_ioc_reg __user *reg, struct beef_data *data)
{
    struct beef_ioc_reg req;

    /* copy to kernel space */
    if (copy_from_user(&req, reg, sizeof(req)))
        return -EFAULT;

    /* bounds check */
    if (req.reg >= __beef_num_reg)
        return -EINVAL;

    /* write to the mmio region */
    beef_reg_write(data, req.reg, req.val);
    return 0;
}

static long beef_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    struct pci_dev *pdev;
    struct beef_data *data;
    struct beef_ioc_reg __user *reg;

    if (cmd >= __beef_ioctl_count)
        return -EINVAL;

    if (!(pdev = beef_chrdev_pdev()))
        return -ENODEV;

    data = pci_get_drvdata(pdev);

    reg = (void __user *)arg;

    switch (cmd) {
        case BEEF_IOCTL_REG_READ:  return beef_ioctl_read(reg, data);
        case BEEF_IOCTL_REG_WRITE: return beef_ioctl_write(reg, data);
        default:                   return -ENOTTY; /* funny */
    }
}

static int beef_mmap(struct file *f, struct vm_area_struct *vma)
{
    struct pci_dev *pdev;
    struct beef_data *data;
    unsigned long len;
    int err;

    if (!(pdev = beef_chrdev_pdev()))
        return -ENODEV;

    data = pci_get_drvdata(pdev);

    /* bounds check to see if bar fits in end - start */
    if ((len = vma->vm_end - vma->vm_start) > data->bar.len) {
        dev_err(&pdev->dev, "mmap request out of bounds\n");
        return -EINVAL;
    }

    /* maps the BAR physical address range into userspace vma */
    if ((err = vm_iomap_memory(vma, data->bar.start, len))) {
        dev_err(&pdev->dev, "vm_iomap_memory() failed: %pe\n", ERR_PTR(err));
        return err;
    }

    return 0;
}

static const struct file_operations beef_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = beef_ioctl,
    .mmap           = beef_mmap,
};

void beef_init_fops(struct cdev *cdev)
{
    cdev_init(cdev, &beef_fops);
}
