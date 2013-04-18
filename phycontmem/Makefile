#CROSS_COMPILE?=arm-marvell-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar
AS=$(CROSS_COMPILE)as
LD=$(CROSS_COMPILE)ld

ifdef PMEM
INC_MEM_HELPER_HEADER=-I../pmemhelper/
CFLAGS += -I../pmemhelper/ -L../pmemhelper/
LIBMEM = -lpmemhelper
else
INC_MEM_HELPER_HEADER=-I../ionhelper/
CFLAGS += -g -I../ionhelper/ -L../ionhelper/
LIBMEM = -lionhelper
endif

LDLIBS += $(LIBMEM) -lrt

.PHONY : all clean
all:clean libphycontmem.so libphycontmem.a

libphycontmem.so: phycontmem.o
	$(CC) $(CFLAGS) $(LDLIBS) -s -shared -o $@ $^

libphycontmem.a: phycontmem.o
	$(AR) -r $@  $^

phycontmem.o: phycontmem.c
	$(CC) -g -Wall -fPIC $(INC_MEM_HELPER_HEADER) -c -o $@ $^


clean:
	rm -f *.o
	rm -f libphycontmem.so libphycontmem.a
