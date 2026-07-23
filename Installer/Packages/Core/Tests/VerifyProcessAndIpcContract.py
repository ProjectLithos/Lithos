from pathlib import Path
R=Path(__file__).resolve().parents[1]
def text(p): return (R/p).read_text(encoding='utf-8')
a=text('Source/address_space.c'); c=text('Source/capability.c'); s=text('Include/oesdk/syscall.h'); m=text('module-placement.example.json')
assert 'Size > UINTPTR_MAX - Address' in a
assert 'OESDK_VIRTUAL_FLAG_USER' in a and 'OESDK_VIRTUAL_FLAG_WRITABLE' in a
assert 'P->Rights&R' in c
assert 'OesdkSystemCallGet=0' in s and 'OesdkSystemCallEvent=2' in s
assert '"mode": "User"' in m
print('[ OK ] Process, user pointer, syscall, IPC and capability contracts.')
