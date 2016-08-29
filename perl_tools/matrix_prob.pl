#!/usr/bin/perl

use List::Util qw[min max sum];
use Math::Round;
use POSIX qw(floor ceil);


my $usage = "USAGE:  matrix_prob.pl [options] <matrix file>

  Options: -bg <background base frequencies>  set of nucleotide freq: 0.29,0.21,0.21,0.29
           -e  <e-value>                      set e-value threshold  (eg. 0.00001)
           -p  <perc>                         set cut-off percentage (eg. 79)
           -s  <score>                        set cut-off score (integer value)

  Compute the match probability distribution for a given Position Weight Matrix (PWM).
  PWMs elements are integer numbers that often are calculated as log likelihoods (log-odds). 
  If the e-value threshold is set, the corresponding score and cut-off percentage are computed.
  If the cut-off percentage is set, the corresponding score and e-value are computed.
  If the cut-off score is set, the corresponding cut-off percentage and e-value are computed.\n\n";

my @p0 = (0.0,0.25,0.25,0.25,0.25);
my $fnmat = "";
my $pval = -1;
my $co_perc = -1;
my $co_score = -1;

# Process command line arguments.
 if (scalar(@ARGV) == 0) {
   printf(STDERR $usage);
   exit(1);
}

while (scalar(@ARGV) > 1) {
  $next_arg = shift(@ARGV);
  if ($next_arg eq "-bg") {
    @p0 = split /,/, shift(@ARGV);
    my $sum = sum(@p0);
    for($i=4; $i>0; $i--) {$p0[$i] = $p0[$i-1]/$sum}
    $p0[0]=0.0;
  } elsif ($next_arg eq "-e") {
    $pval = shift(@ARGV);
  } elsif ($next_arg eq "-p") {
    $co_perc = shift(@ARGV);
  } elsif ($next_arg eq "-s") {
    $co_score = shift(@ARGV);
  } else {
    print(STDERR "Illegal argument ($next_arg)\n");
    printf(STDERR $usage);
    exit(1);
  }
}
($fnmat) = @ARGV;

read_mat(); # output: @mat, $K (length)
process_mat(); # score range goes now from 0 to "offset
print STDERR "after process_mat : max = $max  offset = $offset\n";
if ($pval != -1) {
  print STDERR "set p-value : $pval\n";
}
if ($co_perc != -1) {
  print STDERR "set cut-off percentage : $co_perc%\n";
}
if ($co_score != -1) {
  print STDERR "set cut-off score : $co_score\n";
}

for($i=0; $i <=$max; $i++) {$p[$i]=0; $q[$i]=0}
$max=0;
for($j=1; $j<=4; $j++) {$p[$mat[1][$j]] += $p0[$j]}
$max += max(@{$mat[1]});


for($k=2; $k<=$K; $k++) {
   for($i=0; $i <=$max; $i++) {
      if($p[$i] ne 0) {for($j=1; $j<=4; $j++) {
         $q[$i+$mat[$k][$j]] += $p[$i]*$p0[$j]
         }}
      }
   $max += max(@{$mat[$k]});
   for($i=0; $i<=$max; $i++) {$p[$i]=$q[$i]; $q[$i]=0}
   }

$prob = 0;
for($i=$max; $i>=0; $i--) {
  if($p[$i] ne 0) {
    $prob += $p[$i]; $score=$i-$offset;
    $perc = $i/$max*100;
    #printf STDERR "-- PERC : %6.3f% COFF : %6.3f%\n",$perc, $co_perc;
    if ($pval == -1 and $co_perc == -1 and $co_score == -1) {
      #printf "%6i %10.8f %6.2f%\n", $score, $prob, $perc;
      printf "%6i %.2e %6.2f%\n", $score, $prob, $perc;
    } elsif ($pval != -1) {
      if ($prob >= $pval) {
        printf "SCORE : %6i\tPERC : %6.2f%\n", $score, $perc;
        exit (0);
      }
    } elsif ($co_perc != -1) {
      if ($perc <= $co_perc) {
        #printf STDERR "PERC : %6.2f% COFF : %6.2f%\n",$perc, $co_perc;
        #printf "SCORE : %6i\tPVAL : %10.8f\n", $score, $prob;
        printf "SCORE : %6i\tPVAL : %.2e\n", $score, $prob;
        exit (0);
      }
    } elsif ($co_score != -1) {
      if ($score <= $co_score) {
        #printf "PERC : %6.2f%\tPVAL : %10.8f\n", $perc, $prob;
        printf "PERC : %6.2f%\tPVAL : %.2e\n", $perc, $prob;
        exit (0);
      }
    }
  }
}

sub read_mat {
open(MAT, "<$fnmat") || die("Can't open $fnmat.\n");
$k=1;
while(<MAT>) {
   chomp; 
   next if (/^>/);
   if(/([-0-9]+)\s+([-0-9]+)\s+([-0-9]+)\s+([-0-9]+)/) {
   $mat[$k][1]=$1;
   $mat[$k][2]=$2;
   $mat[$k][3]=$3;
   $mat[$k][4]=$4;
   $k++;
   }
   $K=$k-1;
}
   print STDERR "matrix length : $K\n";
close(MAT);
}

sub process_mat {
$max=0; $offset=0;
for($k=1; $k<=$K; $k++) {
   $min=min(@{$mat[$k]});
   $tmp_max=max(@{$mat[$k]});
   for($i=1; $i<=4; $i++) {$mat[$k][$i] -= $min}
   $max += $tmp_max-$min;;
   $offset -= $min;
   }
}
