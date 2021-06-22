/*
 * Created by zxh on 2018/01/21 . All rights reserved.
 *
 * Interrupt Service Routine, and Interrupt Request routine
 */
#ifndef YEQU_INCLUDE_IDT_H
#define YEQU_INCLUDE_IDT_H

#include <types.h>


void idt_init();

/* Gate descriptor */
struct gate_desc {
	uint16_t base_low;
	uint16_t sel;
	uint8_t  always0;
	uint8_t  flags; 
	uint16_t base_high;
}__attribute__((packed));

// IDTR
struct idt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

/* protected registers */
struct pt_regs {
	uint32_t ds;			/* save user ds segment */
	uint32_t edi; 		/* instruction pusha, push sequence */
	uint32_t esi; 
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t intr_no; 		/* interrupt code */
	uint32_t err_code;  		/* error code */
	uint32_t eip;			/* the fllowing is done by cpu */
	uint32_t cs; 		
	uint32_t eflags;
	uint32_t useresp;
	uint32_t ss;
};

/* interrupt handler pointer */
typedef void (*interrupt_handler_t)(struct pt_regs *);

/* interrupt handler pointer array */
interrupt_handler_t interrupt_handlers[256];

/* register interrupt handler */
void register_interrupt_handler(uint8_t n, interrupt_handler_t h);

/* interrupt handler */
void isr_handler(struct pt_regs *regs);


/* interrupt handler 0 - 19 is part of CPU exception interrupt */
/* ISR: interrupt service routine */
					/* Exception Class. Description */
void isr0(); 				/* Fault. Divide Error Exception (#DE) */ 
void isr1(); 				/* Trap or Fault. Debug Exception (#DB) */ 
void isr2(); 				/* None. NMI Interrupt */ 
void isr3(); 				/* Trap. Breakpoint Exception (#BP) */ 
void isr4(); 				/* Trap. Overflow Exception (#OF) */ 
void isr5(); 				/* Fault. BOUND Range Exceeded Exception (#BR) */
void isr6(); 				/* Fault. Invalid Opcode Exception (#UD) */ 
void isr7(); 				/* Fault. Device Not Available Exception (#NM) */
void isr8(); 				/* Abort. Double Fault Exception (#DF) */
void isr9(); 				/* Abort. Coprocessor Segment Overrun */
void isr10(); 			/* Fault. Invalid TSS Exception (#TS) */
void isr11(); 			/* Fault. Segment Not Present (#NP) */
void isr12(); 			/* Fault. Stack Fault Exception (#SS) */
void isr13(); 			/* Fault. General Protection Exception (#GP) */
void isr14(); 			/* Fault. Page-Fault Exception (#PF) */
void isr15(); 			/* reserve */
void isr16(); 			/* Fault. FPU Floating-Point Error (#MF) */
void isr17(); 			/* Fault. Alignment Check Exception (#AC) */
void isr18(); 			/* Abort. Machine-Check Exception (#MC) */
void isr19(); 			/* Fault. SIMD Floating-Point Exception (#XM) */
void isr20(); 			/* Fault. Virtualization Exception (#VE) */

void isr21();				/* 21-31 Intel reserve */
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

void isr255();			/* 32-255 cUser Defined Interrupts */

/* IRQ Handler */
void irq_handler(struct pt_regs *regs);

/* 
 * IRQ, Intel 8259A.
 * reference: https://en.m.wikipedia.org/wiki/interrupt_request_(PC_architecture)
 */
#define  IRQ0     32 		/* system timer */
#define  IRQ1     33 		/* keyboard controller */
#define  IRQ2     34 		/* cascaded signal from IRQs 8-15 */
#define  IRQ3     35 		/* serial port controller for serial port 2 */
#define  IRQ4     36 		/* serial port controller for serial port 1 */
#define  IRQ5     37 		/* parallel port 2 and 3 or sround card */
#define  IRQ6     38 		/* floppy disk controller */
#define  IRQ7     39 		/* parallel port 1, used for printers */
#define  IRQ8     40 		/* real-time clock */
#define  IRQ9     41 		/* Advanced Configuration and Power Interface */
#define  IRQ10    42 		/* The interrupt is left open for the use of peripherals */
#define  IRQ11    43 		/* The interrupt is left open for the use of peripherals */
#define  IRQ12    44 		/* mouse on PS/2 connector */
#define  IRQ13    45 		/* CPU cp-processor or integrated floating point uint or inter-processor interrupt */
#define  IRQ14    46 		/* primary ATA channel */
#define  IRQ15    47 		/* secondary ATA channel */

/* IRQ:(Interrupt Request) */
void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();


#endif 	/* YEQU_INCLUDE_IDT_H */
