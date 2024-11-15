/*
  mscan_bed2sga.c

  Convert BED file from PWMScan to SGA format.

  # Arguments:
  # species
  # feature name (e.g. matrix name)

  Giovanna Ambrosini, EPFL/ISREC, Giovanna.Ambrosini@epfl.ch

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
/*
#define DEBUG
*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include "hashtable.h"
#ifdef DEBUG
#include <mcheck.h>
#endif

/*#define BUF_SIZE 4096 */
#define BUF_SIZE 8192
#define LINE_SIZE 1024
#define FT_MAX  64
#define CHR_NB 8
#define AC_MAX 18
#define POS_MAX 16
#define SCORE_MAX 12
#define TAG_MAX 128

typedef struct _options_t {
  char *dbPath;
  int help;
  int debug;
  int db;
} options_t;

static options_t options;

typedef struct _bedline_t {
  char seq_id[AC_MAX];
  unsigned long *start;
  unsigned long *end;
  char **tag;
  int *score;
  char *strand;
  int *pflag;
} bedline_t, *bedline_p_t;

bedline_t bed_reg;

char *Species = NULL;
char *Feature = NULL;

static hash_table_t *ac_table = NULL;

int
process_ac()
{
  FILE *input;
  int c;
  char buf[LINE_SIZE];
  char *chrFile;
  char chrom[12];
  int cLen;

  if (options.db) {
      cLen = (int)strlen(options.dbPath) + (int)strlen(Species) + 12;
      if ((chrFile = (char*)malloc(cLen * sizeof(char))) == NULL) {
        perror("process_ac: malloc");
        exit(1);
      }
      strcpy(chrFile, options.dbPath);
  } else {
      cLen = 21 + (int)strlen(Species) + 12;
      if ((chrFile = (char*)malloc(cLen * sizeof(char))) == NULL) {
        perror("process_ac: malloc");
        exit(1);
      }
      strcpy(chrFile, "/home/local/db/genome");
  }
  strcat(chrFile, "/");
  strcat(chrFile, Species);
  strcat(chrFile, "/chr_NC_gi");

  input = fopen(chrFile, "r");
  if (input == NULL) {
    fprintf(stderr, "Could not open file %s: %s(%d)\n",
            chrFile, strerror(errno), errno);
    return 1;
  }

  do {
      c = fgetc(input);
  } while(c != '\n');

  ac_table = hash_table_new(MODE_COPY);
  while (fgets(buf, LINE_SIZE, input) != NULL) {
    char *s;
    char chr_nb[CHR_NB] = "";
    char ncbi_ac[AC_MAX] = "";
    int i = 0;
    int nb_len = 0;
    int ac_len = 0;
    /* Chrom NB */
    s = buf;
    while (*s != 0 && !isspace(*s)) {
      if (i >= CHR_NB) {
        fprintf(stderr, "AC too long in %s\n", s);
        fclose(input);
        exit(1);
      }
      chr_nb[i++] = *s++;
    }
    if (i < CHR_NB)
      chr_nb[i] = 0;
    nb_len = i + 1;
    while (isspace(*s))
      s++;
    /* Chromosome AC */
    i = 0;
    while (*s != 0 && !isspace(*s)) {
      if (i >= AC_MAX) {
        fprintf(stderr, "AC too long \"%s\" \n", s);
        fclose(input);
        exit(1);
      }
      ncbi_ac[i++] = *s++;
    }
    if (i < AC_MAX)
      ncbi_ac[i] = 0;
    ac_len = i + 1;
    strcpy(chrom, "chr");
    strcat(chrom, chr_nb);
    nb_len = (int)strlen(chrom) + 1;
    //printf("adding key %s (len = %d) value %s (ac) (len = %d) to hash table\n", chrom, nb_len, ncbi_ac, ac_len);
    hash_table_add(ac_table, chrom, (size_t)nb_len, ncbi_ac, (size_t)ac_len);
    if (options.debug) {
      char *ac = hash_table_lookup(ac_table, chrom, (size_t)nb_len);
      fprintf (stderr, "Hash table value for %s (len = %d) is %s (len = %d)\n", chrom, nb_len, ac, ac_len);
    }
  }
  return 0;
}

int
process_bed(FILE *input, char *iFile)
{
  unsigned long start, end;
  int score;
  char *s, *res, *buf;
  size_t bLen = LINE_SIZE;

  if (options.debug && input != stdin) {
    char sort_cmd[1024] = "sort -s -c -k1,1 -k2,2n ";
    fprintf(stderr, "Check whether file %s is properly sorted\n", iFile);
      if (strcat(sort_cmd, iFile) == NULL) {
        fprintf(stderr, "strcat failed\n");
        return 1;
      }
    if (strcat(sort_cmd, " 2>/tmp/sortcheck.out") == NULL) {
      fprintf(stderr, "strcat failed\n");
      return 1;
    }
    fprintf(stderr, "executing : %s\n", sort_cmd);
    int sys_code = system(sort_cmd);
    if (sys_code != 0) {
      fprintf(stderr, "system command failed\n");
      return 1;
    }
    struct stat file_status;
    if(stat("/tmp/sortcheck.out", &file_status) != 0){
      fprintf(stderr, "could not stat\n");
      return 1;
    }
    if (file_status.st_size != 0) {
      fprintf(stderr, "BED file %s is not properly sorted\n", iFile);
      return 1;
    } else {
      system("/bin/rm /tmp/sortcheck.out");
    }
  }

  if ((s = malloc(bLen * sizeof(char))) == NULL) {
    perror("process_bed: malloc");
    exit(1);
  }
#ifdef DEBUG
  int c = 1;
#endif
  while ((res = fgets(s, (int) bLen, input)) != NULL) {
    size_t cLen = strlen(s);
    char seq_id[AC_MAX] = "";
    char tag[TAG_MAX] = "";
    char s_pos[POS_MAX] = "";
    char e_pos[POS_MAX] = "";
    char sc[SCORE_MAX] = "";
    char strand = '\0';
    unsigned int i = 0;

    while (cLen + 1 == bLen && s[cLen - 1] != '\n') {
      bLen *= 2;
      if ((s = realloc(s, bLen)) == NULL) {
        perror("process_file: realloc");
        exit(1);
      }
      res = fgets(s + cLen, (int) (bLen - cLen), input);
      cLen = strlen(s);
    }
    if (s[cLen - 1] == '\n')
      s[cLen - 1] = 0;

    buf = s;
    /* Get BED fields */
    /* Chrom NB */
    while (*buf != 0 && !isspace(*buf)) {
      if (i >= AC_MAX) {
        fprintf(stderr, "Chrom NB too long \"%s\" \n", buf);
        exit(1);
      }
      seq_id[i++] = *buf++;
    }
    while (isspace(*buf))
      buf++;
    /* Start Position */
    i = 0;
    while (isdigit(*buf)) {
      if (i >= POS_MAX) {
        fprintf(stderr, "Start position too large \"%s\" \n", buf);
        exit(1);
      }
      s_pos[i++] = *buf++;
    }
    s_pos[i] = 0;
    start = (unsigned long)atoi(s_pos);
    while (isspace(*buf))
      buf++;
    /* End Position */
    i = 0;
    while (isdigit(*buf)) {
      if (i >= POS_MAX) {
        fprintf(stderr, "End position too large \"%s\" \n", buf);
        exit(1);
      }
      e_pos[i++] = *buf++;
    }
    e_pos[i] = 0;
    end = (unsigned long)atoi(e_pos);
    while (isspace(*buf))
      buf++;
    /* Tag */
    i = 0;
    while (*buf != 0 && !isspace(*buf)) {
      if (i >= TAG_MAX) {
        fprintf(stderr, "Tag too long \"%s\" \n", buf);
        fclose(input);
        exit(1);
      }
      tag[i++] = *buf++;
    }
    tag[i] = 0;
    while (isspace(*buf))
      buf++;
    /* Score */
    i = 0;
    while (isdigit(*buf) || *buf == '+' || *buf == '-') {
      if (i >= SCORE_MAX) {
        fprintf(stderr, "Score too large \"%s\" \n", buf);
        exit(1);
      }
      sc[i++] = *buf++;
    }
    sc[i] = 0;
    score = atoi(sc);
    while (isspace(*buf))
      buf++;
    /* Strand */
    strand = *buf++;
    while (isspace(*buf))
      buf++;

#ifdef DEBUG
    printf(" [%d] Chr nb: %s   Start: %lu  End: %lu  Tag: %s  Score: %d Strand: %c\n", c++, seq_id, start, end, tag, score, strand);
#endif

    unsigned long pos = 0;
    if (strand == '+') {
      pos = start + 1;
    } else {
      pos = end;
    }
    int id_len = (int)strlen(seq_id) + 1;
    char *ac = hash_table_lookup(ac_table, seq_id, (size_t)id_len);
    //fprintf (stderr, "Hash table value for %s is %s\n", seq_id, ac);
    printf("%s\t%s\t%lu\t%c\t1\t%s\t%d\n", ac, Feature, pos, strand, tag, score);
  } /* End of While */
  /* Filter overlapping regions for last chromosome */
  if (input != stdin) {
    fclose(input);
  }
  return 0;
}

int
main(int argc, char *argv[])
{
#ifdef DEBUG
  mcheck(NULL);
  mtrace();
#endif
  FILE *input;

  while (1) {
    int c = getopt(argc, argv, "dhf:s:");
    if (c == -1)
      break;
    switch (c) {
      case 'd':
        options.debug = 1;
        break;
      case 'h':
        options.help = 1;
        break;
      case 'i':
        options.dbPath = optarg;
        options.db = 1;
        break;
      case 'f':
        Feature = optarg;
        break;
      case 's':
        Species = optarg;
        break;
      default:
        printf ("?? getopt returned character code 0%o ??\n", c);
    }
  }
  if (optind > argc || options.help == 1 || Species == NULL) {
    fprintf(stderr, "Usage: %s [options] [-f <feature name>] -s <s_assembly (e.g. hg18)> [<] <BED file|stdin>\n"
             "      where options are:\n"
             "  \t\t -d     Produce debug information\n"
             "  \t\t -h     Show this help text\n"
             "  \t\t -i <path> Use <path> to locate the chr_NC_gi file (default is /home/local/db/genome)\n"
             "\n\tConvert BED format (from PWM scan or sequence scan applications) into extended SGA format.\n"
             "\n\tBED lines have the following format:\n"
             "\t\t chr1\t28940\t28955\tGGCTTCCCAGAACCC\t1330\t+\n"
             "\n\t representing the genomic position of a sequence tag match.\n\n",
             argv[0]);
      return 1;
  }
  if (argc > optind) {
      if(!strcmp(argv[optind],"-")) {
          input = stdin;
      } else {
          input = fopen(argv[optind], "r");
          if (NULL == input) {
              fprintf(stderr, "Unable to open '%s': %s(%d)\n",
                  argv[optind], strerror(errno), errno);
             exit(EXIT_FAILURE);
          }
          if (options.debug)
             fprintf(stderr, "Processing file %s\n", argv[optind]);
      }
  } else {
      input = stdin;
  }
  if (Feature == NULL) {
    if ((Feature = malloc(6 * sizeof(char))) == NULL) {
      perror("process_sga: malloc Feature");
      exit(1);
    }
    strcpy(Feature, "motif");
  } else {
    char *s = Feature;
    int i = 0;
    while (*s != 0  && !isspace(*s)) {
      if (i >= FT_MAX) {
        fprintf(stderr, "Feature Name too long \"%s\" \n", Feature);
        return 1;
      }
      s++;
    }
  }

  if (options.debug) {
    fprintf(stderr, " Arguments:\n");
    fprintf(stderr, " Feature : %s\n", Feature);
    fprintf(stderr, " Species : %s\n", Species);
  }
  if (process_ac() == 0) {
    if (options.debug)
      fprintf(stderr, "HASH Table for chromosome access identifier initialized\n");
  } else {
    return 1;
  }
  if (process_bed(input, argv[optind++]) != 0) {
    return 1;
  }
  return 0;
}
