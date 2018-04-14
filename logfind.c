/** \brief logfind - search files for text
 *
 *  logfind [-o] patterns
 *  A specialized version of 'grep'
 *  Default mode is logical and for patterns (-o for logical or)
 *  Output: matching files
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <glob.h>
#include "dbg.h"

#define MAX_LENGTH 81

const char * conf_file_path = "lf_paths";

int main(int argc, char * argv[]) {
    char * s;
    bool and_mode = true;

    while (--argc > 0 && (* ++argv)[0] == '-')
        for (s = argv[0] + 1; *s != '\0'; s++)
            switch (*s) {
                case 'o':
                    and_mode = false;
                    break;
                default:
                    printf("Illegal option %c\n", *s);
                    argc = 0;
                    break;
            }
    if (argc < 1)
        printf("Usage: logfind [-o] pattern ...\n");
    else {
        int i = 0;
        char arguments[argc][MAX_LENGTH];
        for (i = 0; i < argc; i++) {
            strncpy(arguments[i], *argv++, MAX_LENGTH);
        }
        printf("Logical mode: %s\n", and_mode ? "and" : "or");
        for (i = 0; i < argc; i++)
            printf("Argument #%d: %s\n", i, arguments[i]);
    }

    return 0;
}