/*
 * include/linux/mmp_ion.h
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

#ifndef _LINUX_MMP_ION_H
#define _LINUX_MMP_ION_H

#include <linux/types.h>

/**
 * struct mmp_ion_phy_alloc_data - metadata passed from userspace for allocations
 * @size:	width of the allocation
 * @flags:	flags passed to heap
 * @handle:	pointer that will be populated with a cookie to use to refer
 *		to this allocation
 *
 * Provided by userspace as an argument to the ioctl
 */
struct mmp_ion_cont_alloc_data {
	unsigned long offset;
	size_t len;
	struct ion_handle *handle;
};

enum {
	MMP_ION_GET_PHYS,
};

#endif /* _LINUX_ION_H */
