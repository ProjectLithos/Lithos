# OESDK 0.18.2

## Template Kernel SDK usage

- Reworked the Kernel project template to use every safely initialisable SDK subsystem.
- Reduced `kmain.c` to a startup coordinator.
- Added separate `UsageBoot`, `UsageCpu`, `UsageMemory`, `UsageInterrupts`, `UsageObjects`, and `UsageGraphics` source files.
- Added capability derivation, IPC channel/endpoint exchange, and event usage.
- Added scheduler and SMP status reporting without inventing firmware or policy configuration.
- Updated Visual Studio and PowerShell project generation to produce the same modular source layout.
- SDK identity is `0.18.2`; package identity is `0.18.2.0`.
