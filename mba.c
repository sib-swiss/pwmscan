/*
  mba.c 

  Matrix Branch-and-bound Algorithm for generating sequences
  from a profile weight matrix and a cut-off value.
  Optionally, the program computes a probability matrix 
  instead of a list of sequences.

  Giovanna Ambrosini, ISREC, Giovanna.Ambrosini@isrec.ch

  Copyright (c) 2007 Swiss Institute of Bioinformatics.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#ifdef DEBUG
#include <mcheck.h>
#endif

#define NUCL  4
#define LMAX  100

typedef struct _options_t {
  unsigned int count;
  int cutOff;
  int matrix;
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
static float bg[] = {0.25,0.25,0.25,0.25};

float **probmat;  /* Letter Probabily Matrix */
int **cntmat;     /* Count Matrix */

int K = 1;        /* Pseudo Weight (arbitrary, 1 by default) */
int pLen = 0; /* Matrix Length */

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

int
find_min(int *a, int n)
{
  int i, min;

  min = a[0];
  for (i = 1; i < n; i++)
    if (a[i] < min)
      min = a[i];
  return min;
}

void
drop_off_init(int **profile, int len, int *doff)
{
  int scores[NUCL] = {0};
  int sum = 0;
  int i, k;

  if (options.debug)
    fprintf(stderr, "\ndrop_off_init:\n");
  int *max = (int *) calloc(len, sizeof(int));
  if (max == NULL) {
    fprintf(stderr, "Out of memory: %s(%d)\n",strerror(errno), errno); 
    exit(1);
  }
  for (i = 0; i < len; i++) {
    for (k = 0; k < NUCL; k++) {
      scores[k] = profile[k][i];
      if (options.debug)
        fprintf(stderr, "scores:[%d]  %d\n", k, scores[k]);
    }
    if (options.debug)
      fprintf(stderr, "\nscores and max scores per position\n");
      /* Find max score and store it into max array */
    max[i] = find_max(scores, NUCL);
    if (options.debug)
      fprintf(stderr, "max[%d]: %d\n", i, max[i]);
  }
  if (options.debug)
    fprintf(stderr, "\ndrop-off values per position/level\n");
  for (i = len-1; i >= 0; i--) {
    sum += max[i];
    doff[i] = sum;
    if (options.debug)
      fprintf(stderr, "doff[%d]: %d\n", i, doff[i]);
  }
  if (options.debug)
    fprintf(stderr, "\n");
}

void
min_score(int **profile, int len, int *min) 
{
  int scores[NUCL] = {0};
  int sum = 0;
  int i, k;
  int part_min = 0;

  for (i = 0; i < len; i++) {
    for (k = 0; k < NUCL; k++) {
      scores[k] = profile[k][i];
      if (options.debug)
        fprintf(stderr, "scores:[%d]  %d\n", k, scores[k]);
    }
    if (options.debug)
      fprintf(stderr, "\nscores and min scores per position\n");
      /* Find min score and store it into min array */
    part_min = find_min(scores, NUCL);
    if (options.debug)
      fprintf(stderr, "row[%d] partial min: %d\n", i, part_min);
    sum += part_min;
  }
  *min = sum;
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
     level number of 0, the root. The parameter k=NUCL.
     The function next_vertex is used to navigate vertically through the tree,
     that is to explore a new branch of the tree.
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
     The function by_pass is used to move horizontally, based on the estimate
     of upper and lower bounds (sequence score).
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
  int p_len = pLen;

  if (f == NULL) {
    fprintf(stderr, "Could not open file %s: %s(%d)\n",
	    iFile, strerror(errno), errno);
    return -1;
  }
  if (options.debug != 0)
    fprintf(stderr, "Processing file %s\n", iFile);

  int ret;
  while ( (ret=fscanf(f," %d %d %d %d", &a[0][l], &a[1][l], &a[2][l], &a[3][l])) != EOF ) {
    if (ret == 0) {
      fprintf(stderr, "Matrix hasn't the correct format (a PWM without header is required)\n");
      return 0;
    }
    if (l == p_len - 1) {
      /* Reallocate Matrix columns */
      for ( int i = 0; i < NUCL; i++) {
        a[i] = realloc(a[i], p_len*2*sizeof(int));  /* Rellocate columns */
        if (a[i] == NULL) {
          fprintf(stderr, "Out of memory\n");
          return 1;
        }
      }
      p_len *= 2;
    }
    l++;
  }
  fclose(f);
  if ( l == 0 ) return -1;
  return l;
}

int 
BranchAndBound_motif_search(int **profile, int len)
{
  int minScore = 0;
  int partialScore = 0;
  int lmer_cnt = 0;
  int min = 0;
  int i = 0;
  int j = 0;
  int k = 0;

  if (options.debug) {
    fprintf(stderr, "BranchAndBound_motif_search:\n");
    fprintf(stderr, "Motif Length : %d\n", len);
  }
  int *s = calloc(len + 1, sizeof(int));
  if (s == NULL) {
    fprintf(stderr, "Out of memory: %s(%d)\n",strerror(errno), errno); 
    return 1;
  }
  int *doff = calloc(len, sizeof(int));
  if (doff == NULL) {
    fprintf(stderr, "Out of memory: %s(%d)\n",strerror(errno), errno); 
    return 1;
  }
  char *lmer_str = calloc(len + 1, sizeof(char));
  if (lmer_str == NULL) {
    fprintf(stderr, "Out of memory: %s(%d)\n",strerror(errno), errno); 
    return 1;
  }
  lmer_str[len] = '\0';
  drop_off_init(profile, len, doff);
  if (options.debug) {
    fprintf(stderr, "drop-off values: ");
    for (j = 0; j < len; j++)
      fprintf(stderr, "%d  ", doff[j]);
    fprintf(stderr, "\n\n");
  }
  min_score(profile, len, &min);
  minScore = min;
  if (options.debug)
    fprintf(stderr, "minScore: %d  Cut-off: %d\n", minScore, options.cutOff);
  if (options.cutOff > doff[0]) {
    if (options.debug)
      fprintf(stderr, "Cut-off is greater than maximal matrix score (%d), exiting...\n", doff[0]);
    return 1;
  }
  while ((i > 0) || (minScore == min)) {
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
	if ((!options.count) && (!options.matrix)) {
	  nucleotide_string(s, lmer_str);
	  printf("%s  %d\n", lmer_str, partialScore);
	}
        if (options.matrix) { 
          for (k = 0; k < len; k++)
            cntmat[s[k]-1][k]++;
        }
      } /* Score >= cutoff */
      minScore = partialScore;
      //printf("level: %d before next_vertex\n", i);
      next_vertex(s, &i, len, NUCL);
      //printf("level: %d minScore %d\n", i , minScore);
    }
  } /* While loop on tree traversal */
  if (options.matrix) {
    printf(">letter-probability matrix: alength= 4 w= %d nsites= %d\n", len, lmer_cnt);
    if (options.debug)
      fprintf(stderr,"Pseudo Weight: %d\n", K);
    for (i = 0; i < len; i++) {
      for (k = 0; k < NUCL; k++) {
        if (options.debug)
          fprintf(stderr,"cntmat[%d][%d]: %d , bg[%d]: %f corr= %f\n", k, i, cntmat[k][i], k, bg[k], bg[k]*K);
        probmat[k][i] = (float)(cntmat[k][i] + (float)bg[k]*K)/(float)(lmer_cnt + K);
        printf("%f ", probmat[k][i]);
      }
      printf("\n");
    }
    /*printf("\n"); */
  }
  if (options.count)
    printf("Total nb of tags above cut-off (options.cutOff) : %d\n", lmer_cnt);
  return 0;
}

char** 
str_split(char* a_str, const char a_delim)
{
    char** result = 0;
    size_t count = 0;
    char* tmp = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    /* Add space for terminating null string so caller
 *        knows where the list of returned strings ends. */
    count++;
    result = malloc(sizeof(char*) *count);

    if (result) {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
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
  int **profile;
  int i = 0;
  int Lmer = 0;
  char *bgProb = NULL;
  char** tokens;

  while (1) {
    int c = getopt(argc, argv, "c:dhmk:l:p:t");
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
      case 'm':
        options.matrix = 1;
        break;
      case 'k':
        K = atoi(optarg);
        break;
      case 'l':
        pLen = atoi(optarg);
        break;
      case 'p':
        bgProb = optarg;
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
     fprintf(stderr, "Usage: %s [options] -l <PWM length> <PWM filename>\n"
	 "      where options are:\n"
	 "  \t\t -h	   Show this stuff\n"
	 "  \t\t -d        Produce debugging output\n"
	 "  \t\t -c	   Cut-Off value for profile weight matrix (Default is %d)\n"
         "  \t\t -m        Output a base probability matrix instead of a list of sequences\n"
         "  \t\t -k        Define a pseudo weight distributed according to residue priors\n"
         "  \t\t           (Default is %d)\n"
         "  \t\t -p <bg>   Define residue priors (<bg>), by default : 0.25,0.25,0.25,0.25\n"
         "  \t\t           Note that nucleotide frequencies MUST BE comma-separated\n"
	 "  \t\t -t	   Count all tags above cut-off (testing mode)\n\n"
	 "\n\tThe Matrix Branch-and-bound Algorithm (mba) generates sequences from a given\n"
	 "\tposition weight matrix (PWM) and a cut-off value.\n"
	 "\tOptionally, the program computes a probability matrix instead of outputting\n"
	 "\ta list of sequences (-m option).\n"
	 "\tThe weight matrix that describes the profile must have the following format:\n"
	 "	\tone line = one motif position, base order A, C, G, T\n\n"
	 "\tThe length of the profile must be specified (-l <PWM length>), along with\n"
	 "\twith the weight matrix filename (<PWM filename>). \n"
	 "\tA value can be optionally specified as a cut-off for the matrix (default=0).\n" 
	 "\tIf the option '-t' is given, the program only counts the total number of\n"
	 "\tsequences that can be generated, given the profile and the cut-off value.\n\n",
		argv[0], options.cutOff, K);
    return 1;
  }
  /* Allocate space for profile (PWM) */
  profile = (int **)calloc(NUCL, sizeof(int *)); /* Allocate rows */
  if (profile == NULL) {
    fprintf(stderr, "Could not allocate profile matrix array: %s(%d)\n",
	strerror(errno), errno);
    return 1;
  }
  for (i = 0; i < NUCL; i++) {
    profile[i] = calloc(pLen, sizeof(int));  /* Allocate columns */
    if (profile[i] == NULL) {
      fprintf(stderr, "Out of memory\n");
      return 1;
    }
  }
  if ((Lmer = read_profile(argv[optind++], profile)) <= 0)
    return 1;
  if ( Lmer != pLen) {
    fprintf(stderr, "Profile Matrix (len = %d) is inconsistent with given profile Length (%d)\n", 
    Lmer, pLen);
    return 1;
  }
  if (options.debug != 0) {
    fprintf(stderr, "Cut-Off : %d\n", options.cutOff);
    fprintf(stderr, "Motif length: %d\n", Lmer);
    fprintf(stderr, "Prior residue probabilities: %s\n", bgProb);
    fprintf(stderr, "Pseudo Weight: %d\n", K);
    fprintf(stderr, "Weight Matrix: \n\n");
    int *p;
    for (i = 0; i < NUCL; i++) {
      /* processing the rows of the Profile Matrix */
      fprintf(stderr, "%c => [", nucleotide[i]);
      for (p = profile[i]; p < profile[i] + Lmer; p++) {
        fprintf(stderr, " %d ", (*p));
      }
      fprintf(stderr, "]\n");
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "Testing mode (count generated sequences only) : %d\n", options.count);
  }
  /* Allocate space for both cntmat and probmat matrices */
  if (options.matrix) {
    cntmat = (int **)calloc(NUCL, sizeof(int *)); /* Allocate rows */
    if (cntmat == NULL) {
      fprintf(stderr, "Could not allocate count matrix array: %s(%d)\n",
  	strerror(errno), errno);
      return 1;
    }
    for (i = 0; i < NUCL; i++) {
      cntmat[i] = calloc(Lmer, sizeof(int));  /* Allocate columns */
      if (cntmat[i] == NULL) {
        fprintf(stderr, "Out of memory\n");
        return 1;
      }
    }
    probmat = (float **)calloc(NUCL, sizeof(float *)); /* Allocate rows */
    if (probmat == NULL) {
      fprintf(stderr, "Could not allocate probability matrix array: %s(%d)\n",
  	strerror(errno), errno);
      return 1;
    }
    for (i = 0; i < NUCL; i++) {
      probmat[i] = calloc(Lmer, sizeof(float));  /* Allocate columns */
      if (probmat[i] == NULL) {
        fprintf(stderr, "Out of memory\n");
        return 1;
      }
    }
  }
  /* Treat prior nucleotide frequencies (bg probability) */
  if (bgProb != NULL) {
    tokens = str_split(bgProb, ',');
    if (tokens) {
      int i;
      for (i = 0; *(tokens + i); i++) {
        bg[i] =  atof(*(tokens + i));
        free(*(tokens + i));
      }
      if (i != 4) {
        fprintf(stderr, "Number of TOKENS: %d\n", i);
        fprintf(stderr, "Please, specify correct residue priors (<bg>): they MUST BE comma-separated!\n");
        exit(1);
      }
      free(tokens);
    }
  }
  if (options.debug != 0) {
    fprintf(stderr, "Background nucleotide frequencies:\n");
    for (i = 0; i < NUCL; i++) {
      fprintf(stderr, "bg[%i] = %f ", i, bg[i]);
    }
    fprintf(stderr, "\n");
  }
  /* Call Branch&Bound algorithm */
  if (BranchAndBound_motif_search(profile, Lmer) != 0) {
    for ( i = 0; i < NUCL; i++) {
      if (options.debug)
        fprintf(stderr, "Freeing memory: %d profile[%d] = %p\n", i, i, (void *)profile[i]);
      free(profile[i]);  /* Free Matrix columns */
    }
    free(profile);       /* Free Matrix rows */
    return 1;
  }
  for ( i = 0; i < NUCL; i++) {
    if (options.debug)
      fprintf(stderr, "Freeing memory: %d profile[%d] = %p\n", i, i, (void *)profile[i]);
    free(profile[i]);  /* Free Matrix columns */
  }
  free(profile);       /* Free Matrix rows */
  return 0;
}
