#include <stdio.h>
#include <stdint.h>

#include <pl01x.h>

#include <test_vdev_sample.h>

void main(void)
{
	int i = 0;
    pl01x_init(115200, 24000000);
    __malloc_init();
    test_vdev_sample();

    printf("BMGuest is Done\n");
}
