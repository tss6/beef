#ifndef BEEF_DMA_H
#define BEEF_DMA_H

#include "beef.h"

MemTxResult beef_dma_read(BeefState *dev);
MemTxResult beef_dma_write(BeefState *dev);

#endif /* BEEF_DMA_H */
