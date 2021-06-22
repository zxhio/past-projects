/*
 * Created by zxh on 2018/05/05 . All rights reserved.
 */

#include <debug.h>
#include <mm.h>
#include <string.h>
#include <types.h>

/* Page Directory default base address is 0x00. */
struct pde page_dir[1024];

struct page pages[PAGE_NUMBER];
struct page* pg_free_list;

/* page directory address 0x00 */
void flush_tlb()
{
	asm volatile("mov %%eax, %%cr3" : : "a"(0));
}

void copy_page(uint32_t old_addr, uint32_t new_addr)
{
	uint32_t* s = (uint32_t*)old_addr;
	uint32_t* d = (uint32_t*)new_addr;
	memmove(s, d, PAGE_SIZE);
}

static inline void out_of_mem()
{
	panic("Out of Memory!");
}

void enable_paging()
{
	int32_t cr0, cr4;

	/* enable 4MB page */
	asm volatile("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= 0x10;
	asm volatile("mov %0, %%cr4" : : "r"(cr4));

	/* enbale paging */
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

void page_init()
{
	uint32_t i;
	uint32_t start_mem, end_mem;
	uint32_t free_page_num;
	struct page* pg_ptr;

	/*
	 * the highest physical memory is END_MEM,
	 * and the memory must be used state beside START_MEM to END_MEM.
	 */
	start_mem = MEM_LOW;
	end_mem = MEM_HIGH;

	for (i = 0; i < PAGE_NUMBER; i++) {
		pages[i].pg_idx = i;
		pages[i].pg_ref_cnt = 1;
		pages[i].pg_next = NULL;
	}

	/* page number of START_MEM */
	i = (uint32_t)ADDR_TO_PG_IDX(start_mem);
	/* calculate the number of free page. */
	free_page_num = (end_mem - start_mem) >> 12;
	pg_ptr = pg_free_list;
	while (free_page_num-- > 0) {
		pages[i].pg_ref_cnt = 0;
		pg_ptr = &pages[i];
		pg_ptr = pg_ptr->pg_next;
	}
}

/* 
 * free page list, return the first list node,
 * and return the page start address.
 */
uint32_t alloc_page()
{
	if (pg_free_list->pg_next == NULL)
		panic("No free Pages!");

	struct page* free_pg;

	free_pg = pg_free_list->pg_next; 	/* first node */
	free_pg->pg_ref_cnt++;			/* reference count add 1. */
	pg_free_list->pg_next = free_pg->pg_next;

	return PG_IDX_TO_ADDR(free_pg->pg_idx);
}

void free_page(uint32_t addr)
{
	struct page *pg;
	
	if (addr < MEM_LOW || addr >= MEM_HIGH)
		return;
	pg = &pages[ADDR_TO_PG_IDX(addr)];
	if (pg->pg_ref_cnt-- <= 0)
		panic("Try to free free page!");
	if (pg->pg_ref_cnt == 0) {
		pg->pg_next = pg_free_list->pg_next;
		pg_free_list->pg_next = pg;
	}
}

/* 
 * put a Page to Page Table and Page Directory,
 * if the Page's is unset (reference count is 0), we should get a new free Page,
 * and page_dir = 0.
 */
uint32_t put_page(uint32_t page, uint32_t addr)
{
	uint32_t *page_table, tmp;
	if (page < MEM_LOW || page >= (uint32_t)MEM_HIGH)
		printk("Trying to put page %x at %x\n", page, addr);

	if (pages[ADDR_TO_PG_IDX(page)].pg_ref_cnt == 0)
		printk("pages disagrees with %x at %x\n", page, addr);
	/* Page Directory address about the ADDR */
	page_table = (uint32_t *)((addr >> 20) & 0xffc);
	printk("page_table = %x, entry = %x\n", page_table, (addr >> 12) & 0x3ff);
	if (*page_table & PTE_P) {
		/* if Page Directory present, calculate Page Table. */
		printk("Page table now available \n");
		page_table = (uint32_t *)(*page_table & 0xfffff000);
	} else {
		/* else alloc new free page and put it in Page Directory. */ 
		tmp = alloc_page();
		*page_table = tmp | PTE_P | PTE_W | PTE_U;
		printk("Tmp = %x\n", tmp);
		printk("Page Table = %x\n", *page_table);
		page_table = (uint32_t *)tmp;
	}
	printk("Put Page Success\n");
	page_table[(addr >> 12) & 0x3ff] = page | PTE_P | PTE_W | PTE_U;
	return page;
}

void page_dir_init(struct pde* ppde)
{
	uint32_t i, kern_num;

	kern_num = (PAGE_MEM_SIZE) / (PAGE_SIZE * 1024);
	for (i = 0; i < kern_num; i++) {
		ppde[i].pde_base = i << 10;
		ppde[i].pde_flags = PDE_P | PDE_W | PDE_PS;
	}

	for (i = kern_num; i < 1024; i++) {
		ppde[i].pde_base = 0;
		ppde[i].pde_flags = PDE_U;
	}
}

void mm_init()
{
	page_init();
	flush_tlb();
	enable_paging();
}
