#include <stdio.h>

extern void test_global_dynamic();
extern void test_local_dynamic();
extern void test_initial_exec();
extern void test_specific();

int main()
{
	test_initial_exec();
	test_specific();
	test_local_dynamic();
	test_global_dynamic();

	test_specific();
	test_local_dynamic();
	test_global_dynamic();
	test_initial_exec();

	test_local_dynamic();
	test_global_dynamic();
	test_initial_exec();
	test_specific();

	test_global_dynamic();
	test_initial_exec();
	test_specific();
	test_local_dynamic();
}
