# `OesdkInterruptFrame`

The normalised x86-64 interrupt and exception frame passed to OESDK handlers. It stores R15 through RAX, `Vector`, `ErrorCode`, `Rip`, `Cs`, `Rflags`, `Rsp`, and `Ss` in that order.

`Rsp` and `Ss` are valid when the CPU changed privilege level or entered through an IST gate. The built-in NMI, double-fault, and machine-check gates use IST stacks.
