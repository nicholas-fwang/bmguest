#ifndef __IO_H__
#define __IO_H__
#include <stdint.h>

#define dmb() asm __volatile__ ("dmb" : : : "memory")

#define __writel(v, a)  (*(volatile uint32_t *)(a) = (v))
#define __writew(v, a)  (*(volatile uint16_t *)(a) = (v))
#define __writeb(v, a)  (*(volatile uint8_t *)(a) = (v))

#define __readl(a)      (*(volatile uint32_t *)(a))
#define __readw(a)      (*(volatile uint16_t *)(a))
#define __readb(a)      (*(volatile uint8_t *)(a))

#define writel(v, a)    ({ uint32_t vl = v; dmb(); __writel(vl, a); })
#define writew(v, a)    ({ uint16_t vl = v; dmb(); __writew(vl, a); })
#define writeb(v, a)    ({ uint8_t vl = v; dmb(); __writeb(vl, a); })

#define readl(a)        ({ uint32_t vl = __readl(a); dmb(); vl; })
#define readw(a)        ({ uint16_t vl = __readw(a); dmb(); vl; })
#define readb(a)        ({ uint8_t vl = __readb(a); dmb(); vl; })

void write64(uint64_t value, uint32_t addr);
uint64_t read64(uint32_t addr);

#endif
