.section .text
.extern OesdkInterruptDispatch

.macro EXC_NOERR n
.global OesdkExceptionStub\n
OesdkExceptionStub\n:
    pushq $0
    pushq $\n
    jmp OesdkExceptionCommon
.endm

.macro EXC_ERR n
.global OesdkExceptionStub\n
OesdkExceptionStub\n:
    pushq $\n
    jmp OesdkExceptionCommon
.endm

EXC_NOERR 0
EXC_NOERR 1
EXC_NOERR 2
EXC_NOERR 3
EXC_NOERR 4
EXC_NOERR 5
EXC_NOERR 6
EXC_NOERR 7
EXC_ERR   8
EXC_NOERR 9
EXC_ERR   10
EXC_ERR   11
EXC_ERR   12
EXC_ERR   13
EXC_ERR   14
EXC_NOERR 15
EXC_NOERR 16
EXC_ERR   17
EXC_NOERR 18
EXC_NOERR 19
EXC_NOERR 20
EXC_ERR   21
EXC_NOERR 22
EXC_NOERR 23
EXC_NOERR 24
EXC_NOERR 25
EXC_NOERR 26
EXC_NOERR 27
EXC_NOERR 28
EXC_ERR   29
EXC_ERR   30
EXC_NOERR 31

.global OesdkExceptionCommon
OesdkExceptionCommon:
    cld
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rbp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    movq %rsp, %rdi
    call OesdkInterruptDispatch
1:  cli
    hlt
    jmp 1b

.section .rodata
.align 8
.global OesdkExceptionStubTable
OesdkExceptionStubTable:
    .quad OesdkExceptionStub0
    .quad OesdkExceptionStub1
    .quad OesdkExceptionStub2
    .quad OesdkExceptionStub3
    .quad OesdkExceptionStub4
    .quad OesdkExceptionStub5
    .quad OesdkExceptionStub6
    .quad OesdkExceptionStub7
    .quad OesdkExceptionStub8
    .quad OesdkExceptionStub9
    .quad OesdkExceptionStub10
    .quad OesdkExceptionStub11
    .quad OesdkExceptionStub12
    .quad OesdkExceptionStub13
    .quad OesdkExceptionStub14
    .quad OesdkExceptionStub15
    .quad OesdkExceptionStub16
    .quad OesdkExceptionStub17
    .quad OesdkExceptionStub18
    .quad OesdkExceptionStub19
    .quad OesdkExceptionStub20
    .quad OesdkExceptionStub21
    .quad OesdkExceptionStub22
    .quad OesdkExceptionStub23
    .quad OesdkExceptionStub24
    .quad OesdkExceptionStub25
    .quad OesdkExceptionStub26
    .quad OesdkExceptionStub27
    .quad OesdkExceptionStub28
    .quad OesdkExceptionStub29
    .quad OesdkExceptionStub30
    .quad OesdkExceptionStub31
