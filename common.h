#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <gelf.h>

#define loop_count 500000

extern __thread int tls_variable __attribute__((tls_model("initial-exec")));
extern int *__tlsvar_location (void) __attribute__((__const__));
# define tlsvar (*__tlsvar_location ())

