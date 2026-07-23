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

- [`OesdkGdtDescriptorKind`](./OesdkGdtDescriptorKind.md) — type
- [`OESDK_GDT_USER_DATA_RPL3_SELECTOR`](./OESDK_GDT_USER_DATA_RPL3_SELECTOR.md) — macro
- [`OESDK_GDT_USER_CODE_RPL3_SELECTOR`](./OESDK_GDT_USER_CODE_RPL3_SELECTOR.md) — macro
- [`OesdkGdtInformation`](./OesdkGdtInformation.md) — type
- [`OesdkGdtInitialize`](./OesdkGdtInitialize.md) — function
- [`OesdkGdtIsInitialized`](./OesdkGdtIsInitialized.md) — function
- [`OesdkGdtInformationGet`](./OesdkGdtInformationGet.md) — function
- [`OesdkGdtSetKernelStack`](./OesdkGdtSetKernelStack.md) — function
- [`OesdkGdtSetInterruptStack`](./OesdkGdtSetInterruptStack.md) — function

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
- [`OesdkDivideErrorHandler`](./OesdkDivideErrorHandler.md) — function
- [`OesdkInvalidOpcodeHandler`](./OesdkInvalidOpcodeHandler.md) — function
- [`OesdkGeneralProtectionFaultHandler`](./OesdkGeneralProtectionFaultHandler.md) — function
- [`OesdkPageFaultHandler`](./OesdkPageFaultHandler.md) — function
- [`OesdkDoubleFaultHandler`](./OesdkDoubleFaultHandler.md) — function
- [`OesdkMachineCheckHandler`](./OesdkMachineCheckHandler.md) — function

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

## Physical memory

- [`OESDK_PAGE_MASK`](./OESDK_PAGE_MASK.md)
- [`OESDK_PHYSICAL_MAX_BYTES`](./OESDK_PHYSICAL_MAX_BYTES.md)
- [`OesdkPhysicalMemoryInformation`](./OesdkPhysicalMemoryInformation.md)
- [`OesdkAlignmentIsPowerOfTwo`](./OesdkAlignmentIsPowerOfTwo.md)
- [`OesdkAlignDownU64`](./OesdkAlignDownU64.md)
- [`OesdkAlignUpU64`](./OesdkAlignUpU64.md)
- [`OesdkPhysicalAddressToPage`](./OesdkPhysicalAddressToPage.md)
- [`OesdkPhysicalPageToAddress`](./OesdkPhysicalPageToAddress.md)
- [`OesdkPhysicalMemoryInitialize`](./OesdkPhysicalMemoryInitialize.md)
- [`OesdkPhysicalMemoryReserve`](./OesdkPhysicalMemoryReserve.md)
- [`OesdkPhysicalMemoryAllocate`](./OesdkPhysicalMemoryAllocate.md)
- [`OesdkPhysicalMemoryFree`](./OesdkPhysicalMemoryFree.md)
- [`OesdkPhysicalMemoryPageIsAllocated`](./OesdkPhysicalMemoryPageIsAllocated.md)
- [`OesdkPhysicalMemoryPageIsPermanent`](./OesdkPhysicalMemoryPageIsPermanent.md)
- [`OesdkPhysicalMemoryIsInitialized`](./OesdkPhysicalMemoryIsInitialized.md)
- [`OesdkPhysicalMemoryInformationGet`](./OesdkPhysicalMemoryInformationGet.md)
- [`OESDK_IS_POWER_OF_TWO`](./OESDK_IS_POWER_OF_TWO.md)
- [`OESDK_ALIGN_DOWN`](./OESDK_ALIGN_DOWN.md)
- [`OESDK_ALIGN_UP`](./OESDK_ALIGN_UP.md)

## Documentation maintenance rule

When a public API function, function-like macro, or type is added to `Include/oesdk`, the same change must:

1. add `Docs/API/<ApiName>.md`;
2. add the page to this index;
3. mirror it into the installer documentation tree;
4. pass `Tests/VerifyApiDocumentation.py`.

- [OesdkPageFaultDecodeCurrent](OesdkPageFaultDecodeCurrent.md)

- [OESDK_PAGE_FAULT_PRESENT_BIT](OESDK_PAGE_FAULT_PRESENT_BIT.md)

- [OESDK_PAGE_FAULT_WRITE_BIT](OESDK_PAGE_FAULT_WRITE_BIT.md)

- [OESDK_PAGE_FAULT_USER_BIT](OESDK_PAGE_FAULT_USER_BIT.md)

- [OESDK_PAGE_FAULT_RESERVED_BIT](OESDK_PAGE_FAULT_RESERVED_BIT.md)

- [OESDK_PAGE_FAULT_INSTRUCTION_BIT](OESDK_PAGE_FAULT_INSTRUCTION_BIT.md)

- [OESDK_PAGE_FAULT_PROTECTION_KEY_BIT](OESDK_PAGE_FAULT_PROTECTION_KEY_BIT.md)

- [OESDK_PAGE_FAULT_SHADOW_STACK_BIT](OESDK_PAGE_FAULT_SHADOW_STACK_BIT.md)

- [OESDK_PAGE_FAULT_SGX_BIT](OESDK_PAGE_FAULT_SGX_BIT.md)

- [`OesdkMemoryType`](./OesdkMemoryType.md)

- [`OesdkMemoryRegion`](./OesdkMemoryRegion.md)

- [`OesdkMemoryRangeEnd`](./OesdkMemoryRangeEnd.md)

- [`OesdkMemoryRangeIsValid`](./OesdkMemoryRangeIsValid.md)

- [`OesdkMemoryRangesOverlap`](./OesdkMemoryRangesOverlap.md)

- [`OesdkMemoryTypeFromMultiboot1`](./OesdkMemoryTypeFromMultiboot1.md)

- [`OesdkMemoryMapNormalizeMultiboot1`](./OesdkMemoryMapNormalizeMultiboot1.md)

- [`OesdkMemoryMapRegions`](./OesdkMemoryMapRegions.md)

- [`OesdkMemoryMapRegionCount`](./OesdkMemoryMapRegionCount.md)

- [`OesdkVirtualFlags`](OesdkVirtualFlags.md)

- [`OesdkVirtualMemoryInformation`](OesdkVirtualMemoryInformation.md)

- [`OesdkVirtualAddressIsCanonical`](OesdkVirtualAddressIsCanonical.md)

- [`OesdkVirtualMemoryInitialize`](OesdkVirtualMemoryInitialize.md)

- [`OesdkVirtualMap`](OesdkVirtualMap.md)

- [`OesdkVirtualUnmap`](OesdkVirtualUnmap.md)

- [`OesdkVirtualTranslate`](OesdkVirtualTranslate.md)

- [`OesdkVirtualMemoryIsInitialized`](OesdkVirtualMemoryIsInitialized.md)

- [`OesdkVirtualMemoryInformationGet`](OesdkVirtualMemoryInformationGet.md)

- [`OESDK_VIRTUAL_PAGE_MASK`](OESDK_VIRTUAL_PAGE_MASK.md)

- [`OESDK_VIRTUAL_FLAG_PRESENT`](OESDK_VIRTUAL_FLAG_PRESENT.md)

- [`OESDK_VIRTUAL_FLAG_WRITABLE`](OESDK_VIRTUAL_FLAG_WRITABLE.md)

- [`OESDK_VIRTUAL_FLAG_USER`](OESDK_VIRTUAL_FLAG_USER.md)

- [`OESDK_VIRTUAL_FLAG_WRITE_THROUGH`](OESDK_VIRTUAL_FLAG_WRITE_THROUGH.md)

- [`OESDK_VIRTUAL_FLAG_CACHE_DISABLE`](OESDK_VIRTUAL_FLAG_CACHE_DISABLE.md)

- [`OESDK_VIRTUAL_FLAG_GLOBAL`](OESDK_VIRTUAL_FLAG_GLOBAL.md)

- [`OESDK_VIRTUAL_FLAG_NO_EXECUTE`](OESDK_VIRTUAL_FLAG_NO_EXECUTE.md)
- [`OESDK_VIRTUAL_FLAGS_ALL`](./OESDK_VIRTUAL_FLAGS_ALL.md) — macro

- [`OesdkVirtualAddressDecode`](OesdkVirtualAddressDecode.md)
- [`OesdkVirtualAddressParts`](OesdkVirtualAddressParts.md)

- [OesdkHeapInformation](OesdkHeapInformation.md)

- [OesdkHeapBootstrapInitialize](OesdkHeapBootstrapInitialize.md)

- [OesdkHeapBootstrapAllocate](OesdkHeapBootstrapAllocate.md)

- [OesdkHeapBootstrapAllocateZeroed](OesdkHeapBootstrapAllocateZeroed.md)

- [OesdkHeapBootstrapIsInitialized](OesdkHeapBootstrapIsInitialized.md)

- [OesdkHeapInformationGet](OesdkHeapInformationGet.md)

- [OesdkHeapInitialize](OesdkHeapInitialize.md)
- [OesdkAllocate](OesdkAllocate.md)
- [OesdkAllocateAligned](OesdkAllocateAligned.md)
- [OesdkAllocateZeroed](OesdkAllocateZeroed.md)
- [OesdkFree](OesdkFree.md)
- [OesdkHeapBlock](OesdkHeapBlock.md)
- [OesdkHeapIsInitialized](OesdkHeapIsInitialized.md)
- [OESDK_HEAP_DEFAULT_ALIGNMENT](OESDK_HEAP_DEFAULT_ALIGNMENT.md)
## Timer

- [`OesdkTimerSource`](OesdkTimerSource.md)
- [`OesdkTimerInformation`](OesdkTimerInformation.md)
- [`OesdkTimerInitializePit`](OesdkTimerInitializePit.md)
- [`OesdkTimerInitializeLocalApic`](OesdkTimerInitializeLocalApic.md)
- [`OesdkTimerInitializeX2Apic`](OesdkTimerInitializeX2Apic.md)
- [`OesdkTimerInitializeHpet`](OesdkTimerInitializeHpet.md)
- [`OesdkTimerStop`](OesdkTimerStop.md)
- [`OesdkTimerInformationGet`](OesdkTimerInformationGet.md)
- [`OesdkTimerTicks`](OesdkTimerTicks.md)
- [`OesdkTimerTicksToSeconds`](OesdkTimerTicksToSeconds.md)
- [`OesdkTimerTicksToMilliseconds`](OesdkTimerTicksToMilliseconds.md)
- [`OesdkTimerTicksToNanoseconds`](OesdkTimerTicksToNanoseconds.md)

- [`OesdkInterruptControllerType`](OesdkInterruptControllerType.md)

- [`OesdkInterruptControllerInformation`](OesdkInterruptControllerInformation.md)

- [`OesdkInterruptControllerInitializePic`](OesdkInterruptControllerInitializePic.md)

- [`OesdkInterruptControllerInitializeLocalApic`](OesdkInterruptControllerInitializeLocalApic.md)

- [`OesdkInterruptControllerInitializeX2Apic`](OesdkInterruptControllerInitializeX2Apic.md)

- [`OesdkInterruptControllerAttachIoApic`](OesdkInterruptControllerAttachIoApic.md)

- [`OesdkInterruptControllerRouteIrq`](OesdkInterruptControllerRouteIrq.md)

- [`OesdkInterruptControllerMaskIrq`](OesdkInterruptControllerMaskIrq.md)

- [`OesdkInterruptControllerUnmaskIrq`](OesdkInterruptControllerUnmaskIrq.md)

- [`OesdkInterruptControllerEndOfInterrupt`](OesdkInterruptControllerEndOfInterrupt.md)

- [`OesdkInterruptControllerOwnsVector`](OesdkInterruptControllerOwnsVector.md)

- [`OesdkInterruptControllerInformationGet`](OesdkInterruptControllerInformationGet.md)

- [`OESDK_IOAPIC_POLARITY_LOW`](OESDK_IOAPIC_POLARITY_LOW.md)

- [`OESDK_IOAPIC_TRIGGER_LEVEL`](OESDK_IOAPIC_TRIGGER_LEVEL.md)

- [`OESDK_IOAPIC_MASKED`](OESDK_IOAPIC_MASKED.md)
