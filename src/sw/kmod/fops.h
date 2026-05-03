#ifndef BEEF_FOPS_H
#define BEEF_FOPS_H

#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/fs.h>

void beef_init_fops(struct cdev *cdev);

#endif
