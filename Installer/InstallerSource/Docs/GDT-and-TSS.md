# Complete GDT and Task State Segment

OESDK installs a kernel-owned x86-64 Global Descriptor Table after the bootstrap environment has entered long mode. The running kernel therefore does not depend on a bootloader-owned table.

## Descriptor layout

| Selector | Descriptor |
|---:|---|
| `0x00` | Null |
| `0x08` | Kernel code |
| `0x10` | Kernel data |
| `0x18` | User data |
| `0x20` | User code |
| `0x28` | 64-bit available Task State Segment |

Ring-3 segment-register values are formed by adding RPL 3. The public macros `OESDK_GDT_USER_DATA_RPL3_SELECTOR` and `OESDK_GDT_USER_CODE_RPL3_SELECTOR` therefore evaluate to `0x1B` and `0x23` without obscuring the actual GDT slot selectors.

## Task State Segment

The 104-byte x86-64 TSS supplies:

- `RSP0` for controlled ring-3 to ring-0 transitions;
- seven Interrupt Stack Table pointers;
- an I/O-map base set to the end of the TSS, which denies direct user-mode I/O unless a later module deliberately installs an I/O bitmap.

`OesdkGdtSetKernelStack()` must be called with the active thread's aligned kernel-stack top before entering user mode.

## Interrupt Stack Table

OESDK allocates seven independent, 16 KiB, 16-byte-aligned IST stacks. The first three have fixed architectural roles:

| IST | Use |
|---:|---|
| 1 | Double fault |
| 2 | Non-maskable interrupt |
| 3 | Machine check |
| 4–7 | Reserved spare exception stacks |

`OesdkGdtSetInterruptStack()` permits a kernel architecture module to replace an IST pointer with another non-zero, 16-byte-aligned stack top.

## Loading sequence

`OesdkGdtInitialize()` performs the following with interrupts preserved and temporarily disabled:

1. clears the owned GDT, TSS, and diagnostic state;
2. creates kernel and user descriptors;
3. creates the 16-byte TSS descriptor;
4. fills all seven IST entries;
5. loads GDTR with `lgdt`;
6. reloads CS using a far return and reloads the data segments;
7. loads TR with `ltr`;
8. publishes read-only diagnostic information.

The operation is idempotent. `OesdkGdtIsInitialized()` and `OesdkGdtInformationGet()` can be used to verify availability before installing the IDT.
