#include <stdio.h>
#include <stdint.h>

//#include <pl01x.h>
#include <serial_sh.h>
#include <gic.h>

#include <test_vdev_sample.h>

#define irq_enable() asm volatile("cpsie i" : : : "memory")
void main(void)
{
	int i = 0;
 //   pl01x_init(115200, 24000000);
	serial_init();
    __malloc_init();
    test_vdev_sample();
    gic_init();
    //gic_configure_irq(30, GIC_INT_POLARITY_LEVEL,0 ,0);
    //enable_irq(30);
    irq_enable();

    printf("BMGuest is Done\n");
    while(1)
	{
		printf("hello world\n");
	}
}
