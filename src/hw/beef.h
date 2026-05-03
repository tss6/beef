#ifndef BEEF_DEVICE_H
#define BEEF_DEVICE_H

#include "qemu/osdep.h"
#include "qemu/log-for-trace.h"
#include "hw/pci/pci_device.h"
#include "qom/object.h"
#include "beef_enums.h"

#define TYPE_BEEF_DEVICE   BEEF_HW_NAME
#define BEEF_DMA_BUFF_SIZE 4096

#define beef_log(...) qemu_log(BEEF_HW_NAME ": " __VA_ARGS__)

/* basic type declaration and cast macros */
OBJECT_DECLARE_TYPE(BeefState, BeefClass, BEEF_DEVICE);

/* device class */
typedef struct BeefClass {
    /* inheriting PCIdev class */
    PCIDeviceClass parent_class;
} BeefClass;

/* state, fields of the object */
typedef struct BeefState {
    /* inherit parent fields */
    PCIDevice pdev;

    /* BAR0 -> control registers */
    MemoryRegion mmio;
    uint64_t reg[__beef_num_reg];

    uint8_t dma_buf[BEEF_DMA_BUFF_SIZE];
    bool irq[__beef_int_count];
} BeefState;

#endif /* BEEF_DEVICE_H */
