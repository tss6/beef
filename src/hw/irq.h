#ifndef BEEF_IRQ_H
#define BEEF_IRQ_H

#include "beef.h"

void beef_irq_init(BeefState *dev, Error **errp);
void beef_irq_deinit(BeefState *dev);
void beef_irq_raise(BeefState *dev, unsigned int vector);
void beef_irq_lower(BeefState *dev, unsigned int vector);
void beef_irq_reset(BeefState *dev);

#endif /* BEEF_IRQ_H */
