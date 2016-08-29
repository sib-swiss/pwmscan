/*
    Defines the messages to be displayed be the help.
*/

char *help_message_head = "\tmatrix_scan [options] [< file_in] [ > file_out]\n" ;

char *help_message_opt  = "\tOptions :\n" ;

char *help_message_i    = "\t\t -i \t sequence file path\n"
                          "\t\t\t File containing the DNA sequence to scan.\n"
                          "\t\t\t If this option is not set, the program reads from stdin.\n"
                          "\t\t\t This sequence is supposed to be in FASTA format, however any\n"
                          "\t\t\t format consisting of a single FASTA header line followed by the\n"
                          "\t\t\t sequence characters is also supported.\n"
                          "\t\t\t The sequence must be ASCII characters. Space characters\n"
                          "\t\t\t are ignored. Lowercase characters are converted to uppercase.\n"
                          "\t\t\t Any non 'ACGT' character is converted to 'N'.\n"
                          "\t\t\t Only 'ACGT' containing subsequences will be scored. If a stretch\n"
                          "\t\t\t of sequence contains one or more 'N's, it is not scored.\n";

char *help_message_m    = "\t\t -m \t matrix file path\n"
                          "\t\t\t File containing a vertical integer Position Weight Matrix (PWM),\n"
                          "\t\t\t i.e. the file columns represent the matrix rows.\n"
                          "\t\t\t The matrix file column order must be: 'A' 'C' 'G' 'T'.\n"
                          "\t\t\t The file may contain a header line though it is not used.\n";

char *help_message_c    = "\t\t -c \t an integer cut-off score\n" ;

char *help_message_d    = "\t\t -d \t delimiter number (by default 2)\n"
                          "\t\t\t The program will parse the FASTA header to find a sequence identifier\n"
                          "\t\t\t (ID) starting from a '|' char to the next space char. Giving '-d 0'\n"
                          "\t\t\t means that the sequence ID begins at the header first character.\n"
                          "\t\t\t By default the sequence  ID is supposed starting after the second '|'\n"
                          "\t\t\t but this behaviour can be overridden with this option. If the\n"
                          "\t\t\t corresponding '|' can not be found no sequence ID will be extracted.\n"
                          "\t\t\t If the no space is found, the sequence ID will end at the last header\n"
                          "\t\t\t char.\n"
                          "\t\t\t i.e with a header '>field0|field1|field2_1 ; field2_2' and '-d 1', the\n"
                          "\t\t\t extracted sequence ID will be 'field1|field2_1', with '-d 0'\n"
                          "\t\t\t '>field0|field1|field2_1' will be extracted.\n"
                          "\t\t\t If no identifier could be extracted, the default sequence ID is 'chrN'.\n" ;

char *help_message_h    = "\t\t -h \t shows this help text\n" ;

char *help_message_gen  = "\tThe matrix_scan program scans a DNA sequence with a Position Weight Matrix (PWM)\n"
                          "\tand returns all subsequences having a score higher or equal to a specified cut-off.\n"
                          "\tmatrix_scan has been otpimized for speed and relies on dynamic memory allocation.\n" ;
