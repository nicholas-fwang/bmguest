#include <stdio.h>
#include <stdint.h>

#include <pl01x.h>
#include <gic.h>

#include <test_vdev_sample.h>

void main(void)
{
	int i = 0;
    pl01x_init(115200, 24000000);
    __malloc_init();
    test_vdev_sample();
    gic_init();

    printf("BMGuest is Done\n");
}
