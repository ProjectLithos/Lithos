/*
 * OESDK x86-64 application-processor trampoline contract.
 * The kernel copies a linked/assembled trampoline below 1 MiB and patches the
 * page-table root, stack top, APIC/logical identifiers and 64-bit entry point.
 * Platform templates may replace this stub with their boot-loader-specific
 * real-mode to long-mode transition while retaining the public smp.h contract.
 */
.section .rodata
.global oesdk_ap_trampoline_contract
.type oesdk_ap_trampoline_contract, @object
oesdk_ap_trampoline_contract:
    .ascii "OESDK-AP-TRAMPOLINE-0.18.2\0"
.size oesdk_ap_trampoline_contract, .-oesdk_ap_trampoline_contract
