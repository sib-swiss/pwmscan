/*
    Defines structures to store sequences, matrices and rescaled matrices.
*/

#ifndef _STRUCT_H
    #define _STRUCT_H 1

struct sequence {

    /*
        Structure to store sequence informations.

        length : the sequence length

        header : the sequence label

        seq : the sequence

    */

    long length ;
    char *header ;
    char *seq ;
} ;


struct matrix {
    /*
        Stores a matrix of int (nrow, ncol)
        index :
                an pointer array to each column of the matrix
        ncol :
                the number of columns
        nrow :
                the number of rows

        Standards : ncol C means that the matrix has C columns.
                    To loop over, one can use i<C as limit.
                    nrow R means that the matrix has R rows.
                    To loop over, one can use i<R as limit.
    */
    int  nrow ;
    int  ncol ;
    int **index ;
} ;


struct matrix_rescaled {
    /*
        Contains a rescaled matrix.

        matrix :
                a pointer to the rescaled matrix
        rescal_factor :
                a pointer to a int[] containing each of the
                rescaling factors used to rescale the columns.
                These values are the values which were added to the
                matrix value to rescale it (negative values).
    */

    struct matrix *matrix ;
    int *rescal_factors ;
} ;

#endif // _STRUCT_H
