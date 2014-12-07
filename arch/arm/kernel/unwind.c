/*
 * arch/arm/kernel/unwind.c
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
 *
 *
 * Stack unwinding support for ARM
 *
 * An ARM EABI version of gcc is required to generate the unwind
 * tables. For information about the structure of the unwind tables,
 * see "Exception Handling ABI for the ARM Architecture" at:
 *
 * http://infocenter.arm.com/help/topic/com.arm.doc.subset.swdev.abi/index.html
 */

#ifndef __CHECKER__
#if !defined (__ARM_EABI__)
#warning Your compiler does not have EABI support.
#warning    ARM unwind is known to compile only with EABI compilers.
#warning    Change compiler or disable ARM_UNWIND option.
#elif (__GNUC__ == 4 && __GNUC_MINOR__ <= 2)
#warning Your compiler is too buggy; it is known to not compile ARM unwind support.
#warning    Change compiler or disable ARM_UNWIND option.
#endif
#endif 

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/export.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/list.h>

#include <asm/stacktrace.h>
#include <asm/traps.h>
#include <asm/unwind.h>

void __aeabi_unwind_cpp_pr0(void)
{
};
EXPORT_SYMBOL(__aeabi_unwind_cpp_pr0);

void __aeabi_unwind_cpp_pr1(void)
{
};
EXPORT_SYMBOL(__aeabi_unwind_cpp_pr1);

void __aeabi_unwind_cpp_pr2(void)
{
};
EXPORT_SYMBOL(__aeabi_unwind_cpp_pr2);

struct unwind_ctrl_block {
	unsigned long vrs[16];		
	const unsigned long *insn;	
	int entries;			
	int byte;			
};

enum regs {
#ifdef CONFIG_THUMB2_KERNEL
	FP = 7,
#else
	FP = 11,
#endif
	SP = 13,
	LR = 14,
	PC = 15
};

extern const struct unwind_idx __start_unwind_idx[];
static const struct unwind_idx *__origin_unwind_idx;
extern const struct unwind_idx __stop_unwind_idx[];

static DEFINE_SPINLOCK(unwind_lock);
static LIST_HEAD(unwind_tables);

#define prel31_to_addr(ptr)				\
({							\
				\
	long offset = (((long)*(ptr)) << 1) >> 1;	\
	(unsigned long)(ptr) + offset;			\
})

static const struct unwind_idx *search_index(unsigned long addr,
				       const struct unwind_idx *start,
				       const struct unwind_idx *origin,
				       const struct unwind_idx *stop)
{
	unsigned long addr_prel31;

	pr_debug("%s(%08lx, %p, %p, %p)\n",
			__func__, addr, start, origin, stop);

	if (addr < (unsigned long)start)
		
		stop = origin;
	else
		
		start = origin;

	
	addr_prel31 = (addr - (unsigned long)start) & 0x7fffffff;

	while (start < stop - 1) {
		const struct unwind_idx *mid = start + ((stop - start) >> 1);

		if (addr_prel31 - ((unsigned long)mid - (unsigned long)start) <
				mid->addr_offset)
			stop = mid;
		else {
			
			addr_prel31 -= ((unsigned long)mid -
					(unsigned long)start);
			start = mid;
		}
	}

	if (likely(start->addr_offset <= addr_prel31))
		return start;
	else {
		pr_warning("unwind: Unknown symbol address %08lx\n", addr);
		return NULL;
	}
}

static const struct unwind_idx *unwind_find_origin(
		const struct unwind_idx *start, const struct unwind_idx *stop)
{
	pr_debug("%s(%p, %p)\n", __func__, start, stop);
	while (start < stop) {
		const struct unwind_idx *mid = start + ((stop - start) >> 1);

		if (mid->addr_offset >= 0x40000000)
			
			start = mid + 1;
		else
			
			stop = mid;
	}
	pr_debug("%s -> %p\n", __func__, stop);
	return stop;
}

static const struct unwind_idx *unwind_find_idx(unsigned long addr)
{
	const struct unwind_idx *idx = NULL;
	unsigned long flags;

	pr_debug("%s(%08lx)\n", __func__, addr);

	if (core_kernel_text(addr)) {
		if (unlikely(!__origin_unwind_idx))
			__origin_unwind_idx =
				unwind_find_origin(__start_unwind_idx,
						__stop_unwind_idx);

		
		idx = search_index(addr, __start_unwind_idx,
				   __origin_unwind_idx,
				   __stop_unwind_idx);
	} else {
		
		struct unwind_table *table;

		spin_lock_irqsave(&unwind_lock, flags);
		list_for_each_entry(table, &unwind_tables, list) {
			if (addr >= table->begin_addr &&
			    addr < table->end_addr) {
				idx = search_index(addr, table->start,
						   table->origin,
						   table->stop);
				
				list_move(&table->list, &unwind_tables);
				break;
			}
		}
		spin_unlock_irqrestore(&unwind_lock, flags);
	}

	pr_debug("%s: idx = %p\n", __func__, idx);
	return idx;
}

static unsigned long unwind_get_byte(struct unwind_ctrl_block *ctrl)
{
	unsigned long ret;

	if (ctrl->entries <= 0) {
		pr_warning("unwind: Corrupt unwind table\n");
		return 0;
	}

	ret = (*ctrl->insn >> (ctrl->byte * 8)) & 0xff;

	if (ctrl->byte == 0) {
		ctrl->insn++;
		ctrl->entries--;
		ctrl->byte = 3;
	} else
		ctrl->byte--;

	return ret;
}

static int unwind_exec_insn(struct unwind_ctrl_block *ctrl)
{
	unsigned long insn = unwind_get_byte(ctrl);

	pr_debug("%s: insn = %08lx\n", __func__, insn);

	if ((insn & 0xc0) == 0x00)
		ctrl->vrs[SP] += ((insn & 0x3f) << 2) + 4;
	else if ((insn & 0xc0) == 0x40)
		ctrl->vrs[SP] -= ((insn & 0x3f) << 2) + 4;
	else if ((insn & 0xf0) == 0x80) {
		unsigned long mask;
		unsigned long *vsp = (unsigned long *)ctrl->vrs[SP];
		int load_sp, reg = 4;

		insn = (insn << 8) | unwind_get_byte(ctrl);
		mask = insn & 0x0fff;
		if (mask == 0) {
			pr_warning("unwind: 'Refuse to unwind' instruction %04lx\n",
				   insn);
			return -URC_FAILURE;
		}

		
		load_sp = mask & (1 << (13 - 4));
		while (mask) {
			if (mask & 1)
				ctrl->vrs[reg] = *vsp++;
			mask >>= 1;
			reg++;
		}
		if (!load_sp)
			ctrl->vrs[SP] = (unsigned long)vsp;
	} else if ((insn & 0xf0) == 0x90 &&
		   (insn & 0x0d) != 0x0d)
		ctrl->vrs[SP] = ctrl->vrs[insn & 0x0f];
	else if ((insn & 0xf0) == 0xa0) {
		unsigned long *vsp = (unsigned long *)ctrl->vrs[SP];
		int reg;

		
		for (reg = 4; reg <= 4 + (insn & 7); reg++)
			ctrl->vrs[reg] = *vsp++;
		if (insn & 0x80)
			ctrl->vrs[14] = *vsp++;
		ctrl->vrs[SP] = (unsigned long)vsp;
	} else if (insn == 0xb0) {
		if (ctrl->vrs[PC] == 0)
			ctrl->vrs[PC] = ctrl->vrs[LR];
		
		ctrl->entries = 0;
	} else if (insn == 0xb1) {
		unsigned long mask = unwind_get_byte(ctrl);
		unsigned long *vsp = (unsigned long *)ctrl->vrs[SP];
		int reg = 0;

		if (mask == 0 || mask & 0xf0) {
			pr_warning("unwind: Spare encoding %04lx\n",
			       (insn << 8) | mask);
			return -URC_FAILURE;
		}

		
		while (mask) {
			if (mask & 1)
				ctrl->vrs[reg] = *vsp++;
			mask >>= 1;
			reg++;
		}
		ctrl->vrs[SP] = (unsigned long)vsp;
	} else if (insn == 0xb2) {
		unsigned long uleb128 = unwind_get_byte(ctrl);

		ctrl->vrs[SP] += 0x204 + (uleb128 << 2);
	} else {
		pr_warning("unwind: Unhandled instruction %02lx\n", insn);
		return -URC_FAILURE;
	}

	pr_debug("%s: fp = %08lx sp = %08lx lr = %08lx pc = %08lx\n", __func__,
		 ctrl->vrs[FP], ctrl->vrs[SP], ctrl->vrs[LR], ctrl->vrs[PC]);

	return URC_OK;
}

int unwind_frame(struct stackframe *frame)
{
	unsigned long high, low;
	const struct unwind_idx *idx;
	struct unwind_ctrl_block ctrl;

	
	low = frame->sp;
	high = ALIGN(low, THREAD_SIZE);

	pr_debug("%s(pc = %08lx lr = %08lx sp = %08lx)\n", __func__,
		 frame->pc, frame->lr, frame->sp);

	if (!kernel_text_address(frame->pc))
		return -URC_FAILURE;

	idx = unwind_find_idx(frame->pc);
	if (!idx) {
		pr_warning("unwind: Index not found %08lx\n", frame->pc);
		return -URC_FAILURE;
	}

	ctrl.vrs[FP] = frame->fp;
	ctrl.vrs[SP] = frame->sp;
	ctrl.vrs[LR] = frame->lr;
	ctrl.vrs[PC] = 0;

	if (idx->insn == 1)
		
		return -URC_FAILURE;
	else if ((idx->insn & 0x80000000) == 0)
		
		ctrl.insn = (unsigned long *)prel31_to_addr(&idx->insn);
	else if ((idx->insn & 0xff000000) == 0x80000000)
		
		ctrl.insn = &idx->insn;
	else {
		pr_warning("unwind: Unsupported personality routine %08lx in the index at %p\n",
			   idx->insn, idx);
		return -URC_FAILURE;
	}

	
	if ((*ctrl.insn & 0xff000000) == 0x80000000) {
		ctrl.byte = 2;
		ctrl.entries = 1;
	} else if ((*ctrl.insn & 0xff000000) == 0x81000000) {
		ctrl.byte = 1;
		ctrl.entries = 1 + ((*ctrl.insn & 0x00ff0000) >> 16);
	} else {
		pr_warning("unwind: Unsupported personality routine %08lx at %p\n",
			   *ctrl.insn, ctrl.insn);
		return -URC_FAILURE;
	}

	while (ctrl.entries > 0) {
		int urc = unwind_exec_insn(&ctrl);
		if (urc < 0)
			return urc;
		if (ctrl.vrs[SP] < low || ctrl.vrs[SP] >= high)
			return -URC_FAILURE;
	}

	if (ctrl.vrs[PC] == 0)
		ctrl.vrs[PC] = ctrl.vrs[LR];

	
	if (frame->pc == ctrl.vrs[PC])
		return -URC_FAILURE;

	frame->fp = ctrl.vrs[FP];
	frame->sp = ctrl.vrs[SP];
	frame->lr = ctrl.vrs[LR];
	frame->pc = ctrl.vrs[PC];

	return URC_OK;
}

void unwind_backtrace(struct pt_regs *regs, struct task_struct *tsk)
{
	struct stackframe frame;
	register unsigned long current_sp asm ("sp");

	pr_debug("%s(regs = %p tsk = %p)\n", __func__, regs, tsk);

	if (!tsk)
		tsk = current;

	if (regs) {
		frame.fp = regs->ARM_fp;
		frame.sp = regs->ARM_sp;
		frame.lr = regs->ARM_lr;
		
		frame.pc = kernel_text_address(regs->ARM_pc)
			 ? regs->ARM_pc : regs->ARM_lr;
	} else if (tsk == current) {
		frame.fp = (unsigned long)__builtin_frame_address(0);
		frame.sp = current_sp;
		frame.lr = (unsigned long)__builtin_return_address(0);
		frame.pc = (unsigned long)unwind_backtrace;
	} else {
		
		frame.fp = thread_saved_fp(tsk);
		frame.sp = thread_saved_sp(tsk);
		frame.lr = 0;
		frame.pc = thread_saved_pc(tsk);
	}

	while (1) {
		int urc;
		unsigned long where = frame.pc;

		urc = unwind_frame(&frame);
		if (urc < 0)
			break;
		dump_backtrace_entry(where, frame.pc, frame.sp - 4);
	}
}

struct unwind_table *unwind_table_add(unsigned long start, unsigned long size,
				      unsigned long text_addr,
				      unsigned long text_size)
{
	unsigned long flags;
	struct unwind_table *tab = kmalloc(sizeof(*tab), GFP_KERNEL);

	pr_debug("%s(%08lx, %08lx, %08lx, %08lx)\n", __func__, start, size,
		 text_addr, text_size);

	if (!tab)
		return tab;

	tab->start = (const struct unwind_idx *)start;
	tab->stop = (const struct unwind_idx *)(start + size);
	tab->origin = unwind_find_origin(tab->start, tab->stop);
	tab->begin_addr = text_addr;
	tab->end_addr = text_addr + text_size;

	spin_lock_irqsave(&unwind_lock, flags);
	list_add_tail(&tab->list, &unwind_tables);
	spin_unlock_irqrestore(&unwind_lock, flags);

	return tab;
}

void unwind_table_del(struct unwind_table *tab)
{
	unsigned long flags;

	if (!tab)
		return;

	spin_lock_irqsave(&unwind_lock, flags);
	list_del(&tab->list);
	spin_unlock_irqrestore(&unwind_lock, flags);

	kfree(tab);
}
