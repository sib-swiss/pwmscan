/* Allows to calculate a hash from a char or to get the char code back from a hash.
   Since only the capital letters are of interest, the hash domain is [0,25].
*/

#ifndef _HASH_H
    #define _HASH_H 1

    /* there can be only 26 capital letters thus 26 different hash codes */
    #define NB_LETTER 26
    #define CHAR_HASH(chr) ((chr - 'A') % NB_LETTER)
    #define GET_CHAR(hash) ( 'A' + hash )

    /* Often use thus given as a constant to avoid losing time for computing it. */
    #define HASH_N CHAR_HASH('N')
#endif // _HASH_H
