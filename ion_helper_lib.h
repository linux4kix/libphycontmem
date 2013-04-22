/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */


#ifndef __ION_HELPER_LIB_H__
#define __ION_HELPER_LIB_H__

#include <linux/ion.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct mem_handle_mrvl
{
	int fd;
	int map_fd;
	unsigned int size;
	void* va;
	void* pa;
	struct ion_handle* handle;
};

typedef struct mem_handle_mrvl MEM_HANDLE_MRVL;

#define MARVELL_MEMDEV_NAME_CACHEBUFFERED	""	//cacheable & buffered
#define MARVELL_MEMDEV_NAME_NONCACHED		""	//non-cacheable & non-buffered
#define MARVELL_MEMDEV_NAME_WC		""		//non-cacheable & buffered

#define MEM_FLUSH_BIDIRECTION		0
#define MEM_FLUSH_TO_DEVICE		1
#define MEM_FLUSH_FROM_DEVICE		2

struct mem_handle_mrvl* mem_malloc(int size, const char* devname);	//return handle. if NULL, fail
int mem_free(struct mem_handle_mrvl* handle);		//return 0 is ok, other value fail
void mem_flush_cache(int mem_fd, unsigned long offset, unsigned long size, int dir);	//dir should be MEM_FLUSH_BIDIRECTION, MEM_FLUSH_TO_DEVICE or MEM_FLUSH_FROM_DEVICE


#ifdef __cplusplus
}
#endif

#endif
