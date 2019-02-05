#include "common.h"

__thread int tls_variable;

int *__tlsvar_location(void)
{
	return &tls_variable;
}

void *do_stuff(void *arg)
{
	long tid = *(int *)arg;
	while(1) {
		tls_variable++;
		if (tls_variable > loop_count) break;
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
