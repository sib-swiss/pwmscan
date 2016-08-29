#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif


#ifndef _ERROR_H
    #define _ERROR_H 1

void error_fopen(FILE *stream, char *file, int line, char *file2) ;

void error_malloc(FILE *stream, char *file, int line) ;

void error_calloc(FILE *stream, char *file, int line) ;

void error_realloc(FILE *stream, char *file, int line) ;

void error_fseek(FILE *stream, char *file, int line) ;

void error_strtol(FILE *stream, char *file, int line) ;

void error_readline(FILE *stream, char *file, int line, int status) ;

void error_read_stdin(FILE *stream, char *file, int line) ;

void error_read_file(FILE *stream, char *file, int line) ;

void error_read_fasta_entry(FILE *stream, char *file, int line, int error_type) ;

void error_read_matrix_file(FILE *stream, char *file, int line, int error_type) ;

void error_reverse_matrix(FILE *stream, char *file, int line) ;

void error_rescale_matrix(FILE *stream, char *file, int line) ;

void error_write_result(FILE *stream, char *file, int line, int status) ;

void error_get_compl_seq(FILE *stream, char *file, int line, int status) ;


#endif // _ERROR_H
