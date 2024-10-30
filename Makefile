#
# Makefile for PWMScan project
#
CC = gcc
LDFLAGS = -O3 -fPIC -lm

CFLAGS = -O3 -std=gnu99 -W -Wall -Wextra -pedantic
CFLAGS2 = -fPIC -O3 -std=gnu99 -W -Wall -Wextra

binDir = $(PWD)/bin
genomeDir = $(PWD)/genomedb

PROGS = bowtie2bed mscan_bed2sga mscan2bed filterOverlaps mba matrix_scan matrix_prob seq_extract_bcomp pwm_scoring seqshuffle
SCRIPTS = $(wildcard perl_tools/*.pl) pwm_scan pwm_scan_ucsc pwmlib_scan pwmlib_scan_seq pwm_bowtie_wrapper pwm_mscan_wrapper pwm_mscan_wrapper_ucsc pwm_convert scan_genome_with_lib scan_seq_with_lib

OBJS = hashtable.o

all :  $(PROGS)

MBA_SRC = mba.c
MATRIX_PROB_SRC = matrix_prob.c
MATRIX_SCAN_SRC = matrix_scan.c
BOWTIE2BED_SRC = bowtie2bed.c
MSCAN2BED_SRC = mscan2bed.c
MSCAN_BED2SGA_SRC = mscan_bed2sga.c
SEQ_EXTRACT_BCOMP_SRC = seq_extract_bcomp.c
PWM_SCORING_SRC = pwm_scoring.c
FILTEROVERLAPS_SRC = filterOverlaps.c
SEQSHUFFLE_SRC = seqshuffle.c

MATRIX_SCAN_SRC =  matrix_scan.c

bowtie2bed : $(BOWTIE2BED_SRC) $(OBJS)
	$(CC) $(LDFLAGS) -o bowtie2bed $^

mscan2bed : $(MSCAN2BED_SRC) $(OBJS)
	$(CC) $(LDFLAGS) -o mscan2bed $^

mscan_bed2sga : $(MSCAN_BED2SGA_SRC) $(OBJS)
	$(CC) $(LDFLAGS) -o mscan_bed2sga $^

%.o : %.c
	$(CC) $(CFLAGS2) -o $@ -c $^

filterOverlaps : $(FILTEROVERLAPS_SRC)
	$(CC) $(CFLAGS) -o filterOverlaps $^

seqshuffle : $(SEQSHUFFLE_SRC)
	$(CC) $(CFLAGS) -o seqshuffle $^

mba : $(MBA_SRC)
	$(CC) $(CFLAGS) -o mba $(MBA_SRC)

matrix_prob : $(MATRIX_PROB_SRC)
	$(CC) $(CFLAGS) -o matrix_prob $^

matrix_scan : $(MATRIX_SCAN_SRC)
	$(CC) $(CFLAGS) -o matrix_scan $^

seq_extract_bcomp : $(SEQ_EXTRACT_BCOMP_SRC) $(OBJS)
	$(CC) $(CFLAGS) -o seq_extract_bcomp $^

pwm_scoring : $(PWM_SCORING_SRC)
	$(CC) $(CFLAGS) -o pwm_scoring $^

install : $(PROGS) $(SCRIPTS)
	mkdir -p $(binDir)/
	mv -f $(PROGS) $(binDir)
	install $(SCRIPTS) $(binDir)
	sed -i -e 's@/home/local/bin@$(binDir)@g'  $(binDir)/pwm_scan
	sed -i -e 's@/home/local/bin@$(binDir)@g'  $(binDir)/pwm_scan_ucsc
	sed -i -e 's@/home/local/bin@$(binDir)@g'  $(binDir)/pwmlib_scan
	sed -i -e 's@/home/local/bin@$(binDir)@g'  $(binDir)/pwmlib_scan_seq
	sed -i -e 's@/home/local/bin@$(binDir)@g'  $(binDir)/pwm_mscan_wrapper
	sed -i -e 's@/home/local/bin@$(binDir)@g'  $(binDir)/pwm_mscan_wrapper_ucsc
	sed -i -e 's@/home/local/bin@$(binDir)@g'  $(binDir)/pwm_bowtie_wrapper
	sed -i -e 's@/home/local/bin@$(binDir)@g'  $(binDir)/pwm_convert

install-conda : $(PROGS) $(SCRIPTS)
	mkdir -p $(binDir)/
	mv -f $(PROGS) $(binDir)
	install $(SCRIPTS) $(binDir)
	#All binaries are in the PATH, so remove hardcoded paths for conda deployment
	sed -i -e 's@bash $$bin_dir/@@g'           $(binDir)/pwmlib_scan*
	sed -i -e 's@$$bin_dir/@@g'                $(binDir)/pwmlib_* $(binDir)/pwm_*

install-genome :
	gunzip $(genomeDir)/hg19/chrom*.seq.gz

clean :
	$(RM) $(OBJS) $(PROGS)

cleanbin :
	$(RM) $(addprefix $(binDir)/, $(PROGS) $(notdir $(SCRIPTS)))

