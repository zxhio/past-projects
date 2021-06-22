; Load GDTR
[global gdt_flush]
gdt_flush:
	mov	eax,	[esp + 4]	; parameter -> eax
	lgdt	[eax]

	mov	ax,	0x10
	mov	ds,	ax
	mov	es,	ax
	mov	fs,	ax
	mov	gs,	ax
	mov	ss,	ax
	jmp	0x08:_flush
_flush:
	ret


; Load IDTR
[global idt_flush]
idt_flush:
	mov	eax,	[esp + 4]	; parameter -> eax
	lidt	[eax]
	ret

; Interrupt without error code
%macro ISR_NOERRCODE 1
[global isr%1]
isr%1:
	cli
	push	0
	push	%1
	jmp	isr_common_stub
%endmacro

; Interrupt with error code
%macro ISR_ERRCODE 1
[global isr%1]
isr%1:
	cli
	push	%1
	jmp	isr_common_stub
%endmacro

; Interrupt error code
ISR_NOERRCODE	0
ISR_NOERRCODE	1
ISR_NOERRCODE	2
ISR_NOERRCODE	3
ISR_NOERRCODE	4
ISR_NOERRCODE	5
ISR_NOERRCODE	6
ISR_NOERRCODE	7
ISR_ERRCODE	8
ISR_NOERRCODE	9
ISR_ERRCODE	10
ISR_ERRCODE	11
ISR_ERRCODE	12
ISR_ERRCODE	13
ISR_ERRCODE	14
ISR_NOERRCODE	15
ISR_NOERRCODE	16
ISR_ERRCODE	17
ISR_NOERRCODE	18
ISR_NOERRCODE	19
ISR_NOERRCODE	20

ISR_NOERRCODE	21
ISR_NOERRCODE	22
ISR_NOERRCODE	23
ISR_NOERRCODE	24
ISR_NOERRCODE	25
ISR_NOERRCODE	26
ISR_NOERRCODE	27
ISR_NOERRCODE	28
ISR_NOERRCODE	29
ISR_NOERRCODE	30
ISR_NOERRCODE	31

ISR_NOERRCODE	255

[global isr_common_stub]
[extern isr_handler]
isr_common_stub:
	pusha	; edi, esi, ebp, esp, ebx, edx, ecx, eax
	mov	ax,	ds
	push	eax			; sava ds

	mov	ax,	0x10		; load system data segment
	mov	ds,	ax
	mov	es,	ax
	mov	fs,	ax
	mov	gs,	ax
	mov	ss,	ax

	push	esp			; pt_regs pointer
	call	isr_handler
	add	esp,	4

	pop	ebx			; recover data segment
	mov	ds,	bx
	mov	es,	bx
	mov	fs,	bx
	mov	gs,	bx
	mov	ss,	bx

	popa
	add	esp,	8		; clean error code and isr
	iret


; IRQ macro
%macro IRQ 2
[GLOBAL irq%1]
irq%1:
	cli
	push 0
	push %2
	jmp irq_common_stub
%endmacro

IRQ   0,    32			; system timer
IRQ   1,    33			; keyboard controller
IRQ   2,    34			; cascaded signal from IRQs 8-15
IRQ   3,    35			; serial port controller for serial port 2 
IRQ   4,    36			; serial port controller for serial port 1
IRQ   5,    37			; parallel port 2 and 3 or sround card
IRQ   6,    38			; floppy disk controller
IRQ   7,    39			; parallel port 1, used for printers 
IRQ   8,    40			; real-time clock
IRQ   9,    41		 	; Advanced Configuration and Power Interface
IRQ  10,    42			; The interrupt is left open for the use of peripherals
IRQ  11,    43			; The interrupt is left open for the use of peripherals
IRQ  12,    44			; mouse on PS/2 connector
IRQ  13,    45			; CPU cp-processor or integrated floating point uint or inter-processor interrupt
IRQ  14,    46			; primary ATA channel
IRQ  15,    47			;secondary ATA channel

[GLOBAL irq_common_stub]
[EXTERN irq_handler]
irq_common_stub:
	pusha				; pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
	
	mov ax, ds
	push eax			; sava user data segment
	
	mov ax, 0x10			; load system data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	push esp
	call irq_handler
	add esp, 4
	
	pop ebx			; recover data segment
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx
	mov ss, bx
	
	popa				; Pops edi,esi,ebp...
	add esp, 8			; clean up error code and ISR code
	iret				; pop CS, EIP, EFLAGS, SS, ESP
