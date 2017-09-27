PWMScan
============================================================================
PWMScan is a tool to scan entire genomes of common model organisms
with a position weight matrix (PWM).


Description
============================================================================

The Position Weight Matrix (PWM) is the most commonly used model to describe 
the DNA binding motif of a transcription factor. A PWM contains weights for
each base at each motif position. A PWM score can be computed for any base 
sequence of the same length by simply summing up the corresponding weights 
from the PWM.

PWMScan is a software package with a Web interface.

PWMScan can use two alternative search engines:

 - bowtie, a fast memory-efficient short read aligner using indexed genomes
 - matrix_scan, a C program using a conventional search algorithm

Basically, two approaches are used to scan large DNA sequences such as genomes
with a PWM:

 1) use fast string matching algorithms to scan the genome as follows:

     - given a PWM model and a cut-off value, generate all possible matches/tags
       that represent the given PWM along with the corresponding scores; 
     - map the list of tag to a reference genome or a set of DNA sequences,
       using a fast string-matching algorithm (e.g bowtie).

 2) use a conventional algorithm that scans DNA sequences with a PWM (matrix_scan).
    matrix_scan first rescales the scoring matrix so as to have the maximum score
    set to zero. It then computes the PWM score for each position along the sequence,
    and drops out as soon as the score is below a re-computed cut-off value.
    
The Web interface automatically chooses the most suitable method.

We use integer log likelihoods or integer log-odds as the internal working
format of PWMs. The PWM has one column for each nucleotide in DNA sequences,
and it has one row for each position in the pattern. The scores at each 
position are calculated as the sum of integer log likelihoods (log-odds).

We provide several tools for matrix format conversion to integer log-odds,
in particular for JASPAR, TRANSFAC, real PWMs, letter probability matrices (LPMs),
and position frequency matrices (PFMs).
These tools are included in the perl_tools directory.
 
The match list is provided in BED format, with the following fields:

 1- chromosome name (e.g. chr1, chrX, chrM) 
 2- starting position of the matching sequence
 3- ending position of the matching sequence
 4- matching nucleotide sequence 
 5- integer score of the matching sequence 
 6- strand (either '+' or '-')

For a complete description of BED format, please refer to the UCSC site at:

 http://genome.ucsc.edu/FAQ/FAQformat.html#format1


Programs and utilities
============================================================================

The main programs are the following:

 - matrix_prob       Compute the cumulative PWM score distribution
                     given a background model, and a PWM.
                     Used for PWM score computation and conversion.

 - mba               Matrix Branch-and-bound Algorithm (mba) generates a list
                     of all matching sequences given an integer PWM and a cut-off.
                     MBA is the prior step to using Bowtie.

 - matrix_scan       Scan a set of sequence files with a PWM and a cut-off value.

 - bowtie2bed        Convert the BOWTIE output into BED format.

 - mscan2bed         Convert the matrix_scan output into BED format.

 - mscan_bed2sga     Convert the BED file from the PWMSCan pipeline into SGA format.

 - filterOverlaps    Filter out overlapping matches for BED format.

 
The bowtie package is available on SourceForge.net for all UNIX-based platforms:

  - bowtie           http://bowtie-bio.sourceforge.net/index.shtml

SGA is a single-line-oriented and tab-delimited format with the following five 
obligatory fields:

  1. Sequence name/ID (Char String),
  2. Feature (Char String),
  3. Sequence Position (Integer),
  4. Strand (+/- or 0),
  5. Tag Counts (Integer).

An additional field may be added containing application-specific information used by
other programs. In the case of ChIP-seq data, SGA files represent genome-wide tag count
distributions from one or several experiments.
SGA is the working format of our ChIP-seq data analysis programs (sourceforge.net/projects/chip-seq).

Matrix format conversion utilities are the following:

  - jasparconvert.pl      Convert a JASPAR matrix file to MEME format (integer log-odds).

  - transfaconvert.pl     Convert a TRANSFAC matrix file to MEME format (integer log-odds).

  - pwmconvert.pl         Convert a real or SSA-formatted PWM to integer plain-text format.

  - lpmconvert.pl         Convert a Letter Probability Matrix (LPM) file to an integer PWM. 

  - pfmconvert.pl         Convert a Position Frequency Matrix (PFM) file to either a PWM or 
                          a letter probabilities matrix.

The jasparconvert.pl and transfaconvert.pl scripts are based on an original implementation
by William Stafford Noble and Timothy L. Bailey (1999) for MEME (http://meme-suite.org/).

We also provide three (bash) shell wrapper scripts that embed the entire analysis pipeline:

  - pwm_scan              Scan a genome with a PWM and a p-value using either Bowtie or matrix_scan
  - pwm_bowtie_wrapper    Scan a genome with a PWM and a p-value using Bowtie
  - pwm_mscan_wrapper     Scan a genome with a PWM and a p-value using matrix_scan


PWMScan pipeline and application example
============================================================================

Given a PWM and a p-value, the PWMScan pipeline includes the following steps:

  1) Compute the integer cut-off value, given a PWM and a p-value;

  2) Scan the genome
     To use the fast string-matching-based approach, go to steps a) and b):

       a) Generate a list of all possible matching sequences, given a PWM and a cut-off value
          This step is required for Bowtie;

       b) Map the matching sequences to the genome; 

     To use the conventional algorithm execute step c):

       c) Scan the genome with the PWM using matrix_scan;
 
  3) Convert the match list to BED format.

In the example directory, you can find a complete application example exploiting both methods.


Web Interface
============================================================================

PWMScan has a web interface which is freely available at:

   http://ccg.vital-it.ch/pwmtools/pwmscan.php

The Web interface has the following characteristics:

  - Menu-driven selection of server-resident genomes
  - Access to large colllections of PWMs from MEME and other databases
  - Custom PWMs are supplied by copy&paste or file upload
  - Support of various PWM formats: JASPAR, TRANSFAC, plain text
  - Cut-off defined as PWM-score, match percentage, or p-value
  - Output provided in various formats: BED, SGA, FPS, etc.
  - Direct links to the UCSC genome browser for visualization of results
  - Useful links for downstream analysis (ChIP-seq data correlation, and motif analysis tools)

The Web interface doesn't support upload of user-supplied FASTA sequence files.
