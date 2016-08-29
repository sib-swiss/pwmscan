/*
    Contains functions which are involved in I/O operations for matrix_scan.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "errors.h"
#include "memory.h"
#include "misc.h"
#include "struct.h"
#include "hash.h"


char* readline(FILE *stream, int *status)
{   /* Reads a line of text from stream stream and dynamically stores it in memory.
       The read line contains a '\n' and a terminal null byte. This functions works
       for stdin pipe input and for regular files since they all have a terminal '\n'.

       FILE *stream : stream to read from.
       int *status  : a variable to store exit information.

       status :      0 on sucess (complete line was read)
                     1 if EOF was found
                    -1 if an error occured.

       Format info :
                line : will be terminated by '\n\0'.

       Still allocated at exit :
                line, on success only.

       Returns :
                On success, the read line (with the '\n'),
                NULL otherwise (EOF or error).
    */

    int i = 0 ;
    int line_size = 100 ;

    char *line = (char*) calloc(line_size, sizeof(char)) ;
    char *line_r = NULL ;

    if(line == NULL)
    {   //error_calloc(stderr, __TIME__, __LINE__) ;
        *status = -1 ;
        return NULL ;
    }

    int c ;
    char current_char ;
    while(1)
    {   c = fgetc(stream) ;
        /* end of read */
        if(c == EOF)
        {   /* regular end of file read */
            if(feof(stream))
            {   *status = 1 ;
                free(line) ;
                return NULL ;
            }
            /* irregular end of file read */
            *status = -1 ;
            free(line) ;
            return NULL ;
        }
        current_char = c ;
        /* line filled */
        if(i == (line_size - 2))
        {   line_r = realloc((void*)line, line_size*2) ;
            if(line_r == NULL)
            {   //error_realloc(stderr, __TIME__, __LINE__) ;
                *status = -1 ;
                return NULL ;
            }
            line = line_r ;
            line_r = NULL ;
            line_size *= 2 ;
        }

        line[i] = current_char ;
        i++ ;
        if(current_char == '\n')
        {   *status = 0 ;
            break ;
        }
    }
    line[i] = '\0' ;
    return line ;
}


int read_matrix_file(FILE *f_in, struct matrix *pwm)
{   /*  Reads a file containing a PWM and stores it in memory

        FILE *f_in         : the file to read from.

        struct matrix *pwm : a struct matrix address where the matrix will be stored.

        Still allocated at exit :
                    pwm->index, on success only.
                    pwm->index[i], on succes only.

        Returns :
                    On success the number 0, -1 if a memory allocation error occured, -2
                    if the matrix format is not correct.
    */

    char *line ;                 /* space to store a line of text from matrix file */
    int   ncol = 0 ;             /* current column count */
    int   status ;               /* status value for readline() */
    int   flag_header = 0 ;      /* flag is_matrix_line_header */
    int   ncol_index = 30 ;      /* current number of columns in matrix_col_index */
    int   format_error_checker ; /* if more than 4 value per file row are present,
                                    this variable will receive a value when sscanf()
                                    is called. This will be recognized as a format
                                    error.
                                 */

    /* will contain the index for each matrix columns since we don't know how many
       there will be...

        index [ pt0 pt1 ... ptN ]
                 |   |        |
                 |   --          --
                 |     |           |
                col0  col1        colN

        Start with a 30 columns index, reallocate if not sufficient
    */
    int **matrix_col_index = (int**) calloc(ncol_index, sizeof(int*)) ;
    if(matrix_col_index == NULL)
    {   error_calloc(stderr, __TIME__, __LINE__) ;
        return -1 ;
    }

    for(; ; ncol++)
    {
        line = readline(f_in, &status) ;
        /* EOF */
        if(status == 1)
        {   ncol -= 1 ;
            break ;
        }
        /* error */
        if(status < 0)
        {   error_readline(stderr, __TIME__, __LINE__, status) ;
            free(line) ;
            return -1 ;
        }

        /* CHECK IF HEADER LINE */
        if(ncol==0)
        {   for(size_t j=0; j<strlen(line)-1; j++)
            {   if(line[j] == '>')
                {   flag_header = 1 ;

                    break ;
                }
            }
            if(flag_header)
            {   free(line) ;
                continue ;
            }
        }

        /* NON-HEADER LINE
           Matrix_row_index[0] will not contain anything if a header line is present, to avoid this
           matrix_col_index[i-header] prevents the creation of a first null pointer
        */
        /* if index is too small*/
        if(ncol == (ncol_index-1))
        {   matrix_col_index = (int**) realloc(matrix_col_index, 2*ncol_index*sizeof(int*)) ;
            if(matrix_col_index == NULL)
            {   error_realloc(stderr, __TIME__, __LINE__) ;
                free(line) ;
                return -1 ;
            }
            ncol_index *= 2 ;
        }
        /* create space for a col */
        /* trick : a column is 26 long, A-> [0], C->[2] etc. Can later access a row with the letter. */
        matrix_col_index[ncol-flag_header] = (int*) calloc(NB_LETTER, sizeof(int)) ;
        if(matrix_col_index[ncol-flag_header] == NULL)
        {   error_calloc(stderr, __TIME__, __LINE__) ;
            free(line) ;
            return -1 ;
        }
        int n = sscanf(line, "%d %d %d %d %d", &matrix_col_index[ncol-flag_header][CHAR_HASH('A')], &matrix_col_index[ncol-flag_header][CHAR_HASH('C')],
        &matrix_col_index[ncol-flag_header][CHAR_HASH('G')], &matrix_col_index[ncol-flag_header][CHAR_HASH('T')], &format_error_checker) ;
        if(n != 4)
        {   free(line) ;
            free(matrix_col_index) ;
            return -2 ;
        }

        free(line) ;
    }

    /* update matrix struct*/
    /* ncol - header + 1
       if the first line is a header, header = 1 -> remove one header line to line count
       +1 because the loop is 0 based.
    */
    pwm->ncol = ncol - flag_header + 1;
    pwm->nrow = NB_LETTER ;
    pwm->index = matrix_col_index ;
    return 0 ;
}


int read_fasta_entry(FILE *stream, struct sequence *sequence, int flag)
{   /*  Reads a FASTA entry (header + sequence) from stream stream and stores the
        informations in a struct sequence sequence. The sequence will be encoded as
        hash char (CHAR_HASH macro).
        The stream can contain several FASTA entries but only one entry will be read
        per function call. The FASTA entries need to be encoded in ASCII characters.
        A FASTA header can only occupy one line but a sequence can spread on several
        ones with no line length limit.


        *FILE stream              : the stream to read from
        struct sequence sequence* : a pointer to a struct sequence were the data will
                                    be stored.
        int flag                  : should be set to 0 if this function reads the
                                    stream for the first time and to non-0 otherwise.

        Format info :
                sequence->header : will be terminated by '\n\0'.
                sequence->seq    : will be terminated by '\0' (not accounted in sequence length)

        Still allocated at exit :
                sequence->header, on success only.
                sequence->seq, on success only.

        Returns :
                 0 : a header and a sequence were successfully read and another header was found,
                 1 : EOF was found during reading, but a sequence and a header were successuflly read,
                -1 : error, file is empty
                -2 : error, no header found at top of file
                -3 : error, other error types (premature EOF, allocation error, readline() error)
    */

    char *tmp = NULL ;
    char current_char ;
    int c = 0 ;
    int status = 0 ;
    size_t i = 0 ;
    size_t size = 100000000 ;

    /* HEADER READING */
    sequence->header = readline(stream, &status) ;
    if(sequence->header == NULL)
    {   /* error */
        if(status == -1)
        {   error_readline(stderr, __FILE__, __LINE__, status) ;
            sequence->seq = NULL ;
            sequence->length = 0 ;
            return -3 ;
        }
        /* EOF  */
        if(status ==  1)
        {   //error_read_fasta_entry(stderr, __FILE__, __LINE__, 1) ;
            sequence->seq = NULL ;
            sequence->length = 0 ;
            return -1 ;
        }
    }
    /* first time the function reads stream */
    if(flag == 0)
    {   if(sequence->header[0] != '>')
        {   //error_read_fasta_entry(stderr, __FILE__, __LINE__, 2) ;
            free(sequence->header) ;
            sequence->header = NULL ;
            sequence->seq = NULL ;
            sequence->length = 0 ;
            return -2 ;
        }
    }
    /* the function already read from this stream, header first char already
       read on previous function call, needs to put '>' back */
    if(flag != 0)
    {   if(sequence->header[0] != '>')
        {   size_t len = strlen(sequence->header) ;
            tmp = (char*) realloc(sequence->header, len + 2) ;
            if(tmp == NULL)
            {   free(sequence->header) ;
                sequence->header = NULL ;
                sequence->seq = NULL ;
                sequence->length = 0 ;
                error_realloc(stderr, __FILE__, __LINE__) ;
                return -1 ;
            }
            sequence->header = tmp ;
            tmp = NULL ;
            memmove(&sequence->header[1], sequence->header, len) ;
            sequence->header[0] = '>' ;
            sequence->header[len+1] = '\0' ;
        }
    }

    /* SEQUENCE READING */
    sequence->seq = (char*) calloc(size, sizeof(char)) ;
    if(sequence->seq == NULL)
    {   error_calloc(stderr, __FILE__, __LINE__) ;
        free(sequence->header) ;
        sequence->header = NULL ;
        sequence->length = 0 ;
        return -1 ;
    }
    while(1)
    {   c = fgetc(stream) ;
        /* end of read */
        if(c == EOF)
        {   /* regular end of read */
            if(feof(stream))
            {   sequence->length = i ;
                sequence->seq[i] = '\0' ;
                return 1 ;
            }
            /* irregular end of file */
            free(sequence->header) ;
            free(sequence->seq) ;
            sequence->header = NULL ;
            sequence->seq = NULL ;
            sequence->length = 0 ;
            //error_read_fasta_entry(stderr, __FILE__, __LINE__, 3) ;
            return -3 ;
        }
        current_char = c ;
        /* header of next sequence */
        if(current_char == '>')
        {   break ; }
        /* end of sequence line are ignored*/
        if(current_char == '\n')
        {   continue ; }
        /* space characters are ignored */
        if(current_char == ' ')
        {   continue ; }
        /* lower case to upper case*/
        if(islower(current_char))
        {   current_char = toupper(current_char) ; }
        /* invalide char are turned to 'N' */
        if((current_char != 'A') && (current_char != 'C') && (current_char != 'G') && (current_char != 'T'))
        {   current_char = 'N' ; }
        /* buffer is full */
         if(i == size - 2)
        {   tmp = (char*) realloc((void*)sequence->seq, size*2) ;
            if(tmp == NULL)
            {   error_realloc(stderr, __FILE__, __LINE__) ;
                free(sequence->header) ;
                sequence->header = NULL ;
                sequence->length = 0 ;
                return -1 ;
            }
            sequence->seq = tmp ;
            tmp = NULL ;
            size *= 2 ;
        }
        sequence->seq[i] = CHAR_HASH(current_char) ;
        i++ ;
    }
    sequence->length = i ;
    sequence->seq[i] = '\0' ;
    return 0 ;
}


int write_result(long score, long cut_off, FILE *file, char *chrom, int start_pos, int end_pos, char *seq, int word_len, char str)
{   /*  Checks if a given subsequence should be written as a result or not and write a BED-like line to file
        file if needed.
        Written BED line is :
                chrom\tstart_pos\tend_pos\tword\tscore\str\n

        long score    : the subsequence score.
        long cut_off  : the minimal score for a results to be output.
        FILE *file    : the stream to write the file to.
        char *chrom   : the subsequence sequence identifier (e.i. a chromosome name)
        int start_pos : the subsequence start coordinate on the complete sequence.
        int end_pos   : the subsequence end coordinate on the complete sequence.
        char *seq     : the adress of the complete sequence.
        int word_len  : the lenth of the subsequence.
        char str      : the strand on which the subsequence is located on the complete sequence ('-' or '+').

        Still allocated at exit :
                Nothing

        Returns :
                999 if the result was not good enough to be written, fprintf return's value otherwise.
    */

    int n = 999 ;
    int status2 = -999 ;
    char *word = NULL ;
    char *word_compl = NULL ;

    /* extract word of interest from complete sequence*/
    word = (char*) calloc(word_len+1, sizeof(char)) ;
    if(word == NULL)
    {   error_calloc(stderr, __TIME__, __LINE__) ;
        exit(EXIT_FAILURE) ;
    }

    /* last char of word is '\0' dued to calloc() */
    for(int i=0; i<word_len; i++)
    {   word[i] = GET_CHAR(seq[i]) ;
    }

    if(score >= cut_off)
    {
        if(str == '-')
        {   /* compute reverse complement of word */
            word_compl = get_compl_seq(word, word_len, &status2) ;
            if(word_compl == NULL)
            {   error_get_compl_seq(stderr, __TIME__, __LINE__, status2) ;
                exit(EXIT_FAILURE) ;
            }
            n = fprintf(file, "%s\t%d\t%d\t%s\t%ld\t%c\n", chrom, start_pos, end_pos, word_compl, score, str) ;
            /* word_compl is allocated by get_comp_seq() */
            free(word_compl) ;
        }
        else
        {   n = fprintf(file, "%s\t%d\t%d\t%s\t%ld\t%c\n", chrom, start_pos, end_pos, word, score, str) ; }
    }
    free(word) ;
    return n ;
}


