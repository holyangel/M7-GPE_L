/*
 * arch/arm/include/asm/unwind.h
 *
 * Copyright (C) 2008 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __ASM_UNWIND_H
#define __ASM_UNWIND_H

#ifndef __ASSEMBLY__

enum unwind_reason_code {
	URC_OK = 0,			
	URC_CONTINUE_UNWIND = 8,
	URC_FAILURE = 9			
};

struct unwind_idx {
	unsigned long addr_offset;
	unsigned long insn;
};

struct unwind_table {
	struct list_head list;
	const struct unwind_idx *start;
	const struct unwind_idx *origin;
	const struct unwind_idx *stop;
	unsigned long begin_addr;
	unsigned long end_addr;
};

extern struct unwind_table *unwind_table_add(unsigned long start,
					     unsigned long size,
					     unsigned long text_addr,
					     unsigned long text_size);
extern void unwind_table_del(struct unwind_table *tab);
extern void unwind_backtrace(struct pt_regs *regs, struct task_struct *tsk);

#endif	

#ifdef CONFIG_ARM_UNWIND
#define UNWIND(code...)		code
#else
#define UNWIND(code...)
#endif

#endif	
