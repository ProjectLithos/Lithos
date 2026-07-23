# Interrupt Controller and Timer

OESDK initializes the legacy 8259 PIC first, remaps IRQs to vectors 32-47, masks every line, installs the PIT handler on vector 32, unmasks IRQ0, and only then enables CPU interrupts. Hardware interrupt dispatch sends the correct end-of-interrupt after the registered handler returns.

The controller API also supports Local APIC MMIO, x2APIC MSRs, and attaching an IOAPIC for external IRQ routing. PIC is the safe bootstrap path; APIC/x2APIC and IOAPIC are explicit upgrades once firmware tables provide their addresses and interrupt-source overrides.

## PIC sequence

1. Save and remap both 8259 controllers.
2. Set the master base to 32 and the slave base to 40.
3. Mask all 16 IRQ lines.
4. Register the PIT handler on vector 32.
5. Program PIT channel 0.
6. Unmask IRQ0.
7. Enable CPU interrupts.

Slave IRQ end-of-interrupt is sent to the slave first and then the master. Master IRQs send it only to the master.

## APIC and x2APIC

`OesdkInterruptControllerInitializeLocalApic` enables the Local APIC through `IA32_APIC_BASE` and the spurious-interrupt vector register. `OesdkInterruptControllerInitializeX2Apic` enables x2APIC mode and uses MSRs for its ID, spurious vector and EOI.

## IOAPIC

After `OesdkInterruptControllerAttachIoApic`, `OesdkInterruptControllerRouteIrq` programs a redirection-table entry with an explicit vector, destination APIC ID, polarity, trigger mode and mask state.

## Timer

The existing PIT, Local APIC, x2APIC and HPET backends use the controller for IRQ delivery and EOI. PIT uses 1,193,182 Hz and rounded divisor calculation. Tick-to-time conversions use quotient and remainder to avoid multiplying the full tick count.
