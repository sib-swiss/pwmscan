/*
     Contains functions which are involved in handling sequence struct.
*/

#include <stdio.h>
#include <stdlib.h>
#include "struct.h"

void init_sequence(struct sequence *seq)
{   /*  Initialize a struct sequence fields with null values.

        struct sequence *seq : the address of the struct sequence to initialize.

        Still allocated on exit :
                Nothing

        Returns :
                Nothing
    */
    seq->length = 0 ;
    seq->header = NULL ;
    seq->seq = NULL ;
}


void free_sequence(struct sequence *seq)
{   /*  Frees the allocated memory of a sequence struct seq.

        struct matrix *seq : the address of the struct sequence to free.

        Still allocated on exit :
                Nothing

        Returns :
                Nothing
    */

    free(seq->header) ;
    free(seq->seq) ;
}

