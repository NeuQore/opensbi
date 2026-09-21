/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/sbi_bitops.h>
#include <sbi/riscv_asm.h>
#include <sbi/riscv_atomic.h>
#include <sbi/riscv_barrier.h>

#ifndef __riscv_atomic
#error "opensbi strongly relies on the A extension of RISC-V"
#endif

/*
 * Uniprocessor substitutes: CVA6 + HBM never complete AMO/LR/SC to DRAM.
 * Do not use amoadd/amoswap/__sync_val_compare_and_swap here.
 */

long atomic_read(atomic_t *atom)
{
	long ret = atom->counter;
	rmb();
	return ret;
}

void atomic_write(atomic_t *atom, long value)
{
	atom->counter = value;
	wmb();
}

long atomic_add_return(atomic_t *atom, long value)
{
	long ret = atom->counter + value;

	atom->counter = ret;
	wmb();
	return ret;
}

long atomic_sub_return(atomic_t *atom, long value)
{
	return atomic_add_return(atom, -value);
}

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	long cur = atom->counter;

	if (cur == oldval)
		atom->counter = newval;
	wmb();
	return cur;
}

long atomic_xchg(atomic_t *atom, long newval)
{
	long old = atom->counter;

	atom->counter = newval;
	wmb();
	return old;
}

unsigned int atomic_raw_xchg_uint(volatile unsigned int *ptr,
				  unsigned int newval)
{
	unsigned int old = *ptr;

	*ptr = newval;
	wmb();
	return old;
}

unsigned long atomic_raw_xchg_ulong(volatile unsigned long *ptr,
				    unsigned long newval)
{
	unsigned long old = *ptr;

	*ptr = newval;
	wmb();
	return old;
}

int atomic_raw_set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	volatile unsigned long *p = &addr[BIT_WORD(nr)];
	unsigned long old = *p;

	*p = old | mask;
	wmb();
	return old & mask ? 1 : 0;
}

int atomic_raw_clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	volatile unsigned long *p = &addr[BIT_WORD(nr)];
	unsigned long old = *p;

	*p = old & ~mask;
	wmb();
	return old & mask ? 1 : 0;
}

int atomic_set_bit(int nr, atomic_t *atom)
{
	return atomic_raw_set_bit(nr, (unsigned long *)&atom->counter);
}

int atomic_clear_bit(int nr, atomic_t *atom)
{
	return atomic_raw_clear_bit(nr, (unsigned long *)&atom->counter);
}
