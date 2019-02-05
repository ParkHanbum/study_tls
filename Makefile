
COMMON_CFLAGS = -g -lpthread
ifeq ($(TRACE), 1)
  COMMON_CFLAGS += -O0
  TRACE_CFLAGS := -pg
else
  COMMON_CFLAGS += -O2 
endif

# TRACE_CFLAGS += -pg -mfentry -mnop-mcount
SO_CFLAGS = -shared -fPIC

TLS_INFO_COMMON_FLAGS = -lelf $(COMMON_CFLAGS)
TLS_INFO_SHARED_FLAGS = -DSHARED $(TLS_INFO_COMMON_FLAGS) $(SO_CFLAGS)
TLS_INFO_BIN_FLAGS = $(TLS_INFO_COMMON_FLAGS)

preconfig:
TARGET := readtlsinfo libhashmap.so bench_hashmap
TARGET += libreadtlsinfo.so libreadtlsinfo.so.2
TARGET += dlopen_common dlopen_tls
TARGET += libtlsdyn.so libtlsinit.so bench_tlsinit bench_tlsdyn
TARGET += libtlsspec.so bench_tlsspec

build: $(TARGET)

readtlsinfo: read_tls_info.c
	gcc $(TLS_INFO_BIN_FLAGS) $^ -o $@ 
libreadtlsinfo.so: read_tls_info.c
	gcc $(TLS_INFO_SHARED_FLAGS) $^ -o $@ 
libreadtlsinfo.so.2: read_tls_info.c
	gcc $(TLS_INFO_SHARED_FLAGS) $^ -o $@ 
libhashmap.so: use_hashmap_instead_tls.c 
	gcc $(SO_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ 
libtlsinit.so: use_tls_init.c 
	gcc $(SO_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ 
bench_tlsinit: bmt_main.c
	gcc $(TRACE_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ -L. -ltlsinit 
libtlsdyn.so: use_tls_dynamic.c 
	gcc $(SO_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ 
bench_tlsdyn: bmt_main.c
	gcc $(TRACE_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ -L. -ltlsdyn 
libtlsspec.so: use_tls_specific.c 
	gcc $(SO_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ 
bench_tlsspec: bmt_main.c
	gcc $(TRACE_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ -L. -ltlsspec
bench_hashmap: bmt_main.c
	gcc -DHASHMAP $(TRACE_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ -L. -lhashmap 
dlopen_common: simple_dlopen.c
	gcc $(COMMON_CLFAGS) $(TRACE_CFLAGS) $^ -o $@ -ldl 
dlopen_tls: simple_dlopen.c
	gcc -DBMTTLS $(COMMON_CLFAGS) $(TRACE_CFLAGS) $^ -o $@ -ldl 

all:
	$(MAKE) preconfig
	$(MAKE) build

clean:
	rm *.so *.so.2
	rm readtlsinfo
	rm bench_tlsdyn
	rm bench_tlsinit
	rm bench_tlsspec
	rm bench_hashmap
	rm dlopen_tls
	rm dlopen_common

.DEFAULT_GOAL=all
