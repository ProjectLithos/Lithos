# `OESDK_VIRTUAL_FLAGS_ALL`

Mask containing every public flag accepted by `OesdkVirtualMap`:

- `OESDK_VIRTUAL_FLAG_PRESENT`
- `OESDK_VIRTUAL_FLAG_WRITABLE`
- `OESDK_VIRTUAL_FLAG_USER`
- `OESDK_VIRTUAL_FLAG_WRITE_THROUGH`
- `OESDK_VIRTUAL_FLAG_CACHE_DISABLE`
- `OESDK_VIRTUAL_FLAG_GLOBAL`
- `OESDK_VIRTUAL_FLAG_NO_EXECUTE`

`OesdkVirtualMap` rejects any bit outside this mask.

Declared in `<oesdk/virtual_memory.h>`.
