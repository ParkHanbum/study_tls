#include "common.h"

extern int bmt_hashmap();
extern int bmt_tls();

int main()
{
#ifdef HASHMAP
	bmt_hashmap();
#else
	int test = tlsvar;
	bmt_tls();
#endif
}
