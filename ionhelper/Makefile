#CROSS_COMPILE?=arm-marvell-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar
AS=$(CROSS_COMPILE)as
LD=$(CROSS_COMPILE)ld

ifdef ANDROID
INC_ION_HEADER?=-I../../bionic/libc/kernel/common/ \
 -I../../bionic/libc/kernel/arch-arm/
else
INC_ION_HEADER?=-I.
endif

.PHONY : all clean
all:clean libionhelper.so libionhelper.a

libionhelper.so: ion_helper_lib.o
	$(CC) -s -shared -o $@ $^

libionhelper.a: ion_helper_lib.o
	$(AR) -r $@  $^

ion_helper_lib.o: ion_helper_lib.c
	$(CC) -O3 -Wall -fPIC $(INC_ION_HEADER) -c -o $@ $^


clean:
	rm -f *.o
	rm -f libionhelper.so libionhelper.a
