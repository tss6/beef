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

    /* BAR1 -> table and PBA (pending bit array) for the msix vectors
     * note: no need for region because we use msix_init_exclusive_bar()
     * and it just does it... i tried to alloc myself but i get a error
     * because it is already allocated by something else :p */
    // MemoryRegion msix;
    bool irq[__beef_int_count];

    uint8_t dma_buf[BEEF_DMA_BUFF_SIZE];
} BeefState;

#define BEEF_MMIO_BAR_NUM 0
#define BEEF_MSIX_BAR_NUM 1

#endif /* BEEF_DEVICE_H */
