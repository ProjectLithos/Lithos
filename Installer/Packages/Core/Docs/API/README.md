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

## Threads and Scheduler

- [`OESDK_THREAD_DEFAULT_STACK_BYTES`](./OESDK_THREAD_DEFAULT_STACK_BYTES.md) — macro
- [`OesdkThreadId`](./OesdkThreadId.md) — type
- [`OesdkThreadState`](./OesdkThreadState.md) — type
- [`OesdkThreadWaitReason`](./OesdkThreadWaitReason.md) — type
- [`OesdkThreadContext`](./OesdkThreadContext.md) — type
- [`OesdkThread`](./OesdkThread.md) — type
- [`OesdkSchedulerInformation`](./OesdkSchedulerInformation.md) — type
- [`OesdkSchedulerQuantumTicks`](./OesdkSchedulerQuantumTicks.md) — function
- [`OesdkSchedulerInitialize`](./OesdkSchedulerInitialize.md) — function
- [`OesdkThreadCreate`](./OesdkThreadCreate.md) — function
- [`OesdkThreadMakeReady`](./OesdkThreadMakeReady.md) — function
- [`OesdkThreadBlock`](./OesdkThreadBlock.md) — function
- [`OesdkThreadSleep`](./OesdkThreadSleep.md) — function
- [`OesdkThreadTerminate`](./OesdkThreadTerminate.md) — function
- [`OesdkThreadInformationGet`](./OesdkThreadInformationGet.md) — function
- [`OesdkSchedulerCurrentThread`](./OesdkSchedulerCurrentThread.md) — function
- [`OesdkSchedulerSelectNext`](./OesdkSchedulerSelectNext.md) — function
- [`OesdkSchedulerOnTimerTick`](./OesdkSchedulerOnTimerTick.md) — function
- [`OesdkSchedulerReschedulePending`](./OesdkSchedulerReschedulePending.md) — function
- [`OesdkSchedulerRescheduleAcknowledged`](./OesdkSchedulerRescheduleAcknowledged.md) — function
- [`OesdkSchedulerInformationGet`](./OesdkSchedulerInformationGet.md) — function

## Processes and isolation

See `../Processes-User-Mode-System-Calls-IPC.md` and the public headers for AddressSpace, Process, ELF loader, UserMode, Syscall, IPC, Capability and Event.

## Processes, user mode, system calls and IPC

- [`OESDK_RIGHT_DUPLICATE`](OESDK_RIGHT_DUPLICATE.md)
- [`OESDK_RIGHT_EXECUTE`](OESDK_RIGHT_EXECUTE.md)
- [`OESDK_RIGHT_MANAGE`](OESDK_RIGHT_MANAGE.md)
- [`OESDK_RIGHT_READ`](OESDK_RIGHT_READ.md)
- [`OESDK_RIGHT_SIGNAL`](OESDK_RIGHT_SIGNAL.md)
- [`OESDK_RIGHT_TRANSFER`](OESDK_RIGHT_TRANSFER.md)
- [`OESDK_RIGHT_WRITE`](OESDK_RIGHT_WRITE.md)
- [`OESDK_USER_STACK_DEFAULT_BYTES`](OESDK_USER_STACK_DEFAULT_BYTES.md)
- [`OesdkAddressSpace`](OesdkAddressSpace.md)
- [`OesdkAddressSpaceCreate`](OesdkAddressSpaceCreate.md)
- [`OesdkAddressSpaceId`](OesdkAddressSpaceId.md)
- [`OesdkAddressSpaceMap`](OesdkAddressSpaceMap.md)
- [`OesdkAddressSpaceMapping`](OesdkAddressSpaceMapping.md)
- [`OesdkAddressSpaceQuery`](OesdkAddressSpaceQuery.md)
- [`OesdkAddressSpaceUnmap`](OesdkAddressSpaceUnmap.md)
- [`OesdkCapability`](OesdkCapability.md)
- [`OesdkCapabilityAllows`](OesdkCapabilityAllows.md)
- [`OesdkCapabilityCreate`](OesdkCapabilityCreate.md)
- [`OesdkCapabilityDerive`](OesdkCapabilityDerive.md)
- [`OesdkChannel`](OesdkChannel.md)
- [`OesdkChannelInitialize`](OesdkChannelInitialize.md)
- [`OesdkElfImageInformation`](OesdkElfImageInformation.md)
- [`OesdkElfLoadExecutable64`](OesdkElfLoadExecutable64.md)
- [`OesdkElfLoadServices`](OesdkElfLoadServices.md)
- [`OesdkEndpoint`](OesdkEndpoint.md)
- [`OesdkEndpointBind`](OesdkEndpointBind.md)
- [`OesdkEndpointReceive`](OesdkEndpointReceive.md)
- [`OesdkEndpointSend`](OesdkEndpointSend.md)
- [`OesdkEvent`](OesdkEvent.md)
- [`OesdkEventInitialize`](OesdkEventInitialize.md)
- [`OesdkIpcMessage`](OesdkIpcMessage.md)
- [`OesdkKernelArchitecture`](OesdkKernelArchitecture.md)
- [`OesdkModuleMode`](OesdkModuleMode.md)
- [`OesdkProcess`](OesdkProcess.md)
- [`OesdkProcessConfigureUserStack`](OesdkProcessConfigureUserStack.md)
- [`OesdkProcessCreate`](OesdkProcessCreate.md)
- [`OesdkProcessGet`](OesdkProcessGet.md)
- [`OesdkProcessId`](OesdkProcessId.md)
- [`OesdkProcessState`](OesdkProcessState.md)
- [`OesdkProcessTerminate`](OesdkProcessTerminate.md)
- [`OesdkRights`](OesdkRights.md)
- [`OesdkSystemCall`](OesdkSystemCall.md)
- [`OesdkSystemCallArguments`](OesdkSystemCallArguments.md)
- [`OesdkSystemCallDispatch`](OesdkSystemCallDispatch.md)
- [`OesdkSystemCallTable`](OesdkSystemCallTable.md)
- [`OesdkUserModeEnter`](OesdkUserModeEnter.md)
- [`OesdkUserModeFrame`](OesdkUserModeFrame.md)
- [`OesdkUserModeFramePrepare`](OesdkUserModeFramePrepare.md)
- [`OesdkUserRangeValidate`](OesdkUserRangeValidate.md)

## SMP startup

- [`OesdkCpuStartupState`](OesdkCpuStartupState.md)
- [`OesdkInterruptControllerCurrentApicId`](OesdkInterruptControllerCurrentApicId.md)
- [`OesdkInterruptControllerSendInitIpi`](OesdkInterruptControllerSendInitIpi.md)
- [`OesdkInterruptControllerSendStartupIpi`](OesdkInterruptControllerSendStartupIpi.md)
- [`OesdkSmpApplicationProcessorMain`](OesdkSmpApplicationProcessorMain.md)
- [`OesdkSmpApplicationProcessorOnline`](OesdkSmpApplicationProcessorOnline.md)
- [`OesdkSmpConfiguration`](OesdkSmpConfiguration.md)
- [`OesdkSmpCurrentApicId`](OesdkSmpCurrentApicId.md)
- [`OesdkSmpCurrentProcessor`](OesdkSmpCurrentProcessor.md)
- [`OesdkSmpInformation`](OesdkSmpInformation.md)
- [`OesdkSmpInformationGet`](OesdkSmpInformationGet.md)
- [`OesdkSmpInitialize`](OesdkSmpInitialize.md)
- [`OesdkSmpIsBootstrapProcessor`](OesdkSmpIsBootstrapProcessor.md)
- [`OesdkSmpProcessor`](OesdkSmpProcessor.md)
- [`OesdkSmpProcessorGet`](OesdkSmpProcessorGet.md)
- [`OesdkSmpSetProcessorResources`](OesdkSmpSetProcessorResources.md)
- [`OesdkSmpStartApplicationProcessors`](OesdkSmpStartApplicationProcessors.md)
