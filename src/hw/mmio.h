#ifndef BEEF_MMIO_H
#define BEEF_MMIO_H
#include "beef.h"

extern const MemoryRegionOps beef_mmio_ops;

void beef_mmio_init(BeefState *dev, Error **errp);

#endif /* BEEF_MMIO_H */
