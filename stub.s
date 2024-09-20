.set MAGIC, 0x1badb002
.set FLAGS, 7
.set CHECKSUM, -(MAGIC + FLAGS)
.set MODE_TYPE, 0
.set WIDTH, 1024  
.set HEIGHT, 768  
.set DEPTH, 32 

.set HEADER_ADDR, 0
.set LOAD_ADDR, 0
.set LOAD_END_ADDR, 0
.set BSS_END_ADDR, 0
.set ENTRY_ADDR, 0




.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM
    .long HEADER_ADDR
    .long LOAD_ADDR
    .long LOAD_END_ADDR
    .long BSS_END_ADDR
    .long ENTRY_ADDR
    .long MODE_TYPE
    .long WIDTH
    .long HEIGHT
    .long DEPTH
    .space 4 * 13

.section .text
.extern kernel_main
.global _start

_start:
    mov $kernel_stack, %esp
    mov $kernel_stack, %ecx
    push %eax
    push %ebx
    push %ecx
    call kernel_main

_stop:
    cli
    hlt
    jmp _stop


.section .bss
.space 200*1024*1024; # 200 MiB
kernel_stack:
