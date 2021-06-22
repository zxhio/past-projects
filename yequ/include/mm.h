/*
 * Created by zxh on 2018/05/04 . All rights reserved.
 */

#ifndef YEQU_INCLUDE_MM_H
#define YEQU_INCLUDE_MM_H

#include <types.h>


/* 0x00000 - 0x100000 is the system low memory */
#define MEM_LOW		0x100000 

/* 0x100000 - 0x8000000 */
#define MEM_HIGH	0x1000000

/* 15M free physical memory to page */
#define PAGE_MEM_SIZE	(15* 1024 * 1024)

/* page number */
#define PAGE_NUMBER	(PAGE_MEM_SIZE >> 12)

/* page size */
#define PAGE_SIZE	0x1000

/* 
 * the transformation of address and page index 
 * 1-15M used paging
 */
#define ADDR_TO_PG_IDX(addr)	(((addr) - MEM_LOW) >> 12)
#define PG_IDX_TO_ADDR(page)	(((page) << 12) + MEM_LOW)

/* the flags page is not free */
#define PAGE_USED	1

/* 
 * the following struct PAGE, PTE, PDE  
 * reference the Intel Manual, Volume 3A: System Programming Guide, Part 1,
 * 4.3 32-BIT PAGING
 */

/* flags of a 32-Bit Page-Directory Entry that references a Page Table */
#define PDE_P	0x01		/* Present */
#define PDE_W	0x02		/* Read/Write */
#define PDE_U	0x04		/* User/Supervior */
#define PDE_A	0x20		/* Accessed */
#define PDE_PS	0x80		/* if CR4.PSE = 1, must be 0 */

/* flags of a 32-Bit Page-Table Entry that reference a Maps a 4-KB Page */
#define PTE_P	0x01
#define PTE_W	0x02
#define PTE_U	0x04
#define PTE_A	0x20
#define PTE_D	0x40		/* Dirty */
#define PTE_PAT 0x80		/* if the PAT is supported, indirectly determines the memory type
				   used to access the 4-KB Page. */

/* page table entry */
struct pte {
	uint32_t pte_flags	: 9;
	uint32_t pte_avl	: 3;
	uint32_t pte_base	: 20;
};

/* page directory entry */
struct pde {
	uint32_t pde_flags	: 9;
	uint32_t pde_avl	: 3;
	uint32_t pde_base	: 20;
};

/* physical page, 4KB. */
struct page {
	uint32_t pg_idx;
	uint32_t pg_ref_cnt;
	struct page *pg_next;
};

void mm_init();
#endif /* YEQU_INCLUDE_M_H */
