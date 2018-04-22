/** \brief logfind - search files for text
 *
 *  logfind [-o] patterns
 *  A specialized version of 'grep'
 *  Default mode is logical AND for patterns (-o for logical OR)
 *  Output: matching files
 */
//#define _DEBUG_MODE_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <glob.h>
#include "dbg.h"

#define MAX_LINE 120

const char * conf_file_path = "lf_paths_c";

int glob_files(FILE *fp, glob_t *glob_buf);
int glob_err(const char * path, int eerrno);
void glob_return_err(int return_value);
void find_text(char *pattern[], int size, glob_t *glob_buf, bool and_mode);

int main(int argc, char * argv[]) {
    int i = 0;
    char * s;
    char * pattern[argc];
    bool and_mode = true;
    FILE * fp = NULL;
    glob_t glob_buf;

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
        /* Program main logic */
        int glob_ret = 0;
        for (i = 0; i < argc; i++) {
            pattern[i] = *argv++;
        }
        check(*pattern[0] != '\0', "Empty pattern list.");

        fp = fopen(conf_file_path, "r");
        check(fp != NULL, "Can't open the file: %s\n", conf_file_path);

        glob_ret = glob_files(fp, &glob_buf);
        check(glob_ret == 0, "Can't match files in %s", conf_file_path);
        fclose(fp);

#ifdef _DEBUG_MODE_
        log_info("Count of paths matched so far: %zu", glob_buf.gl_pathc);
        for (i = 0; i < glob_buf.gl_pathc; i++) {
            printf("Path #%3d: %s\n", i + 1, glob_buf.gl_pathv[i]);
        }
        for (i = 0; i < argc; i++) {
            printf("Word to find #%d: %s\n", i + 1, pattern[i]);
        }
#endif

        find_text(pattern, argc, &glob_buf, and_mode);

        globfree(&glob_buf);
    }

    return 0;

error:
    if (fp) fclose(fp);
    return 1;
}

int glob_files(FILE *fp, glob_t *glob_buf)
{
    char path_pattern[MAX_LINE] = {'\0'};
    char * new_line_char;
    int glob_return = 0;
    int glob_flags = GLOB_TILDE;

    while (fgets(path_pattern, MAX_LINE, fp) != NULL) {
        if (path_pattern[0] == '\n' || path_pattern[0] == '#')
            continue;
        if ((new_line_char = strchr(path_pattern, '\n')) != NULL)
            *new_line_char = '\0';

        glob_return = glob(path_pattern, glob_flags, glob_err, glob_buf);
        glob_flags |= GLOB_APPEND;

        if (glob_return != 0 && glob_return != GLOB_NOMATCH)
            glob_return_err(glob_return);
    }

    return (glob_buf->gl_pathc > 0) ? 0 : -1;
}

void find_text(char *pattern[], int size, glob_t *glob_buf, bool and_mode)
{
    int file_id = 0;
    int text_id = 0;
    FILE * fp = NULL;
    char line[MAX_LINE] = {'\0'};
    bool text_match[size];
    bool file_match;
    bool file_inside_match;

    for (file_id = 0; file_id < glob_buf->gl_pathc; file_id++) {
        file_match = false;
        for (text_id = 0; text_id < size; text_id++) {
            text_match[text_id] = false;
        }
        fp = fopen(glob_buf->gl_pathv[file_id], "r");
        if (fp) {
            while (fgets(line, MAX_LINE, fp) != NULL) {
                for (text_id = 0; text_id < size; text_id++) {
                    if (strcasestr(line, pattern[text_id])) {
                        if (and_mode) {
                            text_match[text_id] = true;
                        } else {
                            file_match = true;
                            break;
                        }
                    }
                }

                if (file_match) break;
                file_inside_match = true;
                for (text_id = 0; text_id < size; text_id++) {
                    file_inside_match &= text_match[text_id];
                }
                if (file_inside_match) {
                    file_match = true;
                    break;
                }
            }
        } else {
            log_warn("Failed to open file: %s", glob_buf->gl_pathv[file_id]);
        }

        if (file_match) {
            puts(glob_buf->gl_pathv[file_id]);
        }
        fclose(fp);
    }
}

int glob_err(const char * path, int eerrno)
{
    fprintf(stderr, "Error #%s in glob, path: %s\n", strerror(eerrno), path);
    return 0;
}

void glob_return_err(int return_value)
{
    if (return_value == GLOB_NOSPACE)
        log_err("Glob: Running out of memory");
    else if (return_value == GLOB_ABORTED)
        log_err("Glob: Read error");
    else
        log_err("Glob: Unknown problem");
}
