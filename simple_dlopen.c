#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char **argv) {
	void *handle;

	handle = dlopen (argv[1], RTLD_NOW);
	if (!handle) {
		fputs(dlerror(), stderr);
		printf("\n");
		exit(1);
	}

	if (argc > 2) {
		handle = dlopen (argv[2], RTLD_LAZY);
		if (!handle) {
			fputs(dlerror(), stderr);
			printf("\n");
			exit(1);
		}
	}

#ifdef BMTTLS
	int (*bmt_tls)(void) =  (int (*)(void))dlsym(handle, "bmt_tls");
	bmt_tls();
#endif
	getchar();
}
