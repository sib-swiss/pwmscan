/*
  matrix_scan.c

  Scan a DNA sequence file with a Position Weight Matrix (PWM) and returns
  a list of hits in BED-like format with the corresponding matrix score

  # Arguments:
    # Sequence File
    # Matrix File
    # Cut-off score

  DNA Sequence :
    The sequence can contain upper or lower case ASCII characters. Lower case
    characters are converted to uppercase, and all the non 'ACGT' characters
    are converted to 'Ns'. Space characters are ignored.

    The sequence file is read at once and a hash code is computed for each character.
    The sequence is stored in memory as a hash code sequence. It simplifies the access
    to the relevant matrix row when scoring a subsequence.
    If a subsequence contains one or more Ns is discarded.
    The file is expected to be in FASTA format. However, any format consisting of a
    single header line followed by the sequence characters is also supported.

  Matrix :
    The matrix is read from a file. It must be an integer vertical PWM:
    i.e. the file columns correspond to the matrix rows in memory in the
    following order: 'A', 'C', 'G', 'T'.
    The matrix may contain a header line though it is not used.

  Score :
    An integer value representing the PWM score of a DNA sub-sequence.

  Romain Groux, EPFL, 2015

  Copyright (c) 2015 EPFL and Swiss Institute of Bioinformatics.

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




#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include "struct.h"
#include "io.h"
#include "matrix.h"
#include "sequence.h"
#include "misc.h"
#include "errors.h"

int main(int argn, char **argv)
{
    /* command line parsing */
    char c ;
    char *f_seq_name = NULL ;
    char *f_matrix_name = NULL ;
    char *cut_off_arg = NULL ;
    char *n_delimiter_arg = NULL ;
    long cut_off = 0 ;
    long cut_off_rescaled = 0 ;
    long n_delimiter = 2 ;
    /* messages to be displayed by the help (help_messages.c) */
    extern char *help_message_head ;
    extern char *help_message_opt ;
    extern char *help_message_i ;
    extern char *help_message_m ;
    extern char *help_message_c ;
    extern char *help_message_d ;
    extern char *help_message_h ;
    extern char *help_message_gen ;


    while((c = getopt(argn, argv, "i:m:c:d:h")) != -1)
    {   switch(c)
        {   case 'i' :  f_seq_name = optarg ;
                        break ;

            case 'm' :  f_matrix_name = optarg ;
                        break ;

            case 'c' :  cut_off_arg = optarg ;
                        cut_off = strtol(cut_off_arg, NULL, 0) ;
                        if((cut_off == LONG_MIN || cut_off == LONG_MAX ) && (errno == ERANGE))
                        {   error_strtol(stderr, __FILE__, __LINE__) ;
                            exit(EXIT_FAILURE) ;
                        }
                        break ;

            case 'd' :  n_delimiter_arg = optarg ;
                        n_delimiter = strtol(n_delimiter_arg, NULL, 0) ;
                        if((n_delimiter == LONG_MIN || n_delimiter == LONG_MAX) && (errno == ERANGE))
                        {   error_strtol(stderr, __FILE__, __LINE__) ;
                            exit(EXIT_FAILURE) ;
                        }
                        break ;

            case 'h' :  printf("\n\n%s%s%s%s%s%s\n%s\n\n", help_message_head, help_message_opt, help_message_i, help_message_m, help_message_c,
                        help_message_h, help_message_gen) ;
                        return EXIT_SUCCESS ;

        }
    }
    /* checks that every option is set*/
    /* any option set displays help*/
    if((f_seq_name == NULL) && (f_matrix_name == NULL) && (cut_off_arg == NULL) && (n_delimiter_arg == NULL))
    {   printf("\n\n%s%s%s%s%s%s%s\n%s\n\n", help_message_head, help_message_opt, help_message_i, help_message_m, help_message_c,
        help_message_d, help_message_h, help_message_gen) ;
        return EXIT_SUCCESS ;
    }
    if(f_matrix_name == NULL)
    {   fprintf(stderr, "Please set -m option!\n") ;
        exit(EXIT_FAILURE) ;
    }
    if(cut_off_arg == NULL)
    {   fprintf(stderr, "Please set -c option!\n") ;
        exit(EXIT_FAILURE) ;
    }

    /* read matrix file and process matrices */
    FILE *f_matrix = NULL ;
    f_matrix = fopen(f_matrix_name, "rt") ;
    if(f_matrix == NULL)
    {   error_fopen(stderr, __FILE__, __LINE__, f_matrix_name) ;
        exit(EXIT_FAILURE) ;
    }
    struct matrix pwm ;
    init_matrix(&pwm) ;
    struct matrix pwm_rev ;
    init_matrix(&pwm_rev) ;
    struct matrix pwm_rscd ;
    init_matrix(&pwm_rscd) ;
    struct matrix pwm_rev_rscd ;
    init_matrix(&pwm_rev_rscd) ;
    struct matrix_rescaled rescaled ;
    rescaled.matrix = &pwm_rscd ;
    rescaled.rescal_factors = NULL ;
    struct matrix_rescaled rescaled_rev ;
    rescaled_rev.matrix = &pwm_rev_rscd ;
    rescaled_rev.rescal_factors = NULL ;
    /* read matrix file */
    int n = read_matrix_file(f_matrix, &pwm) ;
    if(n != 0)
    {   error_read_matrix_file(stderr, __FILE__, __LINE__, n) ;
        fclose(f_matrix) ;
        exit(EXIT_FAILURE) ;
    }
    fclose(f_matrix) ;
    /* Produce matrix for reverse strand */
    if(reverse_matrix(&pwm, &pwm_rev) == -1 )
    {   error_reverse_matrix(stderr, __FILE__, __LINE__) ;
        exit(EXIT_FAILURE) ;
    }
    /* rescale matrices and cut-off*/
    if(rescale_matrix(&pwm, &rescaled) < 0)
    {   error_rescale_matrix(stderr, __FILE__, __LINE__) ;
        exit(EXIT_FAILURE) ;
    }
    if(rescale_matrix(&pwm_rev, &rescaled_rev) < 0)
    {   error_rescale_matrix(stderr, __FILE__, __LINE__) ;
        exit(EXIT_FAILURE) ;
    }
    cut_off_rescaled = rescale_value(cut_off, pwm_rscd.ncol, rescaled.rescal_factors, '+') ;


    /* determines from which stream sequence is coming */
    FILE *f_seq = NULL ;
    /* given through stdin */
    if(f_seq_name == NULL)
    {   f_seq = stdin ; }
    /* given through regular file */
    else
    {   f_seq = fopen(f_seq_name, "rt") ;
        if(f_seq == NULL)
        {   error_fopen(stderr, __FILE__, __LINE__, f_seq_name) ;
            exit(EXIT_FAILURE) ;
        }
    }

    /* reads every FASTA entry in input stream */
    struct sequence sequence ;
    init_sequence(&sequence) ;
    int read_status = 0 ;
    int flag = 0 ;
    while(1)
    {
        read_status = read_fasta_entry(f_seq, &sequence, flag++) ;
        /* error */
        if(read_status < 0)
        {   switch(read_status)
            {   case -1 :   error_read_fasta_entry(stderr, __FILE__, __LINE__, read_status) ;
                            break ;
                case -2 :   error_read_fasta_entry(stderr, __FILE__, __LINE__, read_status) ;
                            break ;
                case -3 :   error_read_fasta_entry(stderr, __FILE__, __LINE__, read_status) ;
                            break ;
            }
        }
        /* needs to check all possible error codes returnes (int and sequence NULL etc ) */

        /* process sequence header */
        char *sequence_id = NULL ;
        sequence_id = parse_header(sequence.header, n_delimiter) ;
        if(sequence_id == NULL)
        {   sequence_id = (char*) calloc(5, sizeof(char)) ;
            sequence_id[0] = 'c' ; sequence_id[1] = 'h' ;
            sequence_id[2] = 'r' ; sequence_id[3] = 'N' ;
        }

        /* scores each subsequence in sequence */
        int subsequence_len = rescaled.matrix->ncol ;
        for(int i=0; i<sequence.length-(pwm.ncol-1); i++)
        {
            long score_fw = 0 ;
            long score_rev = 0;
            int current_base ;

            /* fw strand score */
            /* this code block is duplicated, kept outside a function to gain running time */
            for(int icol=0; icol<rescaled.matrix->ncol; icol++)
            {   /* interrupt score calculation */
                if(score_fw < cut_off_rescaled)
                {   score_fw = 1 ;
                    break ;
                }

                current_base = sequence.seq[i+icol] ;

                if(current_base == HASH_N)
                {   score_fw = 1 ;
                    break ;
                }
                score_fw += (rescaled.matrix->index[icol][current_base]) ;
            }
            if(score_fw <= 0)
            {   /* reverting scaling on score*/
                long score = rescale_value(score_fw, rescaled.matrix->ncol, rescaled.rescal_factors, '-') ;
                int n = write_result(score, cut_off, stdout, sequence_id, i, i+subsequence_len, &sequence.seq[i], subsequence_len, '+') ;
                if(n < 0)
                /* weak point, not checking that ALL the data have been correctly written to the file, only checking that
                   something has been written */
                {   error_write_result(stderr, __FILE__, __LINE__, n) ;
                    exit(EXIT_FAILURE) ;
                }
            }


            /* rev strand score */
            for(int icol=0; icol<rescaled_rev.matrix->ncol; icol++)
            {   /* interrupt score calculation */
                if(score_rev < cut_off_rescaled)
                {   score_rev = 1 ;
                    break ;
                }

                current_base = sequence.seq[i+icol] ;

                if(current_base == HASH_N)
                {   score_rev = 1 ;
                    break ;
                }

                score_rev += (rescaled_rev.matrix->index[icol][current_base]) ;
            }
            if(score_rev <= 0)
            {   /* reverting scaling on score*/
                /* weak point : cannot check if the copy went well or not */
                long score = rescale_value(score_rev, rescaled_rev.matrix->ncol, rescaled_rev.rescal_factors, '-') ;
                int n = write_result(score, cut_off, stdout, sequence_id, i, i+subsequence_len, &sequence.seq[i], subsequence_len, '-') ;
                if(n < 0)
                /* weak point, not checking that ALL the data have been correctly written to the file, only checking that
                   something has been written */
                {   error_write_result(stderr, __FILE__, __LINE__, n) ;
                    exit(EXIT_FAILURE) ;
                }
            }
        }

        /* last sequence in stream has been read and processed */
        if(read_status == 1)
        {   free_sequence(&sequence) ;
            free(sequence_id) ;
            break ;
        }
        free_sequence(&sequence) ;
        free(sequence_id) ;
    }

    /* frees all memory still allocated */
    free_matrix(&pwm) ;
    free_matrix(&pwm_rev) ;
    free_matrix_rescaled(&rescaled) ;
    free_matrix_rescaled(&rescaled_rev) ;

    (f_seq_name == NULL) ? 0 : fclose(f_seq) ;

    return EXIT_SUCCESS ;
}
