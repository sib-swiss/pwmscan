#!/usr/bin/perl 

$genome=$ARGV[0];
$matrix=$ARGV[1];
$cutoff=$ARGV[2];

if (@ARGV == 0) {
    print "Usage: matrix_scan_parallel.pl <genome chrom files> <PWMW file> <cut-off>\n
           where:
             <genome chrom files> is a list of genome chunk files in FASTA format
             <PWMW file>          is a file containing the PWM in integer log-odds format
             <cut-off>            is the cut-off value expressed as raw integer score.\n
      The program ouputs (to <STDOUT>) a list of PWM matches in BED-like format.\n\n";
    exit(0);
}

$pid=$$; $temp_dir="/tmp/matrix_scan_parallel_$pid"; mkdir $temp_dir;

@chunk=(); open(INP,"ls -1 $genome|"); while(<INP>) {chomp; push @chunk, $_} 
close(INP); open(OUT, ">$temp_dir/matrix_scan.sh"); $i=0; 
while($chrs=shift @chunk) {$i++; 
   $outfile="$temp_dir/chunk" . sprintf("%02d", $i);
   print OUT "cat $chrs | /local/bin/matrix_scan -m $matrix -c $cutoff > $outfile &\n"
   }
print OUT "#\nwait\n";
print OUT "cat  $temp_dir/chunk*\n";
print OUT "/bin/rm -r  $temp_dir\n";
close(OUT);
print `sh $temp_dir/matrix_scan.sh`;
