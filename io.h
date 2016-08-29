#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

#ifndef _STRING_H
    #include <string.h>
#endif

#ifndef _CTYPE_H
    #include <ctype.h>
#endif

#ifndef _STRUCT_H
    #include "struct.h"
#endif

#ifndef _HASH_H
    #include "hash.h"
#endif


#ifndef _IO_H
    #define _IO_H 1

    char* readline(FILE *stream, int *status) ;

    int read_matrix_file(FILE *f_in, struct matrix *pwm) ;

    char* read_file(FILE *stream, char *buffer, int *status, size_t *nline, size_t *nchar) ;

    char* read_stdin(char *buffer, int *status, size_t *nline, size_t *nchar) ;

    int read_fasta_entry(FILE *stream, struct sequence *sequence, int flag) ;

    int write_result(long score, long cut_off, FILE *file, char *chrom, int start_pos, int end_pos, char *seq, int word_len, char str) ;

#endif // _IO_H
