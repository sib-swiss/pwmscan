#!/bin/sh
set -x -e

# programs installed
# bowtie2bed filterOverlaps matrix_prob matrix_scan mba mscan2bed mscan_bed2sga pwm_bowtie_wrapper pwm_convert pwm_mscan_wrapper pwm_mscan_wrapper_ucsc pwm_scan pwm_scan_ucsc pwm_scoring pwmlib_scan pwmlib_scan_seq python seq_extract_bcomp
# jasparconvert.pl lpmconvert.pl pfmconvert.pl pwm2lpmconvert.pl pwmconvert.pl transfaconvert.pl matrix_scan_parallel.py


export DESTDIR="${PREFIX}"
make clean -f Makefile.conda
make -f Makefile.conda
# Fix binDir path
sed -i 's|^binDir = .*|binDir = \$(DESTDIR)/bin|' Makefile.conda
# Fix Perl bang path
sed -i 's@#!.*perl@#!/usr/bin/perl@' perl_tools/*.pl
#FIXME issue with the python/ folder in DESTDIR/bin where a python symlink is already there!
#FIXME issue with all hardcoded paths done by sed during *make install* (most should be in PATH)
#FIXME what to do of /home/local/db/ paths???
make install -f Makefile.conda
