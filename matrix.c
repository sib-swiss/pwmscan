/*
    Contains functions which are involved in handling matrix struct.
*/

#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "struct.h"



void print_matrix(const struct matrix *m)
{   /*  Prints a matrix struct.

        struct matrix *m : the address of the struct matrix to print

        Still allocated on exit :
                Nothing

        Returns :
                Nothing
    */
    int nrow = m->nrow ;
    int ncol = m->ncol ;

    for(int irow=0; irow<nrow; irow++)
    {   for(int icol=0; icol<ncol; icol++)
        {   printf("%8d", m->index[icol][irow]) ; }
        printf("\n") ;
    }
    printf("\n\n") ;
}


void init_matrix(struct matrix *m)
{   /*  Initialize a matrix fields with null values.

        struct matrix *m : the address of the struct matrix to initialize.

        Still allocated on exit :
                Nothing

        Returns :
                Nothing
    */
    m->nrow = 0 ;
    m->ncol = 0 ;
    m->index = NULL ;
}


int reverse_matrix(const struct matrix *m_in, struct matrix *m_rev)
{   /*  Given a struct matrix m_in (nrow, ncol), it will invert the order of all the
        columns. This reverted matrix data are a copy of the input
        matrix and are stored in m_rev.

        m_in        m_rev

        1   2       4   3
        3   4       2   1

        However, since some rows are here only to allow a random access using the
        base char, only these swaps are needed
          m_in           m_rev
        row  0 (A) --> row  0 (T)
        row  2 (C) --> row  2 (G)
        row  6 (G) --> row  6 (C)
        row 19 (T) --> row 19 (A)

        struct matrix *m_in  : the address of the matrix to reverse.
        struct matrix *m_rev : the addresse were the reversed matrix will be stored.

        Still allocated on exit :
                m_rev->index
                m_rev->index[i]

        Returns :
                    On success 0, -1 otherwise.
    */

    int ncol = m_in->ncol ;
    int nrow = m_in->nrow ;
    /* define m_rev values */
    m_rev->ncol = m_in->ncol ;
    m_rev->nrow = m_in->nrow ;
    m_rev->index = (int**) calloc(ncol, sizeof(int*)) ;
    if(m_rev->index == NULL)
    {   error_calloc(stderr, __TIME__, __LINE__) ;
        return -1 ;
    }

    /*  allocate space for a column
        Index are always calculated compared to m_in.
    */
    for(int i=0; i<ncol; i++)
    {   /* allocate space for a column
           Index are always calculated compared to m_in.
        */
        m_rev->index[ncol-i-1] = (int*) calloc(nrow, sizeof(int)) ;
        if( m_rev->index[ncol-i-1] == NULL)
        {   error_calloc(stderr, __TIME__, __LINE__) ;
            return -1 ;
        }
    }

    /* specifically rewrite rowA (0), rowC(2), rowG(6) and rowT(19)
       to row19, row6, row2 and row0 in new matrice. Leave other as they are.
       row 0  -> row 19
       row 2  -> row 6
       row 6  -> row 2
       row 19 -> row 0
    */
    int index_from[] = {0 , 2, 6, 19} ;
    int index_to[]   = {19, 6, 2, 0}  ;
    for(int i=0; i<4; i++)
    {    for(int icol=0; icol<ncol; icol++)
        {   m_rev->index[ncol-icol-1][index_to[i]] = m_in->index[icol][index_from[i]] ;
        }
    }
    return 0 ;
}


int rescale_matrix(const struct matrix *m_in, struct matrix_rescaled *m_out)
{   /*  Given a matrix m_in, it will rescale its columns to make the maximum value be 0
        and store the rescaling factors with the rescaled matrix in m_out.

        struct matrix *m_in  : the address of the matrix to rescale.
        struct matrix *m_out : the addresse where the rescaled matrix will be stored.

        Still allocated on exit :
                m_out->rescal_factors
                m_out->matrix->index
                m_out->matrix->index[i]

        Returns :
                On success 0, -1 otherwise.
    */

    int nrow = m_in->nrow ;
    int ncol = m_in->ncol ;
    int max = 0 ;

    /* define m_out fields */
    m_out->rescal_factors = (int*) calloc(ncol, sizeof(int)) ;
    if(m_out->rescal_factors == NULL)
    {   error_calloc(stderr, __TIME__, __LINE__) ;
        return -1 ;
    }
    m_out->matrix->ncol = ncol ;
    m_out->matrix->nrow = nrow ;
    m_out->matrix->index = (int**) calloc(ncol, sizeof(int*)) ;
    if(m_out->matrix->index == NULL)
    {   error_calloc(stderr, __TIME__, __LINE__) ;
        return -1 ;
    }

    for(int icol=0; icol<ncol; icol++)
    {   /* allocate memory for a column */
        m_out->matrix->index[icol] = (int*) calloc(nrow, sizeof(int)) ;
        if(m_out->matrix->index[icol] == NULL)
        {   error_calloc(stderr, __TIME__, __LINE__) ;
            return -1 ;
        }

        /* search max in column */
        for(int irow=0; irow<nrow; irow++)
        {   if(irow == 0)
            {   max = m_in->index[icol][irow] ; }
            else
            {   if(m_in->index[icol][irow] > max)
                {   max = m_in->index[icol][irow] ; }
            }
        }

        /* rescale column and store it in m_out */
        for(int irow=0; irow<nrow; irow++)
        {   m_out->matrix->index[icol][irow] = m_in->index[icol][irow] - max ;
            m_out->rescal_factors[icol] = -max ;
        }
    }

    return 0 ;
}


void free_matrix(struct matrix *m)
{   /*  Frees the allocated memory of a matrix struct m.

        struct matrix *m : the address of the matrix to free.

        Still allocated on exit :
                Nothing

        Returns :
                Nothing
    */

    for(int i=0; i<m->ncol; i++)
    {   free(m->index[i]) ; }
    free(m->index) ;
}


void free_matrix_rescaled(struct matrix_rescaled *m)
{   /*  Frees the allocated memory of a matrix_rescaled structure m.

        struct matrix_rescaled *m : the address of the struct matrix_rescaled to free.

        Still allocated on exit :
                Nothing

        Returns :
                Nothing
    */
    free_matrix(m->matrix) ;
    free(m->rescal_factors) ;
}
