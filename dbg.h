#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d in %s: " M "\n",\
                                __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#endif      /* NDEBUG */

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d in %s: errno: %s) " M "\n",\
                                 __FILE__, __LINE__, __func__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d in %s: errno: %s) " M "\n",\
                                 __FILE__, __LINE__, __func__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d in %s) " M "\n",\
                                 __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define check(A, M, ...) if (!(A)) { log_err(M, ##__VA_ARGS__); errno = 0; goto error; }

#define sentinel(M, ...) { log_err(M, ##__VA_ARGS__); errno = 0; goto error; }

#define check_mem(A) check((A), "Brak pamięci.")

#define check_debug(A, M, ...) if (!(A)) { debug(M, ##__VA_ARGS__); errno = 0; goto error; }

#endif      /* __dbg_h__ */

