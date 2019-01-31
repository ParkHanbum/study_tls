#define _GNU_SOURCE
#include <link.h>
#include <string.h>
#include <libelf.h>
#include <unistd.h>
#include <linux/limits.h>
#include <errno.h>
#include <pthread.h>

#include "common.h"

extern int err;

struct tls {
	long d;
	int a;
	int b;
	int c;
};

__thread struct tls find_me = {
	.d = 0x0a0b0c0d0e0f0102L,
	.a = 0x03040506,
	.b = 0x0708090a,
	.c = 0x0b0c0d0e,
};

#define err(msg)				\
({						\
	printf("[%d] %s\n", errno, msg);	\
	exit(1);				\
 })

char *read_exename(void)
{
	int len;
	static char exename[PATH_MAX];

	if (!*exename) {
		len = readlink("/proc/self/exe", exename, sizeof(exename)-1);
		if (len < 0)
			err("cannot read executable name");

		exename[len] = '\0';
	}

	printf("%s\n", exename);
	return exename;
}

const char *reltype_tostr(GElf_Word type)
{
	switch(type) {
		case R_X86_64_DTPOFF64:
			return "DTPOFF64";
		case R_X86_64_DTPMOD64:
			return "DTPMOD64";
		default:
			return "DONTCARE";
	}
}

void show_map(void)
{
	FILE *fp;
	char buf[PATH_MAX];

	fp = fopen("/proc/self/maps", "r");
	if (fp == NULL)
		return;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		char *p = buf, *next;
		unsigned long start, end;

		start = strtoul(p, &next, 16);
		if (*next != '-')
			printf("invalid format\n");

		p = next + 1;
		end = strtoul(p, &next, 16);

		if (strstr(next, "libreadtlsinfo")) {
			printf(" %lx - %lx \n", start, end);
		}
	}
}

static long moduleid;
static uintptr_t module_addr;

struct dtv_pointer
{
	void *val;
	void *to_free;
};

typedef union dtv
{
	size_t counter;
	struct dtv_pointer pointer;
} dtv_t;

dtv_t *get_dtv(void)
{
	void *tcb = (void *)pthread_self();
	dtv_t *dtv = (dtv_t *)(*(uintptr_t *)(tcb + sizeof(uintptr_t)));
	return dtv;
}

void show_dtv(void)
{
	void *tcb = (void *)pthread_self();
	printf("tcb : %p\n", tcb);
	dtv_t *dtv = (dtv_t *)(*(uintptr_t *)(tcb + sizeof(uintptr_t)));
	printf("dtv : %p\n", dtv);

	int count = dtv[0].counter;
	printf("Total dtv count : %d\n", count);
	for(int i = 0;i <= moduleid;i++) {
		struct dtv_pointer dtv_el = dtv[i].pointer;
		printf("dtv [%d] : %llx - %llx \n", i, dtv_el.val, dtv_el.to_free);
	}

	if (dtv[moduleid].counter <= 0) {
		printf("not allocated yet\n");
		return;
	}

	if (moduleid > 0) {
		struct dtv_pointer module_dtv = dtv[moduleid].pointer;
		void *val = module_dtv.val;
		printf("Module value located at %p, value : %llx\n", val, *(unsigned long *)val);
	}
}

static long tls_addr;
static int tls_size;

static int do_stuff(struct dl_phdr_info *info, size_t size, void *data)
{
	int j;
	static int once = 0;
	long mid;
	long maddr;
	long _tls_addr;
	int _tls_size;


	printf("[%d] [%lx] : %s \t location at 0x%lx\n",
			size, info->dlpi_tls_modid,
			info->dlpi_name,
			info->dlpi_addr);

	mid = info->dlpi_tls_modid;
	maddr = info->dlpi_addr;


	// Display TLS Section.
	long phnum = info->dlpi_phnum;
	GElf_Phdr *phdr = (GElf_Phdr *)info->dlpi_phdr;
	for (int phidx=0; phidx < phnum; phidx++) {
		GElf_Phdr *_phdr = phdr + phidx;
		if (_phdr->p_type == PT_TLS) {
			printf("[PT_TLS] Virtual Address : %llx - %llx\n",
					_phdr->p_vaddr, _phdr->p_memsz);

			char *ptls = (char *)_phdr->p_vaddr + maddr;
			for(int pos=0;pos < _phdr->p_memsz;pos++) {
				printf("0x%x ", (unsigned char)*(ptls+pos));
			}
			printf("\n");
			_tls_addr = info->dlpi_addr + _phdr->p_vaddr;
			_tls_size = _phdr->p_memsz;
		}

	}

	if (strstr(info->dlpi_name, "libreadtlsinfo") != NULL) {
		tls_addr = _tls_addr;
		tls_size = _tls_size;
		moduleid = mid;
		if (module_addr == 0)
			module_addr = maddr;
	}


	return 0;
}

void allocate_dtv(void)
{
	void *tls_alloc = malloc(tls_size);

	printf("TLS ALLOC : %p\n", tls_alloc);
	if (tls_addr != 0)
		memcpy(tls_alloc, (void *)tls_addr, tls_size);

	dtv_t *dtv = get_dtv();
	dtv[0].counter += 1;
	dtv[moduleid].pointer.val = tls_alloc;
	dtv[moduleid].pointer.to_free = tls_alloc;
}

void print_rela(Elf_Data *edata, int nr)
{
	GElf_Sym symbol;

	for(int i=0;i < nr;i++) {
		gelf_getsym(edata, i, &symbol);
	}
}

void print_symbol(Elf_Data *edata, int nr)
{
	GElf_Rela rela;

	for(int i=0;i < nr;i++) {
		gelf_getrela(edata, i, &rela);
		GElf_Word symidx = GELF_R_SYM(rela.r_info);
		GElf_Word type = GELF_R_TYPE(rela.r_info);
		printf("%x - %d - %s\n", rela.r_offset, symidx, reltype_tostr(type));
	}
}


#ifdef SHARED
__attribute__((constructor))
void init()
#else
int main()
#endif
{
	int fd;
	char *section_name, path[256];
	size_t shstrndx;
	Elf* pElf;
	Elf_Scn *scn = NULL;
	Elf64_Shdr *shdr;

	// find moduleid for libreadtlsinfo.so
	dl_iterate_phdr(do_stuff, NULL);
	printf("MODULE ID\t:[%d]\n", moduleid);
	printf("Print DTV information before refer tls variable\n");
	show_dtv();
	allocate_dtv();
	struct tls *test = &find_me;
	printf("Print DTV information after refer tls variable\n");
	printf("TLS variable address : %p\n", test);
	show_dtv();
	find_me.d = 0x0a0b0c0d0e0f0001L;
	printf("Print DTV information after allocate\n");
	show_dtv();

	show_map();
	printf("version %d\n", elf_version(EV_CURRENT));

}

