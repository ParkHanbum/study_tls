
COMMON_CFLAGS = -g -lpthread -pg
TRACE_CFLAGS = -pg
#TRACE_CFLAGS = -pg -mfentry -mnop-mcount
SO_CFLAGS = -shared -fPIC

TLS_INFO_COMMON_FLAGS = -lelf $(COMMON_CFLAGS)
TLS_INFO_SHARED_FLAGS = -DSHARED $(TLS_INFO_COMMON_FLAGS) $(SO_CFLAGS)
TLS_INFO_BIN_FLAGS = $(TLS_INFO_COMMON_FLAGS)

preconfig:
TARGET := readtlsinfo libhashmap.so libtls.so bench_tls bench_hashmap
TARGET += libreadtlsinfo.so libreadtlsinfo.so.2 dlopen_tls
TARGET += dlopen_common

build: $(TARGET)

readtlsinfo: read_tls_info.c
	gcc $(TLS_INFO_BIN_FLAGS) $^ -o $@ 
libreadtlsinfo.so: read_tls_info.c
	gcc $(TLS_INFO_SHARED_FLAGS) $^ -o $@ 
libreadtlsinfo.so.2: read_tls_info.c
	gcc $(TLS_INFO_SHARED_FLAGS) $^ -o $@ 
libhashmap.so: use_hashmap_instead_tls.c 
	gcc $(SO_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ 
libtls.so: use_tls.c 
	gcc $(SO_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ 
bench_tls: bmt_main.c
	gcc -L. -ltls $(TRACE_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ 
bench_hashmap: bmt_main.c
	gcc -L. -lhashmap -DHASHMAP $(TRACE_CFLAGS) $(COMMON_CFLAGS) $^ -o $@ 
dlopen_common: simple_dlopen.c
	gcc -ldl $(COMMON_CLFAGS) $(TRACE_CFLAGS) $^ -o $@
dlopen_tls: simple_dlopen.c
	gcc -DBMTTLS -ldl $(COMMON_CLFAGS) $(TRACE_CFLAGS) $^ -o $@

all:
	$(MAKE) preconfig
	$(MAKE) build

clean:
	rm libreadtlsinfo.so libreadtlsinfo.so.2
	rm readtlsinfo
	rm libhashmap.so
	rm libtls.so
	rm bench_tls
	rm bench_hashmap
	rm dlopen_tls
	rm dlopen_common

.DEFAULT_GOAL=all
