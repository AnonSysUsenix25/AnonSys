/*
 * Copyright (C) 2012 Regents of the University of California
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

#ifndef _ASM_RISCV_PGTABLE_64_H
#define _ASM_RISCV_PGTABLE_64_H

#include <linux/const.h>

#define PGDIR_SHIFT     30
/* Size of region mapped by a page global directory */
#define PGDIR_SIZE      (_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK      (~(PGDIR_SIZE - 1))

#define PMD_SHIFT       21
/* Size of region mapped by a page middle directory */
#define PMD_SIZE        (_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK        (~(PMD_SIZE - 1))

/* Page Middle Directory entry */
typedef struct {
	unsigned long pmd;
} pmd_t;

#define pmd_val(x)      ((x).pmd)
#define __pmd(x)        ((pmd_t) { (x) })

#define PTRS_PER_PMD    (PAGE_SIZE / sizeof(pmd_t))

static inline int pud_present(pud_t pud)
{
	return (pud_val(pud) & _PAGE_PRESENT);
}

static inline int pud_none(pud_t pud)
{
	return (pud_val(pud) == 0);
}

static inline int pud_bad(pud_t pud)
{
	return !pud_present(pud);
}

static inline void set_pud(pud_t *pudp, pud_t pud)
{
	*pudp = pud;
}

static inline void pud_clear(pud_t *pudp)
{
	set_pud(pudp, __pud(0));
}

static inline unsigned long pud_page_vaddr(pud_t pud)
{
	return (unsigned long)pfn_to_virt(pud_val(pud) >> _PAGE_PFN_SHIFT);
}

#define pmd_index(addr) (((addr) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))

static inline pmd_t *pmd_offset(pud_t *pud, unsigned long addr)
{
	pmd_t *ret = (pmd_t *)pud_page_vaddr(*pud) + pmd_index(addr);
#ifdef CONFIG_DEP_LD_REORDER
	// Alpha has a fence here
	smp_read_barrier_depends();
#endif
	return ret;
}

static inline pmd_t pfn_pmd(unsigned long pfn, pgprot_t prot)
{
	return __pmd((pfn << _PAGE_PFN_SHIFT) | pgprot_val(prot));
}

#define pmd_ERROR(e) \
	pr_err("%s:%d: bad pmd %016lx.\n", __FILE__, __LINE__, pmd_val(e))

#endif /* _ASM_RISCV_PGTABLE_64_H */
