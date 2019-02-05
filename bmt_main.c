#include "common.h"
#include <time.h>

extern int bmt_hashmap();
extern int bmt_tls();
extern void *do_stuff(void *arg);

int main()
{
	pthread_t ts[10];
	for(int i = 0; i < 5; i++) {
		pthread_create(&ts[i], NULL, do_stuff, &ts[i]);
	}

	for(int i = 0; i < 5; i++) {
		pthread_join(ts[i], NULL);
	}
}
