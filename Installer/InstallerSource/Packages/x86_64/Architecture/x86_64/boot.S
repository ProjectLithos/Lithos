.set MULTIBOOT_MAGIC, 0x1BADB002
.set MULTIBOOT_FLAGS, 0x00010003

/*
 * The address fields make QEMU load this ELF as a flat image. This intentionally
 * bypasses QEMU's ELF64 rejection while retaining a normal symbol-rich ELF file.
 */
.section .multiboot,"a"
.balign 4
header_start:
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
    .long header_start
    .long __image_start
    .long __load_end
    .long __bss_end
    .long _start

.section .text.boot,"ax"
.code32
.global _start
.type _start, @function
_start:
    cli
    movl %eax, boot_magic
    movl %ebx, boot_information
    movl $stack_top, %esp

    /* Map the first 4 GiB with 2 MiB pages. This includes QEMU framebuffers. */
    movl $page_directory_pointer, %edi
    movl $page_directories, %eax
    orl $0x03, %eax
    movl $4, %ecx
1:
    movl %eax, (%edi)
    movl $0, 4(%edi)
    addl $0x1000, %eax
    addl $8, %edi
    loop 1b

    movl $page_directories, %edi
    xorl %ecx, %ecx
2:
    movl %ecx, %eax
    shll $21, %eax
    orl $0x83, %eax
    movl %eax, (%edi, %ecx, 8)
    movl $0, 4(%edi, %ecx, 8)
    incl %ecx
    cmpl $2048, %ecx
    jne 2b

    movl $page_directory_pointer, %eax
    orl $0x03, %eax
    movl %eax, page_map_level4
    movl $0, page_map_level4 + 4

    movl %cr4, %eax
    orl $0x20, %eax
    movl %eax, %cr4

    movl $0xC0000080, %ecx
    rdmsr
    orl $0x00000100, %eax
    wrmsr

    movl $page_map_level4, %eax
    movl %eax, %cr3

    lgdt gdt_pointer
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    ljmp $0x08, $long_mode_entry

.code64
long_mode_entry:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs
    movq $stack_top, %rsp
    xorq %rbp, %rbp
    movl boot_magic(%rip), %edi
    movl boot_information(%rip), %esi
    call oesdk_runtime_start
3:
    cli
    hlt
    jmp 3b
.size _start, . - _start

.section .rodata.gdt,"a"
.balign 8
gdt:
    .quad 0
    .quad 0x00AF9A000000FFFF
    .quad 0x00AF92000000FFFF
gdt_end:
gdt_pointer:
    .word gdt_end - gdt - 1
    .long gdt

.section .bss,"aw",@nobits
.balign 4
boot_magic:
    .skip 4
boot_information:
    .skip 4

.balign 4096
.global __bootstrap_page_tables_start
.global __bootstrap_page_tables_end
__bootstrap_page_tables_start:
page_map_level4:
    .skip 4096
page_directory_pointer:
    .skip 4096
page_directories:
    .skip 16384
__bootstrap_page_tables_end:

.balign 16
.global __bootstrap_stack_bottom
.global __bootstrap_stack_top
__bootstrap_stack_bottom:
stack_bottom:
    .skip 65536
__bootstrap_stack_top:
stack_top:

.section .note.GNU-stack,"",@progbits
