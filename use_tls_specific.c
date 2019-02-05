#define _GNU_SOURCE
#include "common.h"
#include <errno.h>
#include <pthread.h>
#include <string.h>

__thread int tls_variable;
__thread int test;

int *__tlsvar_location(void)
{
}

void *do_stuff(void *arg)
{
	pthread_key_t __key;
	long tid = *(int *)arg;
	int test = 0;
	pthread_key_create(&__key, NULL);
	pthread_setspecific(__key, malloc(sizeof(int)));

	int *getval;
	while(1) {
		getval = (int *)pthread_getspecific(__key);
		*getval += 1;
		if (*getval > loop_count)
			break;
	}
}

int bmt_tls()
{
	pthread_t ts[10];
	for(int i = 0; i < 5; i++) {
		pthread_create(&ts[i], NULL, do_stuff, &ts[i]);
	}

	for(int i = 0; i < 5; i++) {
		pthread_join(ts[i], NULL);
	}
}

#ifdef DEBUG
int main()
{
	bmt_tls();
}
#endif
