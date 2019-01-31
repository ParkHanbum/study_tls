#include "common.h"

__thread int tls_variable __attribute__((tls_model("initial-exec")));

int *__tlsvar_location(void)
{
	return &tls_variable;
}

void *do_stuff(void *arg)
{
	long tid = *(int *)arg;
	while(1) {
		tlsvar++;
		if (tlsvar > loop_count) break;
	}

	printf("[%02d] %p = %d\n", tid, &tlsvar, tlsvar);
	printf("Check accessability of address : %d\n",
			*(int *)(&tlsvar));
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
