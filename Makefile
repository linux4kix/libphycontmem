VERSION=2.0

#CROSS_COMPILE?=arm-marvell-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar
AS=$(CROSS_COMPILE)as
LD=$(CROSS_COMPILE)ld

LDLIBS += $(LIBMEM) -lrt

.PHONY : all dynamic clean

dynamic: libphycontmem.so libpmemhelper.so

all:clean dynamic libphycontmem.a

libphycontmem.so: phycontmem.o ion_helper_lib.o pmem_helper_lib.o
	$(CC) $(CFLAGS) $(LDLIBS) -shared -o $@ $^

# The Marvell-built flash plugin links against the old (no longer provided)
# libpmemhelper but doesn't use any of its symbols. Keep the linker happy by
# providing an empty library with the same name.
libpmemhelper.so: emptylib.o
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

libphycontmem.a: phycontmem.o
	$(AR) -r $@  $^

%.o: %.c
	$(CC) $(CFLAGS) -I. -fPIC -c -o $@ $^

clean:
	rm -f *.o lib*.so* libphycontmem.a

tarball:
	git archive --format=tar --prefix=libphycontmem-$(VERSION)/ HEAD | gzip > libphycontmem-$(VERSION).tar.gz
