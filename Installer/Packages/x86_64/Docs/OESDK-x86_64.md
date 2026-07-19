# OESDK x86-64 support

This package provides a QEMU-compatible Multiboot entry point, the transition
to x86-64 long mode, a 4 GiB identity map, the kernel linker script, and Windows
PowerShell build/run commands. Address fields let QEMU load the 64-bit ELF as a
flat kernel image without GRUB. The build uses Visual Studio's Clang,
`llvm-ar`, and LLD and produces both `kernel.elf` and `liboesdk.a`.
