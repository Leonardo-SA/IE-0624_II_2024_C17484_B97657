CFLAGS = -DTEST
OBJS = sdram.o clock.o console.o lcd-spi.o gfx.o spi-mems.o

BINARY = main

LDLIBS += -lm 

LDSCRIPT = ../stm32f429i-discovery.ld

OOCD_INTERFACE = stlink-v2-1

include ../../Makefile.include

