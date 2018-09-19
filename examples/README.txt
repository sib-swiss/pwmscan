Application Examples
==============================================================================
Scan the hg19 human genome with the chen10 CTCF Position Weight Matrix (PWM)
==============================================================================

Important remarks:
==============================================================================

For the following application examples we will use the hg19 human assembly files that can be found in the genomedb sub-directory of the PWMScan software package (https://sourceforge.net/projects/pwmscan).

The PWMScan programs rely on a few rules to make output conversion to BED format easier, namely FASTA header and file naming conventions.

The final output format is BEDdetail, which is an extension of BED format that is used to enhance the track display page.
For PWMScan, we use BEDdetail format to include the name of the PWM as well as the p-value associated to the sites identified by PWMScan.

The BEDdetail format provides the following obligatory fields:

1) Chromosome name (e.g. chr1, chrX, etc.)
2) Starting position of the match
3) Ending position of the match
4) Matching nucleotide sequence
5) Integer score of PWM match
6) Strand (+, -)
7) PWM name
8) P-value of the match


FASTA headers
-----------------------------------------------

All genome sequence files have a FASTA header that is formatted as follows:

  >chr|NC_000001|NC_000001.10 some text

The sequence identifier includes three words concatenated via a UNIX pipe '|': the word 'chr' followed by the NCBI RefSeq partial and full accession identifiers. The accession records indicate sequence identity. The sequence identifier is parsed by most of the PWMScan programs and utilities.

For each genome assembly, we provide a table for NCBI RefSeq identifier to chromosome number conversion (chr_NC_gi) as well as a table for FASTA sequence identifier to chromosome number conversion (chr_hdr). The chr_NC_gi file has been downloaded from NCBI whereas the chr_hdr file has been locally-generated. By consequence, the chr_hdr table can be changed to adapt to other FASTA header format conventions. 

Here is an example for the human genome assembly hg19:

chr_NC_gi
#Chr    Accession.ver   gi
1       NC_000001.10    224384768
2       NC_000002.11    224384767
3       NC_000003.11    224384766
4       NC_000004.11    224384765
5       NC_000005.9     224384764
6       NC_000006.11    224384763
7       NC_000007.13    224384762
8       NC_000008.10    224384761
9       NC_000009.11    224384760
10      NC_000010.10    224384759
11      NC_000011.9     224384758
12      NC_000012.11    224384757
13      NC_000013.10    224384756
14      NC_000014.8     224384755
15      NC_000015.9     224384754
16      NC_000016.9     224384753
17      NC_000017.10    224384752
18      NC_000018.9     224384751
19      NC_000019.9     224384750
20      NC_000020.10    224384749
21      NC_000021.8     224384748
22      NC_000022.10    224384747
X       NC_000023.10    224384746
Y       NC_000024.9     224384745
M       NC_012920.1     251831106


chr_hdr
#Chr    Sequence Header                 Assembly
1       chr|NC_000001|NC_000001.10      hg19
2       chr|NC_000002|NC_000002.11      hg19
3       chr|NC_000003|NC_000003.11      hg19
4       chr|NC_000004|NC_000004.11      hg19
5       chr|NC_000005|NC_000005.9       hg19
6       chr|NC_000006|NC_000006.11      hg19
7       chr|NC_000007|NC_000007.13      hg19
8       chr|NC_000008|NC_000008.10      hg19
9       chr|NC_000009|NC_000009.11      hg19
10      chr|NC_000010|NC_000010.10      hg19
11      chr|NC_000011|NC_000011.9       hg19
12      chr|NC_000012|NC_000012.11      hg19
13      chr|NC_000013|NC_000013.10      hg19
14      chr|NC_000014|NC_000014.8       hg19
15      chr|NC_000015|NC_000015.9       hg19
16      chr|NC_000016|NC_000016.9       hg19
17      chr|NC_000017|NC_000017.10      hg19
18      chr|NC_000018|NC_000018.9       hg19
19      chr|NC_000019|NC_000019.9       hg19
20      chr|NC_000020|NC_000020.10      hg19
21      chr|NC_000021|NC_000021.8       hg19
22      chr|NC_000022|NC_000022.10      hg19
X       chr|NC_000023|NC_000023.10      hg19
Y       chr|NC_000024|NC_000024.9       hg19
M       chr|NC_012920|NC_012920.1       hg19


 Note that the second field of the chr_hdr file corresponds to the chromosome sequence identifier in the FASTA files. Given that the Bowtie indices are generated starting from the chromosome FASTA files, the Bowtie output will include the sequence identifier as specified in the second field of the chr_hdr file. To convert to BED format, which uses chromosome numbers, programs such as bowtie2bed need to read the chr_hdr table.
 If Bowtie indices are generated from chromosome FASTA files with different headers (or sequence identifiers), the second field of the chr_hdr table has to be changed accordingly.

Chromosome files downloaded from UCSC are named chr*.fa. If this is the case, the adapted wrapper scripts pwm_scan_ucsc and pwm_mscan_wrapper_ucsc should be used instead of the deafult ones pwm_scan and pwm_mscan_wrapper (see the Wrapper Scripts Section). The pwm_bowtie_wrapper script should in principle work with genome index files built from UCSC genome assemblies, provided the chr_hdr file is changed according to the UCSC chromosome header convention, which is the following:

  >chr1

An example for the mouse genome mm10 downloaded from UCSC would then be the following:

chr_NC_gi
#Chr    Accession.ver   gi      Assembly
1       chr1    372099109       mm10
2       chr2    372099108       mm10
3       chr3    372099107       mm10
4       chr4    372099106       mm10
5       chr5    372099105       mm10
6       chr6    372099104       mm10
7       chr7    372099103       mm10
8       chr8    372099102       mm10
9       chr9    372099101       mm10
10      chr10   372099100       mm10
11      chr11   372099099       mm10
12      chr12   372099098       mm10
13      chr13   372099097       mm10
14      chr14   372099096       mm10
15      chr15   372099095       mm10
16      chr16   372099094       mm10
17      chr17   372099093       mm10
18      chr18   372099092       mm10
19      chr19   372099091       mm10
X       chrX    372099090       mm10
Y       chrY    372099089       mm10
M       chrM    34538597        mm10

chr_hdr
#Chr    Sequence Header  Assembly
1       chr1             mm10
2       chr2             mm10
3       chr3             mm10
4       chr4             mm10
5       chr5             mm10
6       chr6             mm10
7       chr7             mm10
8       chr8             mm10
9       chr9             mm10
10      chr10            mm10
11      chr11            mm10
12      chr12            mm10
13      chr13            mm10
14      chr14            mm10
15      chr15            mm10
16      chr16            mm10
17      chr17            mm10
18      chr18            mm10
19      chr19            mm10
X       chrX             mm10
Y       chrY             mm10
M       chrM             mm10


Files and paths
-----------------------------------------------
 The tar files genomes_chr_NC_gi.tar.gz and genomes_chr_hdr.tar.gz include a list of chr_NC_gi and chr_hdr tables for several common model organisms such as human, mouse, fruit fly, worm, zebrafish, yeast, and more. As mentioned above, this tables are used to convert match lists to BED format.

To check the content, type:

  tar -tvzf genomes_chr_NC_gi.tar.gz
  tar -tvzf genomes_chr_hdr.tar.gz

To extract the files, execute the following commands:

  cd genomedb
  tar -xvzf genomes_chr_NC_gi.tar.gz
  tar -xvzf genomes_chr_hdr.tar.gz

The default genome root location (variable genome_root_dir) for conversion programs and scripts is the following:

  genome_root_dir=../genomedb

which corresponds to the root directory of the genome assembly data, and is the genome directory used for the following application examples.

The genome root directory can be changed via the command line option -d <genome-root-dir> in all bash wrapper scripts (as described below). 

The PWM for the CTCF transcription factor that will be used in our example is the chen10_ctcf.mat file in the examples sub-directory. 

Upon installation, all PWMScan-specific binary files are located in the bin sub-directory, so we define:

  bin_dir=../bin


Analysis Pipeline using two different scanning methods
==============================================================================

1) Calculate the matrix score corresponding to p-value=0.00001 (10-5)
------------------------------------------------------------------------------

  ../bin/matrix_prob -e 0.00001 --bg "0.29,0.21,0.21,0.29" chen10_ctcf.mat
  SCORE :   1128  PERC :  96.90%

There are two basic methods to scan the genome with a PWM and a cut-off/p-value:

    1- Use a fast string-matching algorithms, such us Bowtie, as follows:
        - Given a PWM and a cut-off or p-value, generate all possible matches that represent the PWM with a score greater or equal to the cut-off;
        - Map the list of all possible PWM matches to a reference genome or a set of DNA sequences of interest using Bowtie. 
    2- Use matrix_scan, a C program using a conventional search algorithm that has been optimized for rapid score computation and drop-off strategy.

If you are interested in using the first approach, you should read Sections 2), 3) and 4), whereas for using matrix_scan please refer to Sections 2) and 5). 


2) Generate the PWM Score Cumulative Distribution 
------------------------------------------------------------------------------

The cumulative score distribution is used to assign a p-value to a PWM match score.

The matrix_prob program is used to compute the cumulative score distribution:

  ../bin/matrix_prob --bg "0.29,0.21,0.21,0.29" chen10_ctcf.mat > chen10_ctcf_co1128_scoretab.txt


3)  Generate a list of all tags that represent all possible PWM matches 
   (with minimal score of 1128)
------------------------------------------------------------------------------

This step is only necessary if fast string-matchig tools (such as Bowtie) are used to scan the genome.

  ../bin/mba -c 1128  chen10_ctcf.mat > chen10_ctcf_co1128.dat

Convert the tag list into FASTA format (for Bowtie):

  awk '{print ">"$2"\n"$1}' chen10_ctcf_co1128.dat > chen10_ctcf_co1128_taglist.fa

The reason for generating a match list in FASTA format is that we want to carry the match score over to the next steps (see below).


4) Map the tags representing the PWM to hg19
------------------------------------------------------------------------------
 PWMScan uses Bowtie to map all matching tags to the hg19 genome assembly.

    1- Bowtie

    # Let's define GENOME_DIR as the genome root directory: 

    GENOME_DIR = ../genomedb
    chr_NC_PATH = GENOME_DIR 

    # Let's define BOWTIE_DIR as the directory containing the Bowtie indexed genomes:

        BOWTIE_DIR = GENOME_DIR/bowtie
    
    # The h_sapiens_hg19 tag identifies the Bowtie index files linked to the hg19 human assembly

    # To build the h_sapiens_hg19 index file for the hg19 assembly, please follow the instructions given in the last Section of this README text file (How to build Bowtie index files).

    # The command to map the list of PWM tags to hg19 is the following:

    bowtie -l 19 -n0 -a BOWTIE_DIR/h_sapiens_hg19 -f chen10_ctcf_co1128_taglist.fa --un unmapped.dat > chen10_ctcf_co1128_bowtie.out

    # It takes of the order of 12-15 seconds for a full scan of the hg19 genome assembly (with the exclusion of the mitochondrial chromosome). 
      The total number of hits is 143597.

    # Example:
    bowtie --threads 4 -l 19 -n0 -a ../genomedb/bowtie/h_sapiens_hg19 -f chen10_ctcf_co1128_taglist.fa --un unmapped.dat > chen10_ctcf_co1128_bowtie.out

    # Convert the Bowtie output to BEDdetail via the following command:

    sort -s -k3,3 -k4,4n chen10_ctcf_co1128_bowtie.out | ../bin/bowtie2bed -s hg19 -l 19 -i chr_NC_PATH | awk 'BEGIN { while((getline line < "chen10_ctcf_co1128_scoretab.txt") > 0 ) {split(line,f," "); pvalue[f[1]]=f[2]} close("chen10_ctcf_co1128_scoretab.txt")} {print $1"\t"$2"\t"$3"\t"$4"\t"$5"\t"$6"\t""chen10_ctcf""\t""P-value="pvalue[$5]}' > chen10_ctcf_co1128_bowtie.bed

    # Example (use default chr_NC_PATH):
    sort -s -k3,3 -k4,4n chen10_ctcf_co1128_bowtie.out | ../bin/bowtie2bed -s hg19 -l 19 -i ../genomedb | awk 'BEGIN { while((getline line < "chen10_ctcf_co1128_scoretab.txt") > 0 ) {split(line,f," "); pvalue[f[1]]=f[2]} close("chen10_ctcf_co1128_scoretab.txt")} {print $1"\t"$2"\t"$3"\t"$4"\t"$5"\t"$6"\t""chen10_ctcf""\t""P-value="pvalue[$5]}' > chen10_ctcf_co1128_bowtie.bed

    # Note: to convert Bowtie output to BED format, we use the chr_hdr file for assembly hg19.
    # By default, the chr_hdr file is located at chr_NC_PATH/hg19 = ../genomedb/hg19.
    chr_NC_PATH can be changed by using option -i.
    # Also note that the bowtie ouput reports the match score in the first field so that it can be easily retrieved.

    # Run the Bowtie-based pipeline

    # Bowtie can read input files from stdin. You should specify "-" for stdin.
    In such case, you can run the entire pipeline as follows:

    ../bin/mba -c 1128 chen10_ctcf.mat | awk '{print ">"$2"\n"$1}' | bowtie --threads 4 -l 19 -n0 -a ../genomedb/bowtie/h_sapiens_hg19 -f - --un unmapped.dat | sort -s -k3,3 -k4,4n | ../bin/bowtie2bed -s hg19 -l 19 -i ../genomedb | awk 'BEGIN { while((getline line < "chen10_ctcf_co1128_scoretab.txt") > 0 ) {split(line,f," "); pvalue[f[1]]=f[2]} close("chen10_ctcf_co1128_scoretab.txt")} {print $1"\t"$2"\t"$3"\t"$4"\t"$5"\t"$6"\t""chen10_ctcf""\t""P-value="pvalue[$5]}' > chen10_ctcf_co1128_bowtie.bed


5) Using matrix_scan
------------------------------------------------------------------------------
    # Let's define GENOME_DIR as the genome root directory: 

    GENOME_DIR = ../genomedb
    chr_NC_PATH = GENOME_DIR 

    # We define as ASSEMBLY_DIR the sub-directory containing all the hg19 chromosome FASTA files:

    ASSEMBLY_DIR = GENOME_DIR/hg19 

    # The command to scan the hg19 genome against the chen10 CTCF PWM is the following:

    cat ASSEMBLY_DIR/chrom*.seq | ../bin/matrix_scan -m chen10_ctcf.mat -c 1128 | sort -s -k1,1 -k2,2n -k6,6 > chen10_ctcf_co1128_matrix_scan.out

    It takes of the order of 30 seconds to scan the entire genome. The total number of hits is 143597 (if we exclude the mitochondrial chromosome) in agreement with the tag mapping methods.
    Note that matrix_scan reports the match score in the fifth field of its output.
    Also note that matrix_scan parses the sequence FASTA header assuming that the sequence identifier is of the following type:

    word|Accession|Accession
    Ex:
    chr|NC_000004|NC_000004.11 

    # Parallelization of matrix_scan

    To improve performance, expecially for high p-values, we can run matrix_scan in parallel (on each chromosome file) using a simple python script called matrix_scan_parallel.py that dispatches the jobs to available CPU-cores. In this way, matrix_scan competes with the Bowtie-based approach.
    
    # Example:

    cat ../genomedb/hg19/chrom*.seq | ../bin/matrix_scan -m chen10_ctcf.mat -c 1128 | sort -s -k1,1 -k2,2n -k6,6 > chen10_ctcf_co1128_matrix_scan.out

    To use matrix_scan_parallel.py, type the following command:

    python ../bin/matrix_scan_parallel.py -m chen10_ctcf.mat -s "$(ls ../genomedb/hg19/chrom*.seq|grep -v chromMt)" -c 1128 -p 15 | sort -s -k1,1 -k2,2n -k6,6 > chen10_ctcf_co1128_matrix_scan.out  
 
    The -p option is used to set the maximum number of parallel processes for execution of matrix_scan. The above example takes only about 10-15 seconds to complete the task in agreement with the Bowtie performance. 

    # Convert the matrix scan output to BEDdetail by issuing the following command:

    ../bin/mscan2bed -s hg19 -i ../genomedb chen10_ctcf_co1128_matrix_scan.out | awk 'BEGIN { while((getline line < "chen10_ctcf_co1128_scoretab.txt") > 0 ) {split(line,f," "); pvalue[f[1]]=f[2]} close("chen10_ctcf_co1128_scoretab.txt")} {print $1"\t"$2"\t"$3"\t"$4"\t"$5"\t"$6"\t""chen10_ctcf""\t""P-value="pvalue[$5]}' > chen10_ctcf_co1128_matrix_scan.bed

    # Note : to convert matrix_scan output to BED format, we use the chr_NC_gi file for assembly hg19.
    # By default, the chr_NC_gi is located at chr_NC_PATH/hg19 = ../genomedb/hg19.
      chr_NC_PATH can be changed by using the <-i> option.

    # To run the entire pipeline:

    cat ../genomedb/hg19/chrom*.seq | ../bin/matrix_scan -m chen10_ctcf.mat -c 1128 | sort -s -k1,1 -k2,2n -k6,6 | ../bin/mscan2bed -s hg19 -i ../genomedb | awk 'BEGIN { while((getline line < "chen10_ctcf_co1128_scoretab.txt") > 0 ) {split(line,f," "); pvalue[f[1]]=f[2]} close("chen10_ctcf_co1128_scoretab.txt")} {print $1"\t"$2"\t"$3"\t"$4"\t"$5"\t"$6"\t""chen10_ctcf""\t""P-value="pvalue[$5]}' > chen10_ctcf_co1128_matrix_scan.bed 

    # Using parallelization:

    python ../bin/matrix_scan_parallel.py -m chen10_ctcf.mat -s "$(ls ../genomedb/hg19/chrom*.seq|grep -v chromMt)" -c 1128 -p 15 | sort -s -k1,1 -k2,2n -k6,6 | ../bin/mscan2bed -s hg19 -i ../genomedb | awk 'BEGIN { while((getline line < "chen10_ctcf_co1128_scoretab.txt") > 0 ) {split(line,f," "); pvalue[f[1]]=f[2]} close("chen10_ctcf_co1128_scoretab.txt")} {print $1"\t"$2"\t"$3"\t"$4"\t"$5"\t"$6"\t""chen10_ctcf""\t""P-value="pvalue[$5]}' > chen10_ctcf_co1128_matrix_scan.bed


Shell (bash) Wrappers to execute the analysis pipeline
==============================================================================

 These shell wrapper scripts have been written to make it easier to run the whole PWMScan pipeline, irrespecitve of what method one chooses.
 They require to specify the matrix file containing the integer PWM, the p-value, the path to the assembly or index files, and the UCSC assembly name for the genomes (e.g. hg19, hg38, mm9 etc.).
 Optional parameters are the overlapping matches flag (-o), the forward scanning (-f) option, the background base composition (-b), the parallel execution (-p) and write to file (-w) flags.
 If the '-o' option is specified, overlapping matches are retained. The '-f' option activates forward scanning. If the '-p' option is set, the matrix_scan program execution is done by parallel dispatch. If the -w option is set, the results are written to file, else output goes to STDOUT.

 The output file is a match list in BEDdetail format.
 
 The pwm_scan script scans a genome with a PWM using either Bowtie or matrix_scan depending on both the p-value and the matrix length.
 For high p-values and long motifs, the Bowtie-based strategy becomes inefficient and the matrix_scan-based approach is the fastest one. 
 The pwm_bowtie_wrapper script implements the Bowtie-based pipeline whereas the pwm_mscan_wrapper script uses the matrix_scan-based approach.

 The pwm_scan_ucsc and pwm_mscan_wrapper_ucsc scripts are just adaptations of the original ones in order to deal with chromosome files downloaded from UCSC. 

 The pwmlib_scan script scans a genome with a collection of PWMs coming from a database.
 Accepted motif database formats are the MEME libraries as well as the motif libraries (both log-odds and letter-probability formats) provided by the PWMScan Web interface.

    NOTE: The path to all the PWMScan binaries and scripts is defined by the bin_dir variable and is hard-coded in all bash scripts, and is changed upon installation via the Makefile, so that all installed scripts in bin_dir have the correct binary pathname.


------------------------------------------------------------------------------

 A few remarks on the genome root directory.

    # The <genome-root-dir> input argument is the root directory of the genome files (the bowtie index files, the FASTA chromosome files used by matrix_scan, and the assembly tables used to convert the PWMScan pipeline output to BED format).
    # <genome-root-dir> is supposed to have two main sub-directory types, the /bowtie sub-directory where the Bowtie index files are stored, and the /<assembly> sub-directory for storing assembly-specific chromosome files.
    # 
    # In our setting, <genome-root-dir> is set to ../genomedb.
    # The chrNC_dir variable (used for locating the chr_NC_gi/chr_hdr tables) is set to chrNC_dir=<genome-root-dir>.
 
------------------------------------------------------------------------------

    1) Bowtie wrapper script

    # Usage:
    pwm_bowtie_wrapper -m <matrix-file> -e <p-value> -d <genome-root-dir> -s <assembly[hg19|mm9|..]> [options]
    - version 1.1.6
    where options are:
         -b <bg_comp>   Background model (residue priors), e.g. : 0.29,0.21,0.21,0.29
                        [def=predefined for some common genome assemblies, else set to uniform]
         -f             Scan sequences in forward direction [def=bidirectional]
         -o             Allow for overlapping matches [def=non-overlapping matches]
         -w             Write output to file [def=STDOUT]

    # Example:

    ../bin/pwm_bowtie_wrapper -m chen10_ctcf.mat -e 0.00001 -d ../genomedb -s hg19 -o -w
   
    # or:

    bash ../bin/pwm_bowtie_wrapper -m chen10_ctcf.mat -e 0.00001 -d ../genomedb -s hg19 -o -w

    # List of matches (BED format) : chen10_ctcf_co1128_bowtie.bed
    # Total number of PWM matches  : 143597
------------------------------------------------------------------------------

    2) Matrix Scan (matrix_scan) wrapper script

    # Usage:
    pwm_mscan_wrapper -m <matrix-file> -e <p-value> -d <genome-root-dir> -s <assembly[hg19|mm9|..]> [options]
    - version 1.1.6
    where options are:
         -b <bg_comp>   Background model (residue priors), e.g. : 0.29,0.21,0.21,0.29
                        [def=predefined for some common genome assemblies, else set to uniform]
         -f             Scan sequences in forward direction [def=bidirectional]
         -o             Allow for overlapping matches [def=non-overlapping matches]
         -w             Write output to file [def=STDOUT]
         -p             Distribute matrix_scan on multiple CPU-cores [def=non-parallel processing]

    # Example:

    ../bin/pwm_mscan_wrapper -m chen10_ctcf.mat -e 0.00001 -d ../genomedb -s hg19 -o -p -w
 
    # or:

    bash ../bin/pwm_mscan_wrapper -m chen10_ctcf.mat -e 0.00001 -d ../genomedb -s hg19 -o -p -w

    # List of matches (BED format) : chen10_ctcf_co1128_matrix_scan.bed
    # Total number of PWM matches  : 143597
------------------------------------------------------------------------------

    3) PWM Scan (pwm_scan) wrapper script

    # Usage:
    pwm_scan -m <matrix-file> -e <p-value> -d <genome-root-dir> -s <assembly[hg19|mm9|..]> [options]
    - version 1.1.6
    where options are:
         -b <bg_comp>   Background model (residue priors), e.g. : 0.29,0.21,0.21,0.29
                        [def=predefined for some common genome assemblies, else set to uniform]
         -f             Scan sequences in forward direction [def=bidirectional]
         -o             Allow for overlapping matches [def=non-overlapping matches]
         -w             Write output to file [def=STDOUT]
         -p             Distribute matrix_scan on multiple CPU-cores [def=non-parallel processing]

    # Examples:

    bash ../bin/pwm_scan -m chen10_ctcf.mat -e 0.00001 -d ../genomedb -s hg19 -o -w

    # List of matches (BED format) : chen10_ctcf_co1128_bowtie.bed

    # Or if we set the <parallel> option:

    bash ../bin/pwm_scan -m chen10_ctcf.mat -e 0.00001 -d ../genomedb -s hg19 -o -w -p

    # List of matches (BED format) : chen10_ctcf_co1128_matrix_scan.bed

    # Also try the following commands (with p-value=10^6):

    bash ../bin/pwm_scan -m chen10_ctcf.mat -e 0.000001 -d ../genomedb -s hg19 -w -p

    # List of matches (BED format) : chen10_ctcf_co1549_bowtie.bed

    bash ../bin/pwm_scan -m stat1.mat -e 0.000001 -d ../genomedb -s hg19 -o -w -p

    # List of matches (BED format) : stat1_co1731_bowtie.bed

    # N.B. 
    # If the <parallel> option is set (default), matrix_scan is run in parallel by splitting the processing of the entire genome on multiple processes for each chromosome file in parallel.
    # Of course it only works on a multi-core processor machine.
------------------------------------------------------------------------------

    4) PWMlib Scan (pwmlib_scan) wrapper script

    # Usage:
    pwmlib_scan -l <pwmlib-file> -e <p-value> -d <genome-root-dir> -s <assembly[hg19|mm9|..]> [options]
    - version 1.1.6
    where options are:
         -b <bg_comp>   Background model (residue priors), e.g. : 0.29,0.21,0.21,0.29
                        [def=predefined for some common genome assemblies, else set to uniform]
         -g             PWM format [def=logodds]. Accepted formats are : meme, logodds, and lpm
         -f             Scan sequences in forward direction [def=bidirectional]
         -o             Allow for overlapping matches [def=non-overlapping matches]
         -w             Write output to file [def=STDOUT]
         -p             Distribute matrix_scan on multiple CPU-cores [def=non-parallel processing]
         -v             Verbose mode and keep PWM tmp files

    Scan a genome assembly with a PWM collection. Accepeted PWM library formats are the MEME format
    (<meme>), and the MEME-like log-odds and letter-probability formats (<logodds> and <lpm>).
    Description of these formats is provided at http://ccg.vital-it.ch/pwmtools/pwmlib.html.

    # Example:

    # Scan the human genome hg19 with the Isakova 2017 collection (lpm format)

    bash ../bin/pwmlib_scan -l ../pwmlibs/isakova2017_human_matrix_probs.mat -g lpm -e 0.00001 -d ../genomedb -s hg19 -p 2>pwmlib_scan_log.txt | sort -s -k1,1 -k2,2n > isakova2017_human_hg19_pwmscan.bed

    # On a linux multi-core platform with 48 CPU cores, the whole process takes about 10 minutes for scanning the entire human genome with a total of 86 matrices.
------------------------------------------------------------------------------


Shell (bash) Wrapper for matrix conversion
==============================================================================

    The pwm_convert bash script is used to convert JASPAR, TRANSFAC, PFM, LPM and real PWM formats to integer log-odds format.

    # Usage: pwm_convert <matrix-file> -f=<format [jaspar|transfac|lpm|pfm|real]>

      OPTIONS:
              -h                  show help message
              -b=<bg-freq-file>   bg nucleotide composition file
              -c=<pseudo-cnt>     pseudo-count fraction [def=0]
              -m=<minscore>       minimal score value [def=-10000]
              -n=<log-scaling>    log scaling factor [def=100]
              -s=<mult>           matrix multiplication factor (for real PWMs) [def=100]

              Convert JASPAR, TRANSFAC, PFM, LPM and real PWM formats to integer log likelihoods (log-odds) format.

     By default, the prior background nucleotide frequencies are set to 0.25.

     # Examples

     # Convert JASPAR to log-odds:
    
     bash ../bin/pwm_convert stat1_jaspar.mat -f=jaspar  2>/dev/null

     # If we want to use different backgroung nucleotide frequencies:

     bash ../bin/pwm_convert stat1_jaspar.mat -f=jaspar -b=bg_probs_2.txt 2>/dev/null
   
     # Where the bg_probs_2.txt file includes the prior bg frequencies, defined as follows:

       A  0.29
       C  0.21
       G  0.21
       T  0.29

     # Convert TRANSFAC to log-odds:

     bash ../bin/pwm_convert statx_transfac.mat -f=transfac -c=0.0000001 2>/dev/null

     # Convert PFM (Position Frequency Matrix) to to log-odds:

     bash ../bin/pwm_convert stat1_pfm.mat -f=pfm 2>/dev/null 
------------------------------------------------------------------------------


How to compute the background base composition of a set of DNA sequences
==============================================================================

    The program seq_extract_bcomp can be used to extract BED regions from a set of FASTA-formatted sequences or, optionally, compute the base composition of a set of DNA sequences. When computing the base composition, the seq_extract_bcomp program can either extract BED-defined regions or use the FASTA input as a whole (if the BED file is not given).

    # Usage: seq_extract_bcomp [options] [-f <bed_file>] [-s <species>] [<] [<fasta_file>|stdin]

      where options are:
        -d          Print debug information
        -h          Show this help text
        -c          Compute base composition [def=on forward strand]
        -b          Compute base composition on both strand [-c is required]
        -r          Compute base composition on reverse strand [-c is required]
        -i <int>    AC index (after how many pipes |) for FASTA header [2]
        -p <path>   Use <path> to locate the chr_NC_gi file [if BED file is given]
                    [default is: $HOME/db/genome]

	Extract BED regions from a set of FASTA-formatted sequences.
	The extracted sequences are written to standard output.
	Optionally (-c), the program computes and only outputs the base composition,
	in which case sequences can be extracted directly from the FASTA input or,
	as for the extraction mode, specified in a BED file. If the BED file is not
	given, the <species> argument is not required. If base composition mode is set
	(-c option), the program can optionally compute it on both strands (-b option)
	for strand-symmetric base composition or on the reverse strand only (-r).

    Here are a few examples of use.

    # Extract STAT1 peak regions (16158 peaks) defined in the BED file Encode_Helas3Stat1_peaks.bed from the human assembly hg19

      cat ../genomedb/hg19/chrom[^Mt]*.seq | ../bin/seq_extract_bcomp -p ../genomedb -f Encode_Helas3Stat1_peaks.bed -s hg19 > seq_extract_stat1_peaks.fa
  
    # This command takes about 20 seconds.

    # Compute the base composition of the STAT1 peak regions on the forward strand

      ../bin/seq_extract_bcomp -c seq_extract_stat1_peaks.fa 

      Total Sequence length: 4863558
      0.2445,0.2547,0.2544,0.2464
 
    # Compute the base composition of the STAT1 peak regions on both strands
  
      ../bin/seq_extract_bcomp -cb seq_extract_stat1_peaks.fa

      Total Sequence length: 4863558
      0.24,0.26,0.26,0.24
------------------------------------------------------------------------------


How to build Bowtie index files
==============================================================================

    Let's define BOWTIE_DIR as the directory containing the bowtie indices and ASSEMBLY_DIR as the one where the FASTA chromosome files for a specific assembly (e.g. hg19) are stored.

    Go to the BOWTIE_DIR directory, and concatenate all chromosome files into a sequence file (h_sapiens_hg19.fa) that includes the entire hg19 genome assembly:

      cd BOWTIE_DIR
      ls -1 -v ASSEMBLY_DIR/chrom*.seq | xargs cat > h_sapiens_hg19.fa

    Example:

      cd ../genomedb/bowtie
      ls -1 -v ../hg19/chrom*.seq | xargs cat > h_sapiens_hg19.fa

    You then execute the following command:

      bowtie-build -f h_sapiens_hg19.fa h_sapiens_hg19
