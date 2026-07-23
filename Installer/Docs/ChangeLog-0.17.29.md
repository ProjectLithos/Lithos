# OESDK 0.17.30

- Added legacy PIC remapping, masking, unmasking and correct EOI handling.
- Added Local APIC and x2APIC controller initialization and EOI support.
- Added IOAPIC attachment and redirection-table routing.
- Integrated PIT startup with PIC IRQ0 routing.
- Added automatic EOI after hardware interrupt dispatch.
- Runtime now starts a 100 Hz PIT only after IDT and PIC setup, then enables interrupts.
- Updated the base template, API documentation, manifests and regression tests.
