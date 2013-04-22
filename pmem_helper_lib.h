/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */


#ifndef __PMEM_HELPER_LIB_H__
#define __PMEM_HELPER_LIB_H__

#ifdef __cplusplus
extern "C"
{
#endif

struct mem_handle_mrvl
{
	int fd;
	unsigned int size;
	void* va;
	void* pa;
};

typedef struct mem_handle_mrvl MEM_HANDLE_MRVL;

#define MARVELL_MEMDEV_NAME_CACHEBUFFERED	"/dev/pmem"	//cacheable & buffered
#define MARVELL_MEMDEV_NAME_NONCACHED		"/dev/pmem_adsp"	//non-cacheable & non-buffered
#define MARVELL_MEMDEV_NAME_WC		"/dev/pmem_wc"		//non-cacheable & buffered

#define MEM_FLUSH_BIDIRECTION		0
#define MEM_FLUSH_TO_DEVICE		1
#define MEM_FLUSH_FROM_DEVICE		2

struct mem_handle_mrvl* mem_malloc(int size, const char* devname);	//return handle. if NULL, fail
int mem_free(struct mem_handle_mrvl* handle);		//return 0 is ok, other value fail
void mem_flush_cache(int mem_fd, unsigned long offset, unsigned long size, int dir);	//dir should be PMEM_FLUSH_BIDIRECTION, PMEM_FLUSH_TO_DEVICE or PMEM_FLUSH_FROM_DEVICE


#ifdef __cplusplus
}
#endif

#endif
