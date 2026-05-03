#include "beef.h"
#include "mmio.h"
#include "irq.h"

/* sets initial state, once realization is done the device is "active".
 *
 * there is also _init that is called before this and cannot fail (does not
 * return a error and should not abort or something like that). it should be
 * used to create stuff that is required before realization. since we dont need
 * anything, we dont set it.
 *
 * devices can be instantiated (and also finalized) without being realized
 *
 * see: https://people.redhat.com/~thuth/blog/qemu/2018/09/10/instance-init-realize.html */
static void beef_realize(PCIDevice *pci_dev, Error **errp)
{
    BeefState *dev = BEEF_DEVICE(pci_dev);

    /* set pci express capability on pci configuration space
     * first check if the bus is express and then set the capability */
    if (pci_bus_is_express(pci_get_bus(pci_dev)) &&
      pcie_endpoint_cap_init(pci_dev, 0) >= 0)
        beef_log("express capability set\n");

    beef_mmio_init(dev, errp);
    beef_irq_init (dev, errp);
    beef_irq_reset(dev);

    beef_log("device realized\n");
}

/* frees used memory regions and dynamically allocated spaces */
static void beef_exit(PCIDevice *pci_dev)
{
    BeefState *dev = BEEF_DEVICE(pci_dev);
    beef_irq_deinit(dev);
    object_unparent(OBJECT(&dev->mmio));
}

/* initializes class data */
static void beef_class_init(ObjectClass *oc, const void *data)
{
    struct DeviceClass *dc = DEVICE_CLASS(oc);
    struct PCIDeviceClass *pci_dc = PCI_DEVICE_CLASS(oc);

    /* set the realize function to be used in the instances */
    pci_dc->realize = beef_realize;

    /* set the function to be used when "unregistering" */
    pci_dc->exit = beef_exit;

    pci_dc->vendor_id = BEEF_VENDOR_ID;
    pci_dc->device_id = BEEF_DEVICE_ID;

    /* set the PCI class id */
    pci_dc->class_id = PCI_CLASS_OTHERS;

    /* description of the device */
    dc->desc = "A PCI device to learn how this shit works";

    /* set the PCI miscelaneous category */
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

/* standard type info declaration
 * (see qom/object.h:476) */
static const TypeInfo beef_info = {
    .name = TYPE_BEEF_DEVICE,
    .parent = TYPE_PCI_DEVICE,
    .instance_size = sizeof(BeefState),
    .class_init = beef_class_init,
    .abstract = false,

    /* such fields could be set if we needed to */
    // .instance_init = beef_instance_init,
    // .instance_finalize = beef_instance_finalize,

    /* @interfaces: The list of interfaces associated with this type.  This
     *  should point to a static array that's terminated with a zero filled
     *  element.  */
    .interfaces = (InterfaceInfo[]){{INTERFACE_PCIE_DEVICE}, {}},
};

/* this registers the type in qemu. More specifically, checks if the name is
 * valid and adds the config to a hash table for fast access. */
static void beef_register_types(void) { type_register_static(&beef_info); }

type_init(beef_register_types)
