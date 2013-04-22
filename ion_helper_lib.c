/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/dma-direction.h>
#include <linux/ion.h>
#include <linux/mmp_ion.h>

#include "phycontmem_internal.h"
#include "ion_helper_lib.h"

#ifdef ANDROID
#define LOG_TAG "mem_helper"
#include <cutils/log.h>
#else
#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...) fprintf( stderr, __VA_ARGS__)
#define LOGE(...)
#endif

#if 1
#define mem_helper_echo(...)	printf(__VA_ARGS__)
#else
#define mem_helper_echo(...)
#endif

static int ion_open()
{
        int fd = open("/dev/ion", O_RDWR);
        if (fd < 0)
                LOGE("open /dev/ion failed!\n");
        return fd;
}

static int ion_close(int fd)
{
        return close(fd);
}

static int ion_ioctl(int fd, int req, void *arg)
{
        int ret = ioctl(fd, req, arg);
        if (ret < 0) {
                LOGE("ioctl %x failed with code %d: %s\n", req,
                       ret, strerror(errno));
                return -errno;
        }
        return ret;
}

static int ion_alloc(int fd, size_t len, size_t align, unsigned int heap_mask,
	      unsigned int flags, struct ion_handle **handle)
{
        int ret;
        struct ion_allocation_data data = {
                .len = len,
                .align = align,
		.heap_id_mask = heap_mask,
                .flags = flags,
        };

        ret = ion_ioctl(fd, ION_IOC_ALLOC, &data);
        if (ret < 0)
                return ret;
        *handle = data.handle;
        return ret;
}

static int _ion_free(int fd, struct ion_handle *handle)
{
        struct ion_handle_data data = {
                .handle = handle,
        };
        return ion_ioctl(fd, ION_IOC_FREE, &data);
}

static int ion_map(int fd, struct ion_handle *handle, size_t length, int prot,
            int flags, off_t offset, unsigned char **ptr, int *map_fd)
{
        struct ion_fd_data data = {
                .handle = handle,
        };

        int ret = ion_ioctl(fd, ION_IOC_MAP, &data);
        if (ret < 0)
                return ret;
        *map_fd = data.fd;
        if (*map_fd < 0) {
                LOGE("map ioctl returned negative fd\n");
                return -EINVAL;
        }
        *ptr = mmap(NULL, length, prot, flags, *map_fd, offset);
        if (*ptr == MAP_FAILED) {
                LOGE("mmap failed: %s\n", strerror(errno));
                return -errno;
        }
        return ret;
}

static int ion_share(int fd, struct ion_handle *handle, int *share_fd)
{
        struct ion_fd_data data = {
                .handle = handle,
        };

        int ret = ion_ioctl(fd, ION_IOC_SHARE, &data);
        if (ret < 0)
                return ret;
        *share_fd = data.fd;
        if (*share_fd < 0) {
                LOGE("share ioctl returned negative fd\n");
                return -EINVAL;
        }
        return ret;
}

static int ion_alloc_fd(int fd, size_t len, size_t align, unsigned int heap_mask,
		 unsigned int flags, int *handle_fd) {
	struct ion_handle *handle;
	int ret;

	ret = ion_alloc(fd, len, align, heap_mask, flags, &handle);
	if (ret < 0)
		return ret;
	ret = ion_share(fd, handle, handle_fd);
	_ion_free(fd, handle);
	return ret;
}

static int ion_import(int fd, int share_fd, struct ion_handle **handle)
{
        struct ion_fd_data data = {
                .fd = share_fd,
        };

        int ret = ion_ioctl(fd, ION_IOC_IMPORT, &data);
        if (ret < 0)
                return ret;
        *handle = data.handle;
        return ret;
}

static int ion_sync_fd(int fd, int handle_fd)
{
    struct ion_fd_data data = {
        .fd = handle_fd,
    };
    return ion_ioctl(fd, ION_IOC_SYNC, &data);
}

struct mem_handle_mrvl* ion_malloc(int size)
{
	struct mem_handle_mrvl* mem;
	struct ion_custom_data data;
	struct mmp_ion_cont_alloc_data alloc_data;
	int rlt = 0;

	LOGI("%s() calling, sz %d\n", __FUNCTION__, size);

	mem = (struct mem_handle_mrvl*)malloc( sizeof(struct mem_handle_mrvl) );
	if( NULL == mem ) {
		mem_helper_echo("malloc in %s(line %d) fail\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	memset( mem, 0, sizeof(struct mem_handle_mrvl) );
	mem->fd = ion_open();
	if( mem->fd < 0 ) {
		mem_helper_echo("open in %s(line %d) fail, ret fd %d\n", __FUNCTION__, __LINE__, mem->fd);
		goto mem_malloc_fail0;
	}

	rlt = ion_alloc(mem->fd, (size_t)size, 0, ION_HEAP_SYSTEM_CONTIG_MASK, 0, &mem->handle);
	if( rlt < 0 ) {
		mem_helper_echo("ion_alloc in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, rlt);
		goto mem_malloc_fail1;
	}

	rlt = ion_map(mem->fd, mem->handle, size, PROT_READ|PROT_WRITE,
		      MAP_SHARED, 0, &mem->va, &mem->map_fd);
	if( rlt < 0 ) {
		mem_helper_echo("ion_alloc in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, rlt);
		goto mem_malloc_fail2;
	}

	mem->size = size;

	data.cmd = MMP_ION_GET_PHYS;
	alloc_data.len = size;
	alloc_data.handle = mem->handle;
	data.arg = (unsigned long int)&alloc_data;
	
	rlt = ioctl(mem->fd, ION_IOC_CUSTOM, (unsigned long)&data);
	if( rlt < 0 ) {
		mem_helper_echo("MEM_GET_PHYS in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, rlt);
		goto mem_malloc_fail3;
	}

	mem->pa = (void *)alloc_data.offset;

	LOGI("%s() ok, sz %d, va 0x%08x, pa 0x%08x, fd %d, handle 0x%08x\n", __FUNCTION__, size, (unsigned int)mem->va, (unsigned int)mem->pa, mem->fd, (unsigned int)mem);

	return mem;

mem_malloc_fail3:
	munmap( mem->va, mem->size );
mem_malloc_fail2:
	close(mem->map_fd);
	_ion_free(mem->fd, mem->handle);
mem_malloc_fail1:
	ion_close( mem->fd );
mem_malloc_fail0:
	free( mem );
	LOGI("%s() fail, sz %d\n", __FUNCTION__, size);
	return NULL;
}

int ion_free(struct mem_handle_mrvl* handle)
{
	LOGI("%s() calling, handle 0x%08x\n", __FUNCTION__, (unsigned int)handle);
	if(handle == NULL) {
		return -1;
	}
	if(handle->fd < 0) {
		return -2;
	}
	munmap( handle->va, handle->size );
	close(handle->map_fd);
	_ion_free(handle->fd, handle->handle);
	ion_close(handle->fd);
	free(handle);
	LOGI("%s() ok, handle 0x%08x\n", __FUNCTION__, (unsigned int)handle);
	return 0;
}

void ion_flush_cache(int mem_fd, unsigned long offset, unsigned long size, int dir)
{
}

