/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "phycontmem_internal.h"
#include "pmem_helper_lib.h"
#include "ion_helper_lib.h"
#include "phycontmem.h"

struct helper_ops {
	struct mem_handle_phycontmem *(*malloc)(int size);
	int (*free)(struct mem_handle_phycontmem* handle);
	void (*flush_cache)(int mem_fd, unsigned long offset, unsigned long size, int dir);
};

static const struct helper_ops ion_helper = {
	ion_malloc,
	ion_free,
	ion_flush_cache,
};

static const struct helper_ops pmem_helper = {
	pmem_malloc,
	pmem_free,
	pmem_flush_cache,
};

static const struct helper_ops *helper = NULL;

//for bmm like
typedef struct phycontmem_node{
	struct mem_handle_phycontmem* mem;
	struct phycontmem_node* next;
}PHYCONTMEM_NODE;

static PHYCONTMEM_NODE* g_phycontmemlist = NULL;

static pthread_mutex_t g_phycontmemlist_mutex = PTHREAD_MUTEX_INITIALIZER;

static void setup_helper(void)
{
	if (access("/dev/ion", R_OK | W_OK) == 0)
		helper = &ion_helper;
	else
		helper = &pmem_helper;
}

static __inline void list_add_node(PHYCONTMEM_NODE* pnode)
{
	pnode->next = g_phycontmemlist;
	g_phycontmemlist = pnode;
	return;
}

static PHYCONTMEM_NODE* list_find_by_va_range(void* VA)
{
	PHYCONTMEM_NODE* pnode;

	pnode = g_phycontmemlist;
	while(pnode) {
		if((unsigned int)VA >= (unsigned int)pnode->mem->va && (unsigned int)VA < (unsigned int)pnode->mem->va + pnode->mem->size) {
			return pnode;
		}
		pnode = pnode->next;
	}
	return NULL;
}


static PHYCONTMEM_NODE* list_find_by_pa_range(unsigned int PA)
{
	PHYCONTMEM_NODE* pnode;

	pnode = g_phycontmemlist;
	while(pnode) {
		if(PA >= (unsigned int)pnode->mem->pa && PA < (unsigned int)pnode->mem->pa + (unsigned int)pnode->mem->size) {
			return pnode;
		}
		pnode = pnode->next;
	}
	return NULL;
}

static PHYCONTMEM_NODE* list_remove_by_va(void* VA)
{
	PHYCONTMEM_NODE* pPrev = NULL;
	PHYCONTMEM_NODE* pnode;

	pnode = g_phycontmemlist;
	while(pnode) {
		if(VA == pnode->mem->va) {
			break;
		}
		pPrev = pnode;
		pnode = pnode->next;
	}
	if(pnode != NULL) {
		if(pPrev) {
			pPrev->next = pnode->next;
		}else{
			g_phycontmemlist = pnode->next;
		}
	}
	return pnode;
}

void* phy_cont_malloc(int size, int attr)
{
	PHYCONTMEM_NODE* pnode;
	void* VA = NULL;
	const char* devname;

	if (!helper)
		setup_helper();

	if(attr == PHY_CONT_MEM_ATTR_DEFAULT) {
		devname = MARVELL_MEMDEV_NAME_CACHEBUFFERED;
	}else if (attr == PHY_CONT_MEM_ATTR_NONCACHED) {
		devname = MARVELL_MEMDEV_NAME_NONCACHED;
	}else if (attr == PHY_CONT_MEM_ATTR_WC) {
		devname = MARVELL_MEMDEV_NAME_WC;
	}else{
		return NULL;
	}

	pnode = (PHYCONTMEM_NODE*)malloc(sizeof(PHYCONTMEM_NODE));
	if(pnode == NULL) {
		return NULL;
	}

	pnode->mem = helper->malloc(size);
	if(pnode->mem == NULL) {
		free(pnode);
		return NULL;
	}
	VA = pnode->mem->va;

	pthread_mutex_lock(&g_phycontmemlist_mutex);
	list_add_node(pnode);
	pthread_mutex_unlock(&g_phycontmemlist_mutex);

	return VA;
}

void phy_cont_free(void* VA)
{
	PHYCONTMEM_NODE* pnode;
	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_remove_by_va(VA);
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	if(pnode) {
		helper->free(pnode->mem);
		free(pnode);
	}
	return;
}

unsigned int phy_cont_getpa(void* VA)
{
	PHYCONTMEM_NODE* pnode;
	unsigned int PA = 0;
	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_find_by_va_range(VA);
	if(pnode) {
		PA = ((unsigned int)VA - (unsigned int)pnode->mem->va) + (unsigned int)pnode->mem->pa;
	}
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	return PA;
}


void* phy_cont_getva(unsigned int PA)
{
	PHYCONTMEM_NODE* pnode;
	void* VA = NULL;
	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_find_by_pa_range(PA);
	if(pnode) {
		VA = (void*)((PA-(unsigned int)pnode->mem->pa) + (unsigned int)pnode->mem->va);
	}
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	return VA;
}

void phy_cont_flush_cache(void* VA, int dir)
{
	PHYCONTMEM_NODE* pnode;
	int mem_dir;
	if(dir == PHY_CONT_MEM_FLUSH_BIDIRECTION) {
		mem_dir = MEM_FLUSH_BIDIRECTION;
	}else if(dir == PHY_CONT_MEM_FLUSH_TO_DEVICE) {
		mem_dir = MEM_FLUSH_TO_DEVICE;
	}else if(dir == PHY_CONT_MEM_FLUSH_FROM_DEVICE) {
		mem_dir = MEM_FLUSH_FROM_DEVICE;
	}else{
		return;
	}

	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_find_by_va_range(VA);
	if (pnode == NULL) {
		pthread_mutex_unlock(&g_phycontmemlist_mutex);
		return;
	}
	helper->flush_cache(pnode->mem->fd, 0, pnode->mem->size, mem_dir);
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	return;
}

void phy_cont_flush_cache_range(void* VA, unsigned long size, int dir)
{
	PHYCONTMEM_NODE* pnode;
	int mem_dir;
	if(dir == PHY_CONT_MEM_FLUSH_BIDIRECTION) {
		mem_dir = MEM_FLUSH_BIDIRECTION;
	}else if(dir == PHY_CONT_MEM_FLUSH_TO_DEVICE) {
		mem_dir = MEM_FLUSH_TO_DEVICE;
	}else if(dir == PHY_CONT_MEM_FLUSH_FROM_DEVICE) {
		mem_dir = MEM_FLUSH_FROM_DEVICE;
	}else{
		return;
	}


	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_find_by_va_range(VA);
	if (pnode == NULL) {
		pthread_mutex_unlock(&g_phycontmemlist_mutex);
		return;
	}
	helper->flush_cache(pnode->mem->fd, ((unsigned long)VA-(unsigned long)pnode->mem->va), size, mem_dir);
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	return;
}
