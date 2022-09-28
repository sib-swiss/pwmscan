#
# Makefile for PWMScan project
#
CC = gcc
LDFLAGS = -fPIC -lm

CFLAGS = -O3 -std=gnu99 -W -Wall -pedantic
CFLAGS2 = -fPIC -O3 -std=gnu99 -W -Wall

binDir = $(PWD)/bin
genomeDir = $(PWD)/genomedb

PROGS = bowtie2bed mscan_bed2sga mscan2bed filterOverlaps mba matrix_scan matrix_prob seq_extract_bcomp pwm_scoring
SCRIPTS = perl_tools/jasparconvert.pl perl_tools/lpmconvert.pl perl_tools/pfmconvert.pl perl_tools/pwm2lpmconvert.pl perl_tools/pwmconvert.pl perl_tools/transfaconvert.pl pwm_scan pwm_scan_ucsc pwmlib_scan pwmlib_scan_seq pwm_bowtie_wrapper pwm_mscan_wrapper pwm_mscan_wrapper_ucsc pwm_convert

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
	cp -pfr $(SCRIPTS) $(binDir)
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
	rm -rf $(OBJS) $(PROGS)

cleanbin :
	rm -rf $(binDir)/mba $(binDir)/bowtie2bed $(binDir)/filterOverlaps $(binDir)/mscan2bed \
	$(binDir)/mscan_bed2sga $(binDir)/matrix_scan $(binDir)/matrix_prob $(binDir)/seq_extract_bcomp \
	$(binDir)/pwm_scoring $(binDir)/pwm_scan $(binDir)/pwm_scan_ucsc $(binDir)/pwmlib_scan $(binDir)/pwmlib_scan_seq \
	$(binDir)/pwm_mscan_wrapper $(binDir)/pwm_mscan_wrapper_ucsc $(binDir)/pwm_bowtie_wrapper \
	$(binDir)/pwm_convert $(binDir)/jasparconvert.pl \
	$(binDir)/lpmconvert.pl $(binDir)/pfmconvert.pl $(binDir)/pwmconvert.pl $(binDir)/transfaconvert.pl $(binDir)/pwm2lpmconvert.pl
