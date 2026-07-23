#include <oesdk/user_mode.h>
#include <oesdk/address_space.h>
OesdkStatus OesdkUserModeFramePrepare(uintptr_t Entry,uintptr_t Stack,OesdkUserModeFrame*F){if(F==NULL||Entry>OESDK_USER_ADDRESS_MAX||Stack==0U||Stack>OESDK_USER_ADDRESS_MAX)return OESDK_STATUS_INVALID_ARGUMENT;*F=(OesdkUserModeFrame){Entry,Stack,UINT64_C(0x202),OESDK_GDT_USER_CODE_RPL3_SELECTOR,OESDK_GDT_USER_DATA_RPL3_SELECTOR};return OESDK_STATUS_SUCCESS;}
#if defined(__x86_64__)
__attribute__((noreturn)) void OesdkUserModeEnter(const OesdkUserModeFrame*F){__asm__ volatile("movw %w0, %%ax\nmovw %%ax, %%ds\nmovw %%ax, %%es\npushq %0\npushq %1\npushq %2\npushq %3\npushq %4\niretq"::"r"((uint64_t)F->DataSelector),"r"((uint64_t)F->StackPointer),"r"(F->Flags),"r"((uint64_t)F->CodeSelector),"r"((uint64_t)F->InstructionPointer):"rax","memory");__builtin_unreachable();}
#endif
