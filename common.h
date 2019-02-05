#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <gelf.h>

#define loop_count 500000


static pthread_key_t gkey;

extern __thread int tls_variable;
extern int *__tlsvar_location (void) __attribute__((__const__));
# define tlsvar (*__tlsvar_location ())

