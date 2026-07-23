# OESDK 0.17.35

- Added the initial bounded thread table and round-robin scheduler policy.
- Added complete public x86_64 saved CPU context records.
- Added thread kernel stacks, unique IDs, priorities, owner pointers, wait reasons, wake ticks, and quantum accounting.
- Added Created, Ready, Running, Blocked, Sleeping, and Terminated states.
- Added overflow-safe millisecond-to-tick quantum calculation with a 15 ms default.
- Connected timer ticks to sleeper wake-up and reschedule requests.
- Added scheduler documentation and contract verification.
