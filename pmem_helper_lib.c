/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/android_pmem.h>
#include <linux/dma-direction.h>

#include "phycontmem_internal.h"
#include "pmem_helper_lib.h"

#ifdef ANDROID
#define LOG_TAG "pmem_helper"
#include <cutils/log.h>
#else
#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

#if 0
#define mem_helper_echo(...)	printf(__VA_ARGS__)
#else
#define mem_helper_echo(...)
#endif

struct mem_handle_mrvl* pmem_malloc(int size)
{
	struct mem_handle_mrvl* mem;
	struct pmem_region pr;
	int rlt = 0;

	LOGI("%s() calling, sz %d\n", __FUNCTION__, size);

	mem = (struct mem_handle_mrvl*)malloc( sizeof(struct mem_handle_mrvl) );
	if( NULL == mem ) {
		mem_helper_echo("malloc in %s(line %d) fail\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	memset( mem, 0, sizeof(struct mem_handle_mrvl) );
	mem->fd = open( MARVELL_MEMDEV_NAME_NONCACHED, O_RDWR );
	if( mem->fd < 0 ) {
		mem_helper_echo("open in %s(line %d) fail, ret fd %d\n", __FUNCTION__, __LINE__, mem->fd);
		goto mem_malloc_fail0;
	}

	mem->va = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, mem->fd, 0);
	if( mem->va == MAP_FAILED ) {
		mem_helper_echo("mmap in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, mem->va);
		goto mem_malloc_fail1;
	}
	mem->size = size;

	rlt = ioctl( mem->fd, PMEM_GET_PHYS, (unsigned long)&pr);
	if( rlt < 0 ) {
		mem_helper_echo("PMEM_GET_PHYS in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, rlt);
		goto mem_malloc_fail2;
	}

	mem->pa = (void*)pr.offset;

	LOGI("%s() ok, sz %d, va 0x%08x, pa 0x%08x, fd %d, handle 0x%08x\n", __FUNCTION__, size, (unsigned int)mem->va, (unsigned int)mem->pa, mem->fd, (unsigned int)mem);

	return mem;

mem_malloc_fail2:
	munmap( mem->va, mem->size );
mem_malloc_fail1:
	close( mem->fd );
mem_malloc_fail0:
	free( mem );
	LOGI("%s() fail, sz %d\n", __FUNCTION__, size);
	return NULL;
}

int pmem_free(struct mem_handle_mrvl* handle)
{
	LOGI("%s() calling, handle 0x%08x\n", __FUNCTION__, (unsigned int)handle);
	if(handle == NULL) {
		return -1;
	}
	if(handle->fd < 0) {
		return -2;
	}
	munmap( handle->va, handle->size );
	close(handle->fd);
	free(handle);
	LOGI("%s() ok, handle 0x%08x\n", __FUNCTION__, (unsigned int)handle);
	return 0;
}

void pmem_flush_cache(int mem_fd, unsigned long offset, unsigned long size, int dir)
{
	struct pmem_sync_region psr;
	int ret;
	if(mem_fd < 0)
		return;
	psr.region.offset = offset;
	psr.region.len = size;

	if(dir == MEM_FLUSH_BIDIRECTION) {
		psr.dir = DMA_BIDIRECTIONAL;
	}else if(dir == MEM_FLUSH_TO_DEVICE) {
		psr.dir = DMA_TO_DEVICE;
	}else if(dir == MEM_FLUSH_FROM_DEVICE) {
		psr.dir = DMA_FROM_DEVICE;
	}else{
		return;
	}

	if (psr.dir == DMA_BIDIRECTIONAL) {
		ret = ioctl(mem_fd, PMEM_CACHE_FLUSH, (unsigned long)&psr.region);
		if( ret < 0 ) {
			mem_helper_echo("PMEM_CACHE_FLUSH in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, ret);
		}
	} else {
		ret = ioctl(mem_fd, PMEM_MAP_REGION, (unsigned long)&psr);
		if( ret < 0 ) {
			mem_helper_echo("PMEM_CACHE_FLUSH in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, ret);
		}
	}
}

