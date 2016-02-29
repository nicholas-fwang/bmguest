include config.mk

ASM_FILES	= src/head.S
ASM_FILES 	+= $(wildcard lib/c/src/arch-arm/*.S)

C_FILES 	= $(wildcard src/*.c)
C_FILES 	+= $(wildcard driver/*.c)
C_FILES 	+= $(wildcard lib/c/src/*.c)
C_FILES 	+= $(wildcard lib/c/src/arch-arm/*.c)
C_FILES 	+= $(wildcard lib/c/src/sys-baremetal/*.c)
C_FILES 	+= $(wildcard lib/c/src/sys-baremetal/arch-arm/*.c)
C_FILES 	+= $(wildcard test/*.c)

OBJS 		:= $(ASM_FILES:.S=.o) $(C_FILES:.c=.o)

BIN			= bmguest.bin
LD_SCRIPT	= bmguest.ld.S
OUTPUT 		= bmguest.axf
MAP			= bmguest.map

CC			= $(CROSS_COMPILE)gcc
LD			= $(CROSS_COMPILE)ld
NM			= $(CROSS_COMPILE)nm
OBJCOPY		= $(CROSS_COMPILE)objcopy


INCLUDES	= -I./src
INCLUDES	+= -I./driver
INCLUDES	+= -I./include
INCLUDES	+= -I./lib/c/include
INCLUDES	+= -I./test


CPPFLAGS	= $(CONFIG_FLAG) $(INCLUDES) -ffreestanding -nostdlib -nodefaultlibs -nostartfiles $(DEBUG_FLAG)
CPPFLAGS	+= -Wall -Wl,--build-id=none

all: $(OBJS) $(OUTPUT) $(MAP) $(BIN)

$(MAP): $(OUTPUT)
	$(NM) $< > $@

clean:
	rm -f $(MAP) $(OUTPUT) $(BIN) \
	bmguest.ld $(OBJS)

$(OUTPUT): bmguest.ld $(OBJS)
	$(CC) $(CPPFLAGS) -T bmguest.ld -o $@ $(OBJS)


$(BIN): $(OUTPUT)
	$(OBJCOPY) -O binary $(OUTPUT) $(BIN)

%.o: %.S
	$(CC) $(CPPFLAGS) -I. -c -o $@ $<

%.o: %.c
	$(CC) $(CPPFLAGS) -I. -c -o $@ $<

bmguest.ld: bmguest.ld.S Makefile
	$(CC) $(CPPFLAGS) -E -P -C -o $@ $<

%: force
	$(MAKE) -C $(KERNEL_SRC) $@

force: ;

Makefile: ;

.PHONY: all clean config.mk
