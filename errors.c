/*
    Contains functions that are related to error handling for matrix_scan.
*/

#include <stdio.h>
#include <stdlib.h>


void error_fopen(FILE *stream, char *file, int line, char *file2)
{   /*  Write a message in case of fopen() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.
        char *file2  : the file which fopen() failed to open.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */

    fprintf(stream, "*** File %s could not be open at src %s:%d ***\n" ,file2, file, line) ;
}


void error_malloc(FILE *stream, char *file, int line)
{   /*  Write a message in case of malloc() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */

    fprintf(stream, "*** malloc() error at src %s:%d ***\n", file, line) ;
}


void error_calloc(FILE *stream, char *file, int line)
{   /*  Write a message in case of calloc() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */

    fprintf(stream, "*** calloc() error at src %s:%d ***\n", file, line) ;
}


void error_realloc(FILE *stream, char *file, int line)
{   /*  Write a message in case of realloc() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */
    fprintf(stream, "*** realloc() error at src %s:%d ***\n", file, line) ;
}


void error_fseek(FILE *stream, char *file, int line)
{   /*  Write a message in case of fseek() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */

    fprintf(stream, "*** fseek() error at src %s:%d ***\n", file, line) ;
}


void error_strtol(FILE *stream, char *file, int line)
{   /*  Write a message in case of strtol() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */
    fprintf(stream, "*** strtol() error at src %s:%d ***\n", file, line) ;
}


void error_readline(FILE *stream, char *file, int line, int status)
{   /*  Write a message in case of readline() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.
        int status   : readline() status (the given argument) value after readline() call.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */

    fprintf(stream, "*** readline() error (code %d) at src %s:%d ***\n", status, file, line) ;
}


void error_read_fasta_entry(FILE *stream, char *file, int line, int error_type)
{   /*  Write a message in case of read_fasta_entry() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.
        error_type   : 1 if the file is empty, 2 if no first header was found, 3 for unregular reading termination.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */
    switch(error_type)
    {   case -1 :   fprintf(stream, "*** Cannot read a FASTA entry. File is empty at src %s:%d ***\n", file, line) ;
                    break ;

        case -2 :   fprintf(stream, "*** Cannot read a FASTA entry. First file line is not a header at src %s:%d\n", file, line) ;
                    break ;

        case -3 :   fprintf(stream, "*** read_fasta_entry() error at src %s:%d\n", file, line) ;
                    break ;
    }
}


void error_read_matrix_file(FILE *stream, char *file, int line, int error_type)
{   /*  Write a message in case of read_matrix_file() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.
        error_type   : read_matrix() returned value.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */
    /* memory allocation error */
    if(error_type == -1)
    {   fprintf(stream, "*** read_matrix_file() error at src %s:%d ***\n", file, line) ; }
    /* sscanf error */
    if(error_type == -2)
    {   fprintf(stream, "*** Incorrect matrix format at src %s:%d ***\n", file, line) ; }
}


void error_reverse_matrix(FILE *stream, char *file, int line)
{   /*  Write a message in case of reverse_matrix() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */
    fprintf(stream, "*** reverse_matrix() error at src %s:%d ***\n", file, line) ;
}


void error_rescale_matrix(FILE *stream, char *file, int line)
{   /*  Write a message in case of rescale_matrix() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */
    fprintf(stream, "*** rescale_matrix() error at src %s:%d ***\n", file, line) ;
}


void error_write_result(FILE *stream, char *file, int line, int status)
{   /*  Write a message in case of write_result() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.
        int status   : write_result() returned value.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */
    fprintf(stream, "*** write_result() error (code %d) at src %s:%d ***\n", status, file, line) ; }


void error_get_compl_seq(FILE *stream, char *file, int line, int status)
{   /*  Write a message in case of get_compl_seq() error.

        FILE *stream : the stream where the message should be written.
        char *file   : the src file where this function is called.
        int line     : the src line numbeer where this function is called.

        Still allocated at exit :
                Nothing

        Returns :
                Nothing

    */
        fprintf(stream, "*** get_compl_seq() error (code %d) at src %s:%d ***\n", status, file, line) ;
}

