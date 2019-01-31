#include "common.h"
#include "hashmap.c"

Hashmap *map;

void *do_stuff(void *arg)
{
	int val;
	long tcb_self = pthread_self();
	while(1) {
		// hashmap will return NULL when key has not putted yet.
		void *res = hashmapGet(map, &tcb_self);
		if (res == NULL) {
			val = 1;
			hashmapPut(map, &tcb_self, &val);
		}
		else {
			*(int *)res += 1;
			if (*(int *)res > loop_count) {
				return NULL;
			}
		}
	}
}

int bmt_hashmap()
{
	pthread_t ts[10];
	for(int i = 0; i < 10; i++) {
		pthread_create(&ts[i], NULL, do_stuff, NULL);
	}

	for(int i = 0; i < 10; i++) {
		pthread_join(ts[i], NULL);
	}
}

__attribute__((constructor))
void init(void)
{
	map = hashmapCreate(50000, hashmapDefaultHash, hashmapDefaultEquals);
}

#ifdef DEBUG
int main()
{
	bmt_hashmap();
}
#endif
