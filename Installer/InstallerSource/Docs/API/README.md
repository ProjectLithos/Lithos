# OESDK API Reference

This index documents the public OESDK API supplied by the installed headers.

Every public function, function-like macro, and public SDK type has its own Markdown page. The API documentation verification test must pass before a release package is accepted.

## Boot Context

- [`OesdkBootProtocol`](./OesdkBootProtocol.md) — type
- [`OesdkBootContext`](./OesdkBootContext.md) — type
- [`OesdkBootContextInitialize`](./OesdkBootContextInitialize.md) — function
- [`OesdkBootContextGet`](./OesdkBootContextGet.md) — function

## Console

- [`kclear`](./kclear.md) — function
- [`kputc`](./kputc.md) — function
- [`kputs`](./kputs.md) — function
- [`kprintf`](./kprintf.md) — function

## Debug

- [`oesdk_debug_printf`](./oesdk_debug_printf.md) — function
- [`kdebugf`](./kdebugf.md) — macro

## CPU

- [`OesdkCpuRegisters`](./OesdkCpuRegisters.md) — type
- [`OesdkCpuFeatures`](./OesdkCpuFeatures.md) — type
- [`OesdkCpuInitialize`](./OesdkCpuInitialize.md) — function
- [`OesdkCpuFeaturesGet`](./OesdkCpuFeaturesGet.md) — function
- [`OesdkCpuQuery`](./OesdkCpuQuery.md) — function
- [`OesdkCpuDisableInterrupts`](./OesdkCpuDisableInterrupts.md) — function
- [`OesdkCpuEnableInterrupts`](./OesdkCpuEnableInterrupts.md) — function
- [`OesdkCpuInterruptStateSave`](./OesdkCpuInterruptStateSave.md) — function
- [`OesdkCpuInterruptStateRestore`](./OesdkCpuInterruptStateRestore.md) — function
- [`OesdkCpuHalt`](./OesdkCpuHalt.md) — function
- [`OesdkCpuPause`](./OesdkCpuPause.md) — function
- [`OesdkCpuReadFlags`](./OesdkCpuReadFlags.md) — function
- [`OesdkCpuReadCr0`](./OesdkCpuReadCr0.md) — function
- [`OesdkCpuReadCr2`](./OesdkCpuReadCr2.md) — function
- [`OesdkCpuReadCr3`](./OesdkCpuReadCr3.md) — function
- [`OesdkCpuReadCr4`](./OesdkCpuReadCr4.md) — function
- [`OesdkCpuWriteCr3`](./OesdkCpuWriteCr3.md) — function
- [`OesdkCpuInvalidatePage`](./OesdkCpuInvalidatePage.md) — function

## GDT and TSS

- [`OesdkGdtInformation`](./OesdkGdtInformation.md) — type
- [`OesdkGdtInitialize`](./OesdkGdtInitialize.md) — function
- [`OesdkGdtInformationGet`](./OesdkGdtInformationGet.md) — function
- [`OesdkGdtSetKernelStack`](./OesdkGdtSetKernelStack.md) — function

## Graphics

- [`kgfx_surface`](./kgfx_surface.md) — type
- [`kgfx_available`](./kgfx_available.md) — function
- [`kgfx_screen`](./kgfx_screen.md) — function
- [`kgfx_pixel`](./kgfx_pixel.md) — function
- [`kgfx_fill`](./kgfx_fill.md) — function
- [`kgfx_rectangle`](./kgfx_rectangle.md) — function

## Interrupts

- [`OesdkInterruptFrame`](./OesdkInterruptFrame.md) — type
- [`OesdkPageFaultInformation`](./OesdkPageFaultInformation.md) — type
- [`OesdkIdtInformation`](./OesdkIdtInformation.md) — type
- [`OesdkIdtInitialize`](./OesdkIdtInitialize.md) — function
- [`OesdkIdtInformationGet`](./OesdkIdtInformationGet.md) — function
- [`OesdkExceptionName`](./OesdkExceptionName.md) — function
- [`OesdkInterruptHandlerRegister`](./OesdkInterruptHandlerRegister.md) — function
- [`OesdkInterruptHandlerUnregister`](./OesdkInterruptHandlerUnregister.md) — function
- [`OesdkInterruptHandlerIsRegistered`](./OesdkInterruptHandlerIsRegistered.md) — function
- [`OesdkPageFaultDecode`](./OesdkPageFaultDecode.md) — function
- [`OesdkInterruptFrameDump`](./OesdkInterruptFrameDump.md) — function

## LibC Host

- [`OesdkLibcHost`](./OesdkLibcHost.md) — type
- [`OesdkLibcInstallHost`](./OesdkLibcInstallHost.md) — function
- [`OesdkLibcGetHost`](./OesdkLibcGetHost.md) — function
- [`OesdkLibcInitialize`](./OesdkLibcInitialize.md) — function

## Panic and Assertions

- [`OesdkPanic`](./OesdkPanic.md) — function
- [`OesdkPanicAt`](./OesdkPanicAt.md) — function
- [`OESDK_PANIC`](./OESDK_PANIC.md) — macro
- [`OESDK_ASSERT`](./OESDK_ASSERT.md) — macro
- [`OESDK_ASSERT_CODE`](./OESDK_ASSERT_CODE.md) — macro

## Status

- [`OesdkStatus`](./OesdkStatus.md) — type
- [`OESDK_STATUS_SUCCESS`](./OESDK_STATUS_SUCCESS.md) — constant macro
- [`OESDK_STATUS_INVALID_ARGUMENT`](./OESDK_STATUS_INVALID_ARGUMENT.md) — constant macro
- [`OESDK_STATUS_NOT_SUPPORTED`](./OESDK_STATUS_NOT_SUPPORTED.md) — constant macro
- [`OESDK_STATUS_OUT_OF_MEMORY`](./OESDK_STATUS_OUT_OF_MEMORY.md) — constant macro
- [`OESDK_STATUS_NOT_FOUND`](./OESDK_STATUS_NOT_FOUND.md) — constant macro
- [`OESDK_STATUS_ALREADY_EXISTS`](./OESDK_STATUS_ALREADY_EXISTS.md) — constant macro
- [`OESDK_STATUS_BUSY`](./OESDK_STATUS_BUSY.md) — constant macro
- [`OESDK_STATUS_TIMEOUT`](./OESDK_STATUS_TIMEOUT.md) — constant macro
- [`OESDK_STATUS_CORRUPT`](./OESDK_STATUS_CORRUPT.md) — constant macro
- [`OESDK_SUCCEEDED`](./OESDK_SUCCEEDED.md) — macro
- [`OESDK_FAILED`](./OESDK_FAILED.md) — macro
- [`OesdkStatusName`](./OesdkStatusName.md) — function
- [`OesdkStatusDescription`](./OesdkStatusDescription.md) — function

## Documentation maintenance rule

When a public API function, function-like macro, or type is added to `Include/oesdk`, the same change must:

1. add `Docs/API/<ApiName>.md`;
2. add the page to this index;
3. mirror it into the installer documentation tree;
4. pass `Tests/VerifyApiDocumentation.py`.
