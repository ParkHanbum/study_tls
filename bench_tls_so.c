#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static __thread int test1 __attribute__ ((tls_model("global-dynamic")));
static __thread int test2 __attribute__ ((tls_model("local-dynamic")));
static __thread int test3 __attribute__ ((tls_model("initial-exec")));

static int count = 500000;
void test_global_dynamic()
{
	int i = 0;
	do {
	int *local = &test1;
	} while(i++ < count);
}
void test_local_dynamic()
{
	int i = 0;
	do {
	int *local = &test2;
	} while(i++ < count);

}

void test_initial_exec()
{
	int i = 0;
	do {
	int *local = &test3;
	} while(i++ < count);

}

#ifndef SHARED
static __thread int test4 __attribute__ ((tls_model("local-exec")));

void test_local_exec()
{
	int i = 0;
	do {
	int *local = &test4;
	} while(i++ < count);
}
#endif

static pthread_key_t __key;
void test_specific()
{
	int i = 0;
	do {
	int *local = pthread_getspecific(__key);
	} while(i++ < count);
}

int main()
{
	test_global_dynamic();
	test_local_dynamic();
	test_initial_exec();
#ifndef SHARED
	test_local_exec();
#endif
	pthread_key_create(&__key, NULL);
	pthread_setspecific(__key, malloc(sizeof(int)));
	test_specific();
}
