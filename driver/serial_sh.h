#include <io.h>

#define SCIF_BASE   0xE6E60000

#define SCIF_CLK_FREQ 14745600

#define BAUDRATE 38400

#define SCSCR_INIT	0x32

#define SCIF_ER    0x0080
#define SCIF_TEND  0x0040
#define SCIF_BRK   0x0010
#define SCIF_FER   0x0008
#define SCIF_PER   0x0004
#define SCIF_RDF   0x0002
#define SCIF_DR    0x0001

#define SCIF_ERRORS (SCIF_PER | SCIF_FER | SCIF_ER | SCIF_BRK)

#define SCIF_ORER	0x0000

#define SCxSR_RDxF_CLEAR	0x00fc
#define SCxSR_ERROR_CLEAR	0x0073

#define SCFCR_RFRST 0x0002
#define SCFCR_TFRST 0x0004

#define SCSCR   0x08
#define SCSMR   0x00
#define SCFCR   0x18
#define SCxSR   0x10
#define SCxTDR  0x0c
#define SCxRDR  0x14
#define DL  0x30
#define SCLSR 0x24

#define DL_VALUE(bps, clk) (clk / bps / 16) 

int serial_init(void);
int serial_putc(const char c);
int serial_getc(void);
