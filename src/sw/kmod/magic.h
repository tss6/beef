#include <linux/dev_printk.h>

#define beef_err(dev, expr, err)                                \
    dev_err((dev), "%s: %s failed: %pe\n",                      \
            __func__, (expr), ERR_PTR(err))

#define run_check(dev, expr, label)                             \
do {                                                            \
    err = (expr);                                               \
    if (unlikely(err < 0)) {                                    \
        beef_err((dev), #expr, err);                            \
        goto label;                                             \
    }                                                           \
} while (0)
