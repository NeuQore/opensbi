/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Christoph Müllner <cmuellner@linux.com>
 */

#include <sbi/riscv_barrier.h>
#include <sbi/riscv_locks.h>

/*
 * Single-hart CVA6 on AWS F2 HBM: amoadd/lr.sc to DRAM never complete
 * (SmartConnect/HBM do not finish AXI atomics). Ticket locks would hang
 * forever in spin_lock. Interrupts are off during OpenSBI boot.
 */

static inline bool spin_lock_unlocked(spinlock_t lock)
{
	return lock.owner == lock.next;
}

bool spin_lock_check(spinlock_t *lock)
{
	RISCV_FENCE(r, rw);
	return !spin_lock_unlocked(*lock);
}

bool spin_trylock(spinlock_t *lock)
{
	spin_lock(lock);
	return true;
}

void spin_lock(spinlock_t *lock)
{
	(void)lock;
}

void spin_unlock(spinlock_t *lock)
{
	(void)lock;
}
