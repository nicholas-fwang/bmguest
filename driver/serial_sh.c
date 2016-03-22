#include "serial_sh.h"
#include "io.h"

static void serial_setbrg(void)
{
	unsigned short dl = DL_VALUE(BAUDRATE,SCIF_CLK_FREQ);
	writew(dl,SCIF_BASE + DL);
}

int serial_init(void)
{
	writew(SCSCR_INIT,SCIF_BASE+SCSCR);
	writew(SCSCR_INIT,SCIF_BASE+SCSCR);
    writew(0,SCIF_BASE+SCSMR);
    writew(0,SCIF_BASE+SCSMR);
    writew(SCFCR_RFRST|SCFCR_TFRST,SCIF_BASE+SCFCR);

    readw(SCIF_BASE+SCFCR);

    writew(0,SCIF_BASE+SCFCR);

	serial_setbrg();
	return 0;
}

static void handle_error(void)
{
//    readw(SCIF_BASE+SCxSR);
    writew(SCxSR_ERROR_CLEAR,SCIF_BASE+SCxSR);
//    readw(SCIF_BASE+SCLSR);
    writew(0x00,SCIF_BASE+SCLSR);
}

static void serial_raw_putc(const char c)
{
    while (1) {
        if(readw(SCIF_BASE+SCxSR) & SCIF_TEND)
            break;
    }

    writeb(c,SCIF_BASE+SCxTDR);
    writew(readw(SCIF_BASE+SCxSR) & ~SCIF_TEND,SCIF_BASE+SCxSR);

}

int serial_putc(const char c)
{
	if (c == '\n')
		serial_raw_putc('\r');
	serial_raw_putc(c);
	return c;
}


static int serial_getc_check(void)
{
    unsigned short status;
    status = readw(SCIF_BASE+SCxSR);
    if(status & SCIF_ERRORS)
        handle_error();
    if(readw(SCIF_BASE+SCLSR) & SCIF_ORER)
        handle_error();
    return status & (SCIF_DR | SCIF_RDF);

}

int serial_getc(void)
{
    unsigned short status;
    char ch;

    while (!serial_getc_check())
        ;

    ch = readb(SCIF_BASE+SCxRDR);
    status = readw(SCIF_BASE+SCxSR);

    writew(SCxSR_RDxF_CLEAR,SCIF_BASE+SCxSR);

    if(status & SCIF_ERRORS)
        handle_error();
    if(readw(SCIF_BASE+SCLSR) & SCIF_ORER)
        handle_error();
    return ch;
}

