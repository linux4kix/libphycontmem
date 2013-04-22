/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */

#ifndef __PHYCM_INTERNAL_H__
#define __PHYCM_INTERNAL_H__

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

#define MEM_FLUSH_BIDIRECTION		0
#define MEM_FLUSH_TO_DEVICE		1
#define MEM_FLUSH_FROM_DEVICE		2

#endif
