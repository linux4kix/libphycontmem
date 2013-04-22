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

struct mem_handle_mrvl* ion_malloc(int size);	//return handle. if NULL, fail
int ion_free(struct mem_handle_mrvl* handle);		//return 0 is ok, other value fail
void ion_flush_cache(int mem_fd, unsigned long offset, unsigned long size, int dir);	//dir should be MEM_FLUSH_BIDIRECTION, MEM_FLUSH_TO_DEVICE or MEM_FLUSH_FROM_DEVICE


#ifdef __cplusplus
}
#endif

#endif
