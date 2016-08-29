#
# Makefile for PWMScan project
#
CC = gcc
LDFLAGS = -fPIC -lm

CFLAGS = -O2 -std=gnu99 -W -Wall -pedantic
CFLAGS2 = -fPIC -O2 -std=gnu99 -W -Wall

binDir = ./bin.x86_64

PROGS = mba bowtie2bed mscan_bed2sga mscan2bed filterOverlaps matrix_scan
OBJS = hashtable.o

all :  $(PROGS)

MBA_SRC = mba.c
BOWTIE2BED_SRC = bowtie2bed.c
MSCAN2BED_SRC = mscan2bed.c
MSCAN_BED2SGA_SRC = mscan_bed2sga.c
FILTEROVERLAPS_SRC = filterOverlaps.c

MATRIX_SCAN_SRC = main.c io.c matrix.c sequence.c misc.c errors.c help_messages.c
MATRIX_SCAN_OBJS = main.o io.o matrix.o sequence.o misc.o errors.o help_messages.o

bowtie2bed : $(BOWTIE2BED_SRC) $(OBJS)
	$(CC) $(LDFLAGS) -o bowtie2bed $^

mscan2bed : $(MSCAN2BED_SRC) $(OBJS)
	$(CC) $(LDFLAGS) -o mscan2bed $^

mscan_bed2sga : $(MSCAN_BED2SGA_SRC) $(OBJS)
	$(CC) $(LDFLAGS) -o mscan_bed2sga $^

%.o : %.c
	$(CC) $(CFLAGS2) -o $@ -c $^

mba : $(MBA_SRC)
	$(CC) $(CFLAGS) -o mba $(MBA_SRC)

filterOverlaps : $(FILTEROVERLAPS_SRC)
	$(CC) $(CFLAGS) -o filterOverlaps $(FILTEROVERLAPS_SRC)

matrix_scan : $(MATRIX_SCAN_OBJS)
	$(CC) $(CFLAGS) -o matrix_scan $^

main.o: main.c io.c matrix.c sequence.c misc.c errors.c help_messages.c
	$(CC) $(CFLAGS) -c $(MATRIX_SCAN_SRC)

help_messages_o: help_message.c
	$(CC) $(CFLAGS) -c help_messages.c

errors.o: errors.c
	$(CC) $(CFLAGS) -c errors.c

misc.o: misc.c
	$(CC) $(CFLAGS) -c misc.c

sequence.o: sequence.c
	$(CC) $(CFLAGS) -c sequence.c

matrix.o: matrix.c
	$(CC) $(CFLAGS) -c matrix.c

io.o: io.c
	$(CC) $(CFLAGS) -c io.c

install:
	mv $(PROGS) $(binDir)

clean :
	rm -rf $(OBJS) $(MATRIX_SCAN_OBJS) $(PROGS)

cleanbin :
	rm -rf $(binDir)/mba $(binDir)/bowtie2bed $(binDir)/filterOverlaps \
	$(binDir)/mscan2bed $(binDir)/mscan_bed2sga $(binDir)/matrix_scan
