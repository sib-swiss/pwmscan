/*
  mba.c 

  Matrix Branch-and-bound Algorithm for generating sequences
  from a profile weight matrix and a cut-off value

  Giovanna Ambrosini, EPFL, Giovanna.Ambrosini@epfl.ch

  Copyright (c) 2014 EPFL and Swiss Institute of Bioinformatics.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#ifdef DEBUG
#include <mcheck.h>
#endif

#define NUCL  4
#define LMAX  100

typedef struct _options_t {
  unsigned int count;
  int cutOff;
  int help;
  int debug;
} options_t;

/* We use a tree structure to represent all possible L-mers or
   strings of length L.
   Each vertex of the tree is represented as the paring of an array s
   and a level i (s,i).
   Traversing the complete tree is implemented in the next_vertex 
   function, below. We begin at level 0 (the root) and then consider
   each of its NUCL=4 children in order. For each child, we again
   consider each of its NUCL children and so on.
   At each vertex we calculate a bound - the partial score and a 
   drop-off value for the current score - and then decide whether
   or not to branch out further.
*/   

static options_t options;

static char nucleotide[] = {'A','C','G','T'};

void
nucleotide_string(int *s, char *string)
{
  int *n = s;
  char *p = string;

  for (; *n ;) {
    *p++ = nucleotide[*n - 1];
    n++;
  }
  //*p = 0;
}

int 
find_max(int *a, int n)
{
  int i, max;

  max = a[0];
  for (i = 1; i < n; i++) 
    if (a[i] > max)
      max = a[i];
  return max;
}

void
drop_off_init(int **profile, int len, int *doff)
{
  int scores[NUCL] = {0};
  int sum = 0;
  int i, k;

  if (options.debug)
    printf("\ndrop_off_init:\n");
  int *max = (int *)calloc(len, sizeof(int));
  if (max == NULL) {
    fprintf(stderr, "Out of memory: %s(%d)\n",strerror(errno), errno); 
    exit(1);
  }
  for (i = 0; i < len; i++) {
    for (k = 0; k < NUCL; k++) {
      scores[k] = profile[k][i];
      if (options.debug)
        printf("scores:[%d]  %d\n", k, scores[k]);
    }
  if (options.debug)
    printf("\nscores and max scores per position\n");
    /* Find max score and store it into max array */
    max[i] = find_max(scores, NUCL);
    if (options.debug)
      printf("max[%d]: %d\n", i, max[i]);
  }
  if (options.debug)
    printf("\ndrop-off values per position/level\n");
  for (i = len-1; i >= 0; i--) {
    sum += max[i];
    doff[i] = sum;
    if (options.debug)
      printf("doff[%d]: %d\n", i, doff[i]);
  }
  if (options.debug)
    printf("\n");
}

int 
score(int **profile, int *s)
{
  int *n = s;
  int value = 0;
  int k = 0;
  
  for (; *n ;) {
    value += profile[(*n)-1][k];
    n++;
    k++;
  }
  return value;
}

void
next_vertex(int *s, int *i, int len, int k)
{
  /* The integer i indicates the level on which the vertex lies (current vertex)
     the array s represents the vertex at level i, that is the sequence
     of nucleotide codes, from 0 to i-1, being traversed.
     The function returns the next vertex in the tree as a new pairing 
     of an array (s) and a level (i).
     At level len, when the traversal is complete the function will return a 
     level number of 0, the root.
  */
  int j;

  if (*i < len) {
    s[*i] = 1;  /* Add nucleotide A, coded by 1 */
    (*i)++;     /* Increment level  */
    return; 
  } else {
    for (j = len -1; j >= 0; j--) {
      if ( s[j] < k ) {  /* if A or C or G and last level */
        s[j] += 1;       /* Change base: A->C, C->G, G->T  */ 
	*i = j + 1;      /* Set level (to last)  */
	return;
      }
      s[j] = 0;     /* If T, reset nucleotide and go to upper level */
    }
  }
  /* Complete tree traversal */
  *i = 0;
}

void
by_pass(int *s, int *i, int k)
{
  /* The integer i indicates the level on which the vertex lies (current vertex)
     the array s represents the vertex at level i, that is the sequence
     of nucleotide codes, from 0 to i-1, being traversed.
     The subroutine allows us to skip the subtree rooted at vertex (s,i).
     If we skip a vertex at level i of the tree, we can just increment s[i-1],
     unless s[i-1]=4 (T), in which case we need to jump up in the tree.
     At level len=L, when the traversal is complete the function will return a 
     level number of 0, the root.
  */
  int j;

  for (j = *i - 1; j >= 0; j--) {
    if ( s[j] < k ) {  /* if A or C or G and last level */
      s[j] += 1;       /* Change base: A->C, C->G, G->T  */ 
      *i = j + 1;      /* (re)Set level (to current)  */
      return;
    }
    s[j] = 0;     /* If T, reset nucleotide and go to upper level */
  }
  /* Complete tree traversal */
  *i = 0;
}

int 
read_profile(char *iFile, int **p)
{
  FILE *f = fopen(iFile, "r");
  int **a = p; 
  int l = 0;

  if (f == NULL) {
    fprintf(stderr, "Could not open file %s: %s(%d)\n",
	    iFile, strerror(errno), errno);
    return -1;
  }
  if (options.debug != 0)
    fprintf(stderr, "Processing file %s\n", iFile);
  while ( l < LMAX && fscanf(f," %d %d %d %d", &a[0][l], &a[1][l], &a[2][l], &a[3][l]) != EOF )
    l++;
  fclose(f);
  if ( l == 0 ) return -1;
  return l;
}

int 
BranchAndBound_motif_search(int **profile, int len)
{
  int minScore = options.cutOff - 1;
  int partialScore = 0;
  int lmer_cnt = 0;
  int i = 0;
  int j = 0;

  if (options.debug) {
    printf("BranchAndBound_motif_search:\n");
    printf("Motif Length : %d\n", len);
  }
  int *s = (int *)calloc(len + 1, sizeof(int));
  if (s == NULL) {
    fprintf(stderr, "Out of memory: %s(%d)\n",strerror(errno), errno); 
    return 1;
  }
  int *doff = (int *)calloc(len, sizeof(int));
  if (doff == NULL) {
    fprintf(stderr, "Out of memory: %s(%d)\n",strerror(errno), errno); 
    return 1;
  }
  char *lmer_str = (char *)calloc(len + 1, sizeof(char));
  if (lmer_str == NULL) {
    fprintf(stderr, "Out of memory: %s(%d)\n",strerror(errno), errno); 
    return 1;
  }
  lmer_str[len] = '\0';
  drop_off_init(profile, len, doff);
  if (options.debug) {
    printf("drop-off values: ");
    for (j = 0; j < len; j++ )
      printf("%d  ", doff[j]);
    printf("\n\n");
  }
  while ((i > 0) || (minScore == (options.cutOff - 1))) {
    if (i < len) {
      partialScore = score(profile, s);
      if (partialScore < (options.cutOff - doff[i])) {
	/* Bypass the entire subtree rooted at vertex (s,i) */
        //printf("call by_pass for level %d part score %d\n", i, partialScore);
        by_pass(s, &i, NUCL);
      } else {
	/* return next vertex in the tree (s, i) */ 
        //printf("next_vertex for level %d part score %d\n", i, partialScore);
        next_vertex(s, &i, len, NUCL);
      }
    } else { /* We are at the last position/level of the tree  */
      partialScore = score(profile, s);
      //printf("level %d score %d: calling next_vertex for LAST LEVEL ...\n", i, partialScore);
      if (partialScore >= options.cutOff) {
	lmer_cnt +=1;
	//printf("cnt%d: first base %c score %d\n", lmer_cnt, nucleotide[*s - 1], partialScore);
        //nucleotide_string(s, lmer_str);
        //printf("TAG %d: %s  %d\n", lmer_cnt, lmer_str, partialScore);
	if ( !options.count ) {
	  nucleotide_string(s, lmer_str);
	  printf("%s  %d\n", lmer_str, partialScore);
	}
      }
      minScore = partialScore;
      next_vertex(s, &i, len, NUCL);
    }
  } /* While loop on tree traversal */
  if (options.count)
    printf("Total nb of tags above cut-off (options.cutOff) : %d\n", lmer_cnt);
  return 0;
}

int
main(int argc, char *argv[])
{
#ifdef DEBUG
  mcheck(NULL);
  mtrace();
#endif

  options.cutOff = 0;
  options.count = 0;
  int pLen = 0;
  int **profile;
  int i = 0;
  int Lmer = 0;

  while (1) {
    int c = getopt(argc, argv, "c:dhl:t");
    if (c == -1)
      break;
    switch (c) {
      case 'c':
        options.cutOff = atoi(optarg);
        break;
      case 'd':
        options.debug = 1;
        break;
      case 'h':
        options.help = 1;
        break;
      case 'l':
        pLen = atoi(optarg);
        break;
      case 't':
        options.count = 1;
        break;
      case '?':
	break;
      default:
        printf ("?? getopt returned character code 0%o ??\n", c);
      }
    }   
  if (optind == argc || pLen == 0 || options.help == 1) {
     fprintf(stderr, "Usage: %s [options] -l <profile length> <Weight Matrix filename>\n"
	 "      where options are:\n"
	 "  \t\t -h	Show this stuff\n"
	 "  \t\t -d     Produce debugging output\n"
	 "  \t\t -c	Cut-Off value for profile weight matrix (default is %d)\n"
	 "  \t\t -t	Count all tags above cut-off (testing mode)\n\n"
	 "\n\tThe Matrix Branch-and-bound Algorithm (mba) generates sequences\n"
	 "\tfrom a profile weight matrix and a cut-off value.\n"
	 "\tThe weight matrix that describes the profile must have\n"
	 "\tthe following format:\n"
	 "	\tone line = one motif position, base order A, C, G, T\n\n"
	 "\tThe length of the profile must be specified (-l <profile length>), \n"
	 "\talong with the weight matrix filename (<WM filename>). \n"
	 "\tA value can be optionally specified as a cut-off for the matrix (default=0).\n" 
	 "\tIf the option '-t' is given, the program only counts the total number of\n"
	 "\tsequences that can be generated, given the profile and the cut-off value.\n\n",
		argv[0], options.cutOff);
    return 1;
  }
  profile = (int **)calloc(NUCL, sizeof(int)); /* Allocate rows */
  if (profile == NULL) {
    fprintf(stderr, "Could not allocate profile matrix array: %s(%d)\n",
	strerror(errno), errno);
    return 1;
  }
  for ( i = 0; i < NUCL; i++) {
    profile[i] = calloc(pLen, sizeof(int));  /* Allocate columns */
    if (profile[i] == NULL) {
      fprintf(stderr, "Out of memory\n");
      return 1;
    }
  }
  if ((Lmer = read_profile(argv[optind++], profile)) < 0)
    return 1;
  if ( Lmer != pLen) {
    fprintf(stderr, "Profile Matrix (len = %d) is inconsistent with given profile Length (%d)\n", 
    Lmer, pLen);
    return 1;
  }
  if (options.debug != 0) {
    printf("Cut-Off : %d\n", options.cutOff);
    printf("Motif length: %d\n", Lmer);
    printf("Weight Matrix: \n\n");
    int *p;
    for ( i = 0; i < NUCL; i++) {
      /* processing the rows of the Profile Matrix */
      printf("%c => [", nucleotide[i]);
      for (p = profile[i]; p < profile[i] + Lmer; p++) {
        printf(" %d ", (*p));
      }
      printf("]\n");
    }
    printf("\n");
    printf("Testing mode (count generated sequences only) : %d\n", options.count);
  }
  if (BranchAndBound_motif_search(profile, Lmer) != 0)
    return 1;
  return 0;
}
