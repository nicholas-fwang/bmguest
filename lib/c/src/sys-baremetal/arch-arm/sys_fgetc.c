#include <stdio.h>
#include <stdint.h>
//#include <pl01x.h>
#include <serial_sh.h>

extern int __fgetc();
/* Put character for elf-loader */
int
__fgetc()
{
	//return pl01x_getc();
	return serial_getc();
}

