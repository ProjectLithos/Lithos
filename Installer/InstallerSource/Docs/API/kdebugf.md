# `kdebugf`

**Category:** Debug  
**Kind:** macro  
**Header:** `<oesdk/debug.h>`

Writes debug-only formatted output.

## Declaration

```c
#define kdebugf(...)
```

## Parameters

### `...`

A format string followed by its arguments.

## Returns

The Debug implementation returns the formatter result; in Release it expands to a no-op expression.

## Notes

- Arguments should not contain required side effects because Release builds do not evaluate them.

## Example

```c
kdebugf("Boot magic: 0x%x\n", magic);
```

[Back to the API index](./README.md)
