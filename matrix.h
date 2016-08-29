#ifndef _STDIO_H
    #include <stdio.h>
#endif

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

#ifndef _STRUCT_H
    #include "struct.h"
#endif


#ifndef _MATRIX_H
    #define _MATRIX_H 1

    void print_matrix(const struct matrix *m) ;

    void init_matrix(struct matrix *m) ;

    int reverse_matrix(const struct matrix *m_in, struct matrix *m_rev) ;

    int rescale_matrix(const struct matrix *m_in, struct matrix_rescaled *m_out) ;

    void free_matrix(struct matrix *m) ;

    void free_matrix_rescaled(struct matrix_rescaled *m) ;

#endif // _MATRIX_H
