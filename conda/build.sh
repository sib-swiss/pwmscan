#!/bin/sh
set -x -e

# programs installed
# bowtie2bed filterOverlaps matrix_prob matrix_scan mba mscan2bed mscan_bed2sga pwm_bowtie_wrapper pwm_convert pwm_mscan_wrapper pwm_mscan_wrapper_ucsc pwm_scan pwm_scan_ucsc pwm_scoring pwmlib_scan pwmlib_scan_seq scan_genome_with_lib scan_seq_with_lib seq_extract_bcomp
# jasparconvert.pl lpmconvert.pl pfmconvert.pl pwm2lpmconvert.pl pwmconvert.pl transfaconvert.pl


export DESTDIR="${PREFIX}"
make clean
make
# Fix binDir path
sed -i 's|^binDir = .*|binDir = \$(DESTDIR)/bin|' Makefile
#FIXME what to do of /home/local/db/ paths???
make install-conda
