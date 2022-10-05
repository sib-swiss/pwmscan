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
#FIXME server and local/container do NOT give the same filterOverlaps output!!!
#FIXME what is "MA0137.3 STAT1" -> "Motif Name" ?
$DOCKER_CMD mba -c 1475 /work_dir/$INPUT_MATRIX | $DOCKER_CMD awk '{print ">"$2"\n"$1}' | $DOCKER_CMD bowtie --threads 4 -l11 -n0 -a /bowtie_dir/$GENOME -f - --un /work_dir/unmapped.dat | sort --parallel=5 -T $LOCAL_TMP -s -k3,3V -k4,4n -k2,2 | $DOCKER_CMD bowtie2bed -s $ASSEMBLY -l 11 -i /bowtie_dir | $DOCKER_CMD filterOverlaps -l11 | $DOCKER_CMD awk -v matrix="/work_dir/matrixScore_tab_${MYPID}.txt" 'BEGIN { while((getline line < matrix) > 0 ) {split(line,f," "); pvalue[f[1]]=f[2]} close(matrix)} {print $1"\t"$2"\t"$3"\t"$4"\t"$5"\t"$6"\t""MA0137.3 STAT1""\t""P-value="pvalue[$5]}' > $WORK_DIR/pwmscan_${ASSEMBLY}_${MYPID}.bed


# Convert to other formats (SGA/FPS)
#FIXME mscan_bed2sga needs the chr_NC_gi file related to the genome assembly
#FIXME chipcenter    needs the chr_size  file related to the genome assembly
#FIXME mscan_bed2sga: invalid option -- 'i' So cannot give the chr_NC_gi path!
$DOCKER_CMD mscan_bed2sga -i /bowtie_dir -f MA0137.3-STAT1 -s $ASSEMBLY /work_dir/pwmscan_${ASSEMBLY}_${MYPID}.bed | $DOCKER_CMD chipcenter -i /bowtie_dir/$ASSEMBLY -s 6 -c 9999999 | sort --parallel=5 -T $LOCAL_TMP -s -k1,1 -k3,3n -k4,4 > $WORK_DIR/pwmscan_${ASSEMBLY}_${MYPID}.sga
#mscan_bed2sga -f MA0137.3-STAT1 -s hg19 $workDir/$pwmscanFile.bed | chipcenter -s 6 -c 9999999 | sort --parallel=5 -T LOCAL_TMP -s -k1,1 -k3,3n -k4,4 > $workDir/$pwmscanFile.sga
#sga2fps.pl -f MA0137.3-STAT1 -s hg19 $workDir/$pwmscanFile.sga > $workDir/$pwmscanFile.fps


# Generate BED file for UCSC
#NOTE if not human or mouse (and few others) there is no track at UCSC!
#FIXME on the web the description is "motif matches" in the log but description "PWMScan motif matches" is produced!
$DOCKER_CMD sed '1s/^/track name=MA0137.3-STAT1 type=bedDetail description="PWMScan motif matches" color=0,0,102 url=""\n/' /work_dir/pwmscan_${ASSEMBLY}_${MYPID}.bed > $WORK_DIR/pwmscan_${ASSEMBLY}_${MYPID}_ucsc.bed ; gzip -9 $WORK_DIR/pwmscan_${ASSEMBLY}_${MYPID}_ucsc.bed

exit 0

