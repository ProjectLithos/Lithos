# OESDK 0.18.0

- Added process records, per-process address-space mapping metadata, user stacks and termination.
- Added strict user-pointer and user-range validation.
- Added ELF64 x86-64 executable loading through portable allocation/copy services.
- Added controlled Ring 3 entry frames using the existing user GDT descriptors.
- Added the three-operation Get/Set/Event syscall dispatcher and negative-status return ABI.
- Added Channel, Endpoint, Capability and Event reusable object modules.
- Added capability rights reduction and transfer enforcement.
- Added manifest-driven Kernel/User/Excluded/Automatic module placement and architecture profiles.
- Introduced separate `sdkIdentity` and four-part `packageIdentity` metadata.
