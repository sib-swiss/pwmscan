#!/usr/bin/env perl

use warnings;
use diagnostics;
use Getopt::Long qw(:config no_ignore_case);

my %opt;
my @options = ("help", "h", "t=s", "T", "s=s", "f=s");

# initialize default options

my $target   = 1.0;
my $totalIC  = 0;
my $s        = 0.001;
my $format   = "%6.4f";

# get options from command line

if( ! GetOptions( \%opt, @options ) ) { &Usage(); }
&Usage() if defined($opt{'help'}) || defined($opt{'h'});

if($opt{'t'}  ne '') {$target = $opt{'t'}};
if($opt{'T'}  ne '') {$totalIC = 1};
if($opt{'s'}  ne '') {$s = $opt{'s'}};
if($opt{'f'}  ne '') {$format = $opt{'f'}};
#FIXME if($opt{'nH'} ne '') {$nH_flag = 1};

# major loop over input file

$k=0; my @mat; my $pos_flag = 0; while(<STDIN>) {

   if   (/^\s*(\d+)\s+([0-9\.]+)\s+([0-9\.]+)\s+([0-9\.]+)\s+([0-9\.]+)/) {
      $k++; $mat[$k][1]=$2; $mat[$k][2]=$3; $mat[$k][3]=$4; $mat[$k][4]=$5; $pos_flag=1}
   elsif(/^\s*([0-9\.]+)\s+([0-9\.]+)\s+([0-9\.]+)\s+([0-9\.]+)/) {
      $k++; $mat[$k][1]=$1; $mat[$k][2]=$2; $mat[$k][3]=$3; $mat[$k][4]=$4; $pos_flag=0}
   elsif($k > 0) {
      adjust_ppm();
      @mat=(); $k=0; $e=0; print}
   else {print}
   }

if($k > 0) {
   adjust_ppm()}

sub adjust_ppm {
   @mat = normalize_ppm(@mat); $ic = comp_ic(@mat); $ic_max = ic_max(@mat);
   if($totalIC) {$T=$target/$k} else {$T=$target}
   if($ic_max <= $T) {if($totalIC) {printf STDERR
      "Target total IC %6.4f higher than maximum IC for input matrix: %6.4f - matrix skipped.\n", $T*$k, $ic_max*$k}
                     else         {printf STDERR
      "Target per position IC %6.4f higher than maximum IC for input matrix: %6.4f - matrix skipped.\n", $T, $ic_max}}
   else {
      if($totalIC) {printf STDERR "Initial toal IC %6.4f, ", $ic*$k}
      else         {printf STDERR "Initial per position IC %6.4f, ", $ic}
      if($T > $ic) {
         while($ic < $T) {$e += $s;
            @mat = skew($e, @mat);
            $ic = comp_ic(@mat); # printf "%4.2f %6.4f %6.4f\n", $e, $target, $ic;
            }
         }
      else {
         while($ic > $T) {$e -= $s;
            @mat = skew($e, @mat);
            $ic = comp_ic(@mat); # printf "%4.2f %6.4f %6.4f\n", $e, $target, $ic;
            }
         }
      $ic = comp_ic(@mat);
      if($totalIC) {printf STDERR "final total IC %6.4f\n", $ic*$k}
      else         {printf STDERR "final per position IC %6.4f\n", $ic}
      print_ppm($pos_flag)
      }
   }

sub print_ppm {my $pos_flag = shift @_; $L = @mat-1;
   if($pos_flag) {for($i=1; $i<=$L; $i++) {print "$i";
      for($j=1; $j<=4; $j++) {printf "\t$format", $mat[$i][$j]}
      print "\n"}}
   else          {for($i=1; $i<=$L; $i++) {printf "$format", $mat[$i][1];
      for($j=2; $j<=4; $j++) {printf "\t$format", $mat[$i][$j]}
      print "\n"}}
   }

sub comp_ic {my @pwm = @_;
   my $l = @pwm-1; my $ic = 0;
   for($i=1; $i<=$l; $i++) { for($j=1; $j<=4; $j++) {
      if($pwm[$i][$j] ne 0) {$ic += $pwm[$i][$j]*log($pwm[$i][$j]*4)}}}
   $ic = $ic/($l*log(2));
   return $ic}

sub normalize_ppm {my @pwm = @_;
   my $l = @pwm-1;
   for($i=1; $i<=$l; $i++) {$sum=0;
      for($j=1; $j<=4; $j++) {$sum += $pwm[$i][$j]}
      for($j=1; $j<=4; $j++) {$pwm[$i][$j] = $pwm[$i][$j]/$sum}}
   return @pwm}

sub ic_max{my @pwm = @_;
  my $l = @pwm-1; my $ic = 0;
  for($i=1; $i<=$l; $i++) {$n=0; $pmax=0;
     for($j=1; $j<=4; $j++) {if($pwm[$i][$j]  > $pmax) {$pmax = $pwm[$i][$j]}}
     for($j=1; $j<=4; $j++) {if($pwm[$i][$j] == $pmax) {$n++}}
     $ic += log(4/$n)}
     $ic = $ic/($l*log(2));
     return $ic
  }

sub skew {my $e = shift @_; @pwm = @_; my $l = @pwm-1; $e = exp($e); # print "skew: $l $e\n";
   for($i=1; $i<=$l; $i++) {$sum = 0;
      for($j=1; $j<=4; $j++) {$pwm[$i][$j] = $pwm[$i][$j]**$e; $sum += $pwm[$i][$j]}
      for($j=1; $j<=4; $j++) {$pwm[$i][$j] = $pwm[$i][$j]/$sum}}
      return @pwm
   }

sub Usage {
   print STDERR <<"_USAGE_";

   logo_rescale.pl [options] <stdin>

      where options are:
      -h|--help       Show this stuff
      -t target-IC    Set target IC
      -T              Target IC is total IC, default is average per position IC
      -s (step-size   Log step size for changing exponent, use lower values for
                      more precise IC match (default 0.001)
      -f format       C-style output format for position-specific probability
                      matrix (PPM) (default %6.4f)

   Input format and processing: Base frequency (PFMs) or probability matrices
   (PPMs) are acceptable. Numbers must be non-negative integers or decimals.
   Lines correspond to positions, columns to bases. Lines defining motif
   positions must consist of 4 or 5 numbers. If five numbers are present, the
   first number is an integer indicating the motif position. Numbers are
   white-space delimited. Lines may contain white-space characters at the
   beginning.

   The input file may contain multiple motifs. Each motif consists of a block of
   lines conforming to the motif position format. Additional lines between PFM
   or PPM blocks are considered header or metadata lines and are transferred
   to output as is.

   Probabilites of the output matrix (P) relate to the probabilites of the
   input matrix (p) as follows (R notation)

      P[i,j] = = p[i,j]**e/sum(p[i,]**e)

   The program increases or decrease e untill the target IC is reached,
   starting with value exp(s) and incrementing or decrementing step size s
   in each iteration.
   Example: if the target IC is higher than the IC of the input PPM, and
   the step size s is set to  0.01 then the program tries first
   s = exp(0.01) ~ 1.01. Next it will try exp(0.02) ~ 1.02 and so on until
   the requested IC value is reached.

   Note that for each input matrix, there is a maximum IC value that can be
   reached. This value depends on the length of the matrix, but also on whether
   some lines contain identical maximal base frequency values. If the latter
   is the case then an IC of 2 cannot be reached for that motif position. If
   the target IC value requested by the user cannot be reached, the program
   issues and error message and skips the matrix.

_USAGE_
    exit(1);
}

