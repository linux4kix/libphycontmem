/*
 * include/uapi/linux/insecure_uapi_ion.h
 *
 * Copyright (C) 2011 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _LINUX_INSECURE_UAPI_ION_H
#define _LINUX_INSECURE_UAPI_ION_H

#include <linux/types.h>

/**
 * struct insecure_uapi_ion_phy_alloc_data - metadata passed from userspace for allocations
 * @size:	width of the allocation
 * @flags:	flags passed to heap
 * @handle:	pointer that will be populated with a cookie to use to refer
 *		to this allocation
 *
 * Provided by userspace as an argument to the ioctl
 */
struct insecure_uapi_ion_cont_alloc_data {
	unsigned long offset;
	size_t len;
	int dma_buf;
};

enum ion_heap_ids {
	INVALID_HEAP_ID = -1,
	ION_SYSTEM_HEAP_ID = 4,
	ION_SYSTEM_CONTIG_HEAP_ID = 8,
	ION_CARVEOUT_HEAP_ID = 12,
	ION_CHUNK_HEAP_ID = 16,
	ION_DMA_HEAP_ID = 20,
};

#define ION_VMALLOC_HEAP_NAME		"vmalloc"
#define ION_SYSTEM_CONTIG_HEAP_NAME	"system_config"
#define ION_CARVEOUT_HEAP_NAME		"carveout"
#define ION_CHUNK_HEAP_NAME		"chunk"
#define ION_DMA_HEAP_NAME		"dma"

enum {
	INSECURE_UAPI_ION_GET_PHYS,
};

#define ION_FLAG_INSECURE	 16   /* Use this flag to designate that 
                                         */

#endif /* _LINUX_INSECURE_UAPI_ION_H */
