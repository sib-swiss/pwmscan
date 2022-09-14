#!/bin/bash

# Directories and Files
usage() { echo -e "Usage: $0 [-i input_matrix] [-b bowtie_dir] [-g genome_assembly] [-o output_dir]\n" 1>&2; exit 1; }
while getopts ":i:b:g:o:" option; do
    case "${option}" in
        i)
            i=${OPTARG}
            ;;
        b)
            b=${OPTARG}
            ;;
        g)
            g=${OPTARG}
            ;;
        o)
            o=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))
if [ -z "${i}" ] || [ -z "${b}" ] || [ -z "${g}" ] || [ -z "${o}" ]; then
    usage
fi

INPUT_MATRIX="${i}"
BOWTIE_DIR="${b}"
WORK_DIR="${o}"
GENOME="${g}"
ASSEMBLY=${g##*_}
MYPID=$BASHPID
LOCAL_TMP=/tmp

#NOTE Comment to use local (not in container) commands
#NOTE -i is required for docker to read piped stdin
DOCKER_CMD="docker run --rm -i --mount type=bind,source=`realpath $WORK_DIR`,target=/work_dir --mount type=bind,source=`realpath $BOWTIE_DIR`,target=/bowtie_dir sibswiss/pwmscan:1.1.9"


#TODO what to log in $WORK_DIR/pwmscan_${MYPID}.log???

# Matrix conversion
#FIXME web part contains extra lines: *Background letter frequencies (from dataset with add-one prior applied)*
$DOCKER_CMD pwmconvert.pl -c /work_dir/$INPUT_MATRIX 1>$WORK_DIR/pwmscan_${MYPID}_pwmconvert.log 2>/dev/null
echo 'FORMAT:' `tail -1 $WORK_DIR/pwmscan_${MYPID}_pwmconvert.log`


# PWM score conversion
#FIXME print an extra line: *SCORE :   1475 PERC :  99.64%*
$DOCKER_CMD matrix_prob -e 0.00001 -b 0.29,0.21,0.21,0.29 /work_dir/$INPUT_MATRIX 2>/dev/null
echo 'Generate Matrix Score Table:'
$DOCKER_CMD matrix_prob            -b 0.29,0.21,0.21,0.29 /work_dir/$INPUT_MATRIX 2>/dev/null >$WORK_DIR/matrixScore_tab_${MYPID}.txt


# Command pipeline
#FIXME where this *unmapped.dat* file comes from?
#FIXME bowtie2bed needs the chr_hdr file related to the genome assembly
#FIXME indentation broken and p-value missing in the final bed!
$DOCKER_CMD mba -c 1475 /work_dir/$INPUT_MATRIX | $DOCKER_CMD awk '{print ">"$2"\n"$1}' | $DOCKER_CMD bowtie --threads 4 -l11 -n0 -a /bowtie_dir/$GENOME -f - --un /work_dir/unmapped.dat | sort --parallel=5 -T $LOCAL_TMP -s -k3,3V -k4,4n -k2,2 | $DOCKER_CMD bowtie2bed -s $ASSEMBLY -l 11 -i /bowtie_dir | $DOCKER_CMD filterOverlaps -l11 | $DOCKER_CMD awk 'BEGIN { while((getline line < "/work_dir/matrixScore_tab_${MYPID}.txt") > 0 ) {split(line,f," "); pvalue[f[1]]=f[2]} close("/work_dir/matrixScore_tab_${MYPID}.txt")} {print $1"  "$2"    "$3"    "$4"    "$5"    "$6"    ""MA0137.3 STAT1""  ""P-value="pvalue[$5]}' > $WORK_DIR/pwmscan_${ASSEMBLY}_${MYPID}.bed
#mba -c 1475 /home/local/www-ccg/weekly/pwmfile_hg38_37886_39604 | awk '{print ">"$2"\n"$1}' | bowtie --threads 4 -l11 -n0 -a $bowtieDir/h_sapiens_hg38 -f - --un wwwtmp/unmapped.dat | sort --parallel=5 -T LOCAL_TMP -s -k3,3V -k4,4n -k2,2 | bowtie2bed -s hg38 -l 11 | filterOverlaps -l11 | awk 'BEGIN { while((getline line < "$workDir/matrixScore_tab_37886_39604.txt") > 0 ) {split(line,f," "); pvalue[f[1]]=f[2]} close("$workDir/matrixScore_tab_37886_39604.txt")} {print $1"  "$2"    "$3"    "$4"    "$5"    "$6"    ""MA0137.3 STAT1""  ""P-value="pvalue[$5]}' > $workDir/$pwmscanFile.bed

exit 0
