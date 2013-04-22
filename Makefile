VERSION=2.0

#CROSS_COMPILE?=arm-marvell-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar
AS=$(CROSS_COMPILE)as
LD=$(CROSS_COMPILE)ld

LDLIBS += $(LIBMEM) -lrt

.PHONY : all dynamic clean

dynamic: libphycontmem.so.1.0 libphycontmem.so.1 libphycontmem.so

all:clean dynamic libphycontmem.a

libphycontmem.so.1.0: phycontmem.o ion_helper_lib.o
	$(CC) $(CFLAGS) $(LDLIBS) -s -shared -Wl,-soname,libphycontmem.so.1 -o $@ $^

libphycontmem.so.1: libphycontmem.so.1.0
	ln -s $^ $@

libphycontmem.so: libphycontmem.so.1
	ln -s $^ $@

libphycontmem.a: phycontmem.o
	$(AR) -r $@  $^

%.o: %.c
	$(CC) $(CFLAGS) -I. -fPIC -c -o $@ $^

clean:
	rm -f *.o libphycontmem.so* libphycontmem.a

tarball:
	git archive --format=tar --prefix=libphycontmem-$(VERSION)/ HEAD | gzip > libphycontmem-$(VERSION).tar.gz
