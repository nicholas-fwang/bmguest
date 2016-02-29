#include <stdio.h>
#include <gic.h>

#include <gic_regs.h>
#include <asm_io.h>

#define gicd_read(offset)           getl(gic_hw.base + GICD_OFFSET + offset)
#define gicd_write(offset, value)   putl(value, gic_hw.base + GICD_OFFSET + offset)

#define gicc_read(offset)           getl(gic_hw.base + GICC_OFFSET + offset)
#define gicc_write(offset, value)   putl(value, gic_hw.base + GICC_OFFSET + offset)

#define read_cbar()              ({ uint32_t rval; asm volatile(\
                                    " mrc     p15, 4, %0, c15, c0, 0\n\t" \
                                    : "=r" (rval) : : "memory", "cc"); rval; })

typedef void (*gic_irq_handler_t)();
gic_irq_handler_t irq_handlers[1024];

static uint64_t get_periphbase(void)
{
    uint32_t periphbase = 0x0;
    uint32_t cbar = (uint64_t) read_cbar();
    uint8_t upper_periphbase = cbar & 0xFF;

    printf("%s[%d]: value of cbar: 0x%08x\n", __func__, __LINE__, cbar);

    if (upper_periphbase != 0) {
        periphbase |= (upper_periphbase << 32);
        cbar &= ~(0xFF);
    }
    periphbase |= cbar;

    return periphbase;
}

void gic_init(void)
{
    int i;

    // This code will be moved other parts, not here. */
    /* Get GICv2 base address */
    gic_hw.base = (uint32_t)(get_periphbase() & 0x000000FFFFFFFFFFULL);
    if(gic_hw.base == 0x0) {
        printf("Warning: Curretn architecture has no value in CBAR\n    \
                The architecture do not follow design philosophy from ARM recommendation\n");
        // TODO(casionwoo): vaule of base address will be read from DTB or configuration file.
        // Currently, we set the base address of gic to 0x2C000000, it is for RTSM.
        gic_hw.base = 0x2C000000;
    }

    gic_hw.nr_irqs = (uint32_t) (32 * ((gicd_read(GICD_TYPER) & GICD_TYPE_LINES_MASK) + 1));
    gic_hw.nr_cpus = (uint32_t) (1 + ((gicd_read(GICD_TYPER) & GICD_TYPE_CPUS_MASK) >> GICD_TYPE_CPUS_SHIFT));

    gicc_write(GICC_CTLR, GICC_CTL_ENABLE | GICC_CTL_EOI);
    gicd_write(GICD_CTLR, 0x1);

    /* No Priority Masking: the lowest value as the threshold : 255 */
    // We set 0xff but, real value is 0xf8
    gicc_write(GICC_PMR, 0xff);

    printf("GIC: nr_irqs: 0x%08x\n", gic_hw.nr_irqs);
    printf(" nr_cpus: 0x%08x\n", gic_hw.nr_cpus);

    // Set interrupt configuration do not work.
    for (i = 32; i < gic_hw.nr_irqs; i += 16) {
        printf("BEFORE: GICD_ICFGR(%d): 0x%08x\n", i >> 4, gicd_read(GICD_ICFGR(i >> 4)));
        gicd_write(GICD_ICFGR(i >> 4), 0x0);
        printf("AFTER: GICD_ICFGR(%d): 0x%08x\n", i >> 4, gicd_read(GICD_ICFGR(i >> 4)));
    }

    /* Disable all global interrupts. */
    for (i = 0; i < gic_hw.nr_irqs; i += 32) {
        printf("BEFORE: GICD_ICENABLER(%d): 0x%08x\n", i >> 5, gicd_read(GICD_ICENABLER(i >> 5)));
        gicd_write(GICD_ISENABLER(i >> 5), 0xffffffff);
        uint32_t valid = gicd_read(GICD_ISENABLER(i >> 5));
        gicd_write(GICD_ICENABLER(i >> 5), valid);
        printf("AFTER: GICD_ICENABLER(%d): 0x%08x\n", i >> 5, gicd_read(GICD_ICENABLER(i >> 5)));
    }

    // We set priority 0xa0 for each but real value is a 0xd0, Why?
    /* Set priority as default for all interrupts */
    for (i = 0; i < gic_hw.nr_irqs; i += 4) {
        printf("BEFORE: GICD_IPRIORITYR(%d): 0x%08x\n", i >> 2, gicd_read(GICD_IPRIORITYR(i >> 2)));
        gicd_write(GICD_IPRIORITYR(i >> 2), 0xa0a0a0a0);
        printf("AFTER: GICD_IPRIORITYR(%d): 0x%08x\n", i >> 2, gicd_read(GICD_IPRIORITYR(i >> 2)));
    }

    /* Route all global IRQs to CPU0 */
    for (i = 32; i < gic_hw.nr_irqs; i += 4) {
        printf("BEFORE: GICD_ITARGETSR(%d): 0x%08x\n", i >> 2, gicd_read(GICD_ITARGETSR(i >> 2)));
        gicd_write(GICD_ITARGETSR(i >> 2), 1 << 0 | 1 << 8 | 1 << 16 | 1 << 24);
        printf("AFTER: GICD_ITARGETSR(%d): 0x%08x\n", i >> 2, gicd_read(GICD_ITARGETSR(i >> 2)));
    }

    for (i = 0; i < 1024; i++) {
        irq_handlers[i] = 0x0;
    }
}

void gic_configure_irq(uint32_t irq,
        enum gic_irq_polarity polarity,  uint8_t cpumask,
        uint8_t priority)
{
    if (irq < gic_hw.nr_irqs) {
        uint32_t icfg;
        volatile uint8_t *reg8;
        /* disable forwarding */
        gic_disable_irq(irq);
        /* polarity: level or edge */
        icfg = gicd_read(GICD_ICFGR(irq >> 4));

        if (polarity == GIC_INT_POLARITY_LEVEL)
            icfg &= ~(2u << (2 * (irq % 16)));
        else
            icfg |= (2u << (2 * (irq % 16)));

        gicd_write(GICD_ICFGR(irq >> 4), icfg);
        gicd_write(GICD_ITARGETSR(irq >> 2), cpumask);
        gicd_write(GICD_IPRIORITYR(irq >> 2), priority);

        /* enable forwarding */
        enable_irq(irq);
    }
}

uint32_t gic_get_irq_number(void)
{
    return gicc_read(GICC_IAR) & GICC_IAR_MASK;
}

void gic_set_sgi(const uint32_t target, uint32_t sgi)
{
    gicd_write(GICD_SGIR(0), GICD_SGIR_TARGET_LIST |
                            (target << GICD_SGIR_CPU_TARGET_LIST_OFFSET) |
                            (sgi & GICD_SGIR_SGI_INT_ID_MASK));
}

/* API functions */
void enable_irq(uint32_t irq)
{
    gicd_write(GICD_ISENABLER(irq >> 5), 1UL << (irq & 0x1F));
}

void gic_disable_irq(uint32_t irq)
{
    gicd_write(GICD_ICENABLER(irq >> 5), 1UL << (irq & 0x1F));
}

void gic_completion_irq(uint32_t irq)
{
    gicc_write(GICC_EOIR, irq);
}

void gic_deactivate_irq(uint32_t irq)
{
    gicc_write(GICC_DIR, irq);
}

void gic_set_irq_handler(int irq, gic_irq_handler_t handler)
{
    if ( irq < 1024 ) {
        irq_handlers[irq] = handler;
    }
}

void do_irq(void *pregs)
{
    uint32_t irq;
    irq = gic_get_irq_number();
    printf("Get IRQ number %d\n", irq);
    if (irq_handlers[irq]) {
        irq_handlers[irq]();
    }
    gic_completion_irq(irq);
    gic_deactivate_irq(irq);
}
