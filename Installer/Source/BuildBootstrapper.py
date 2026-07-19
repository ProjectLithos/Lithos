#!/usr/bin/env python3
"""Build the small native Windows launcher without bundling the SDK payload."""

from __future__ import annotations

import base64
import gzip
import hashlib
import pathlib
import struct


ROOT = pathlib.Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "Source" / "Installer.ps1"
OUTPUT = ROOT / "OESDK-Setup-0.0.1-x64.exe"
IMAGE_BASE = 0x140000000
FILE_ALIGNMENT = 0x200
SECTION_ALIGNMENT = 0x1000


def align(value: int, alignment: int) -> int:
    return (value + alignment - 1) & ~(alignment - 1)


def wide(value: str) -> bytes:
    return value.encode("utf-16le") + b"\0\0"


def pack_optional_header(entry_rva: int, image_size: int, header_size: int,
                         code_size: int, data_size: int, import_rva: int,
                         import_size: int, iat_rva: int, iat_size: int) -> bytes:
    values = bytearray(0xF0)
    struct.pack_into("<HBBIII", values, 0, 0x20B, 1, 0, code_size, data_size, 0)
    struct.pack_into("<IIQ", values, 16, entry_rva, 0x1000, IMAGE_BASE)
    struct.pack_into("<II", values, 32, SECTION_ALIGNMENT, FILE_ALIGNMENT)
    struct.pack_into("<HHHHHH", values, 40, 6, 1, 0, 0, 6, 1)
    struct.pack_into("<I", values, 52, 0)
    struct.pack_into("<III", values, 56, image_size, header_size, 0)
    struct.pack_into("<HH", values, 68, 2, 0x8160)
    struct.pack_into("<QQQQ", values, 72, 0x100000, 0x1000, 0x100000, 0x1000)
    struct.pack_into("<II", values, 104, 0, 16)
    struct.pack_into("<II", values, 120, import_rva, import_size)
    struct.pack_into("<II", values, 208, iat_rva, iat_size)
    return bytes(values)


def section_header(name: bytes, virtual_size: int, virtual_address: int,
                   raw_size: int, raw_offset: int, characteristics: int) -> bytes:
    return struct.pack("<8sIIIIIIHHI", name.ljust(8, b"\0"), virtual_size,
                       virtual_address, raw_size, raw_offset, 0, 0, 0, 0,
                       characteristics)


def build_idata(base_rva: int) -> tuple[bytes, int, int]:
    data = bytearray(60)  # two descriptors and one terminating descriptor

    def append(payload: bytes, alignment: int = 1) -> int:
        while len(data) % alignment:
            data.append(0)
        offset = len(data)
        data.extend(payload)
        return base_rva + offset

    kernel_name = append(b"KERNEL32.dll\0")
    shell_name = append(b"SHELL32.dll\0")
    exit_name = append(struct.pack("<H", 0) + b"ExitProcess\0", 2)
    shell_execute_name = append(struct.pack("<H", 0) + b"ShellExecuteW\0", 2)
    kernel_ilt = append(struct.pack("<QQ", exit_name, 0), 8)
    shell_ilt = append(struct.pack("<QQ", shell_execute_name, 0), 8)
    kernel_iat = append(struct.pack("<QQ", exit_name, 0), 8)
    shell_iat = append(struct.pack("<QQ", shell_execute_name, 0), 8)

    struct.pack_into("<IIIII", data, 0, kernel_ilt, 0, 0, kernel_name, kernel_iat)
    struct.pack_into("<IIIII", data, 20, shell_ilt, 0, 0, shell_name, shell_iat)
    return bytes(data), kernel_iat, shell_iat


def rip_disp(instruction_rva: int, instruction_length: int, target_rva: int) -> bytes:
    return struct.pack("<i", target_rva - (instruction_rva + instruction_length))


def build() -> None:
    script = SCRIPT.read_text(encoding="utf-8")
    compressed = gzip.compress(script.encode("utf-8"), compresslevel=9, mtime=0)
    payload = base64.b64encode(compressed).decode("ascii")
    loader = (
        "$b=[Convert]::FromBase64String('" + payload + "');"
        "$m=New-Object IO.MemoryStream(,$b);"
        "$g=New-Object IO.Compression.GzipStream($m,[IO.Compression.CompressionMode]::Decompress);"
        "$r=New-Object IO.StreamReader($g);iex $r.ReadToEnd()"
    )
    encoded = base64.b64encode(loader.encode("utf-16le")).decode("ascii")
    parameters = "-NoLogo -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -EncodedCommand " + encoded
    if len(parameters) >= 30000:
        raise SystemExit("Encoded PowerShell command is too large for a Windows process command line")

    text_rva = 0x1000
    rdata_rva = 0x2000
    rdata = bytearray()
    runas_rva = rdata_rva + len(rdata); rdata.extend(wide("runas"))
    powershell_rva = rdata_rva + len(rdata); rdata.extend(wide("powershell.exe"))
    parameters_rva = rdata_rva + len(rdata); rdata.extend(wide(parameters))
    rdata_raw_size = align(len(rdata), FILE_ALIGNMENT)
    idata_rva = align(rdata_rva + len(rdata), SECTION_ALIGNMENT)
    idata, exit_iat_rva, shell_iat_rva = build_idata(idata_rva)

    code = bytearray(b"\x48\x83\xec\x38\x31\xc9")
    offset = len(code); code.extend(b"\x48\x8d\x15\0\0\0\0")
    code[offset + 3:offset + 7] = rip_disp(text_rva + offset, 7, runas_rva)
    offset = len(code); code.extend(b"\x4c\x8d\x05\0\0\0\0")
    code[offset + 3:offset + 7] = rip_disp(text_rva + offset, 7, powershell_rva)
    offset = len(code); code.extend(b"\x4c\x8d\x0d\0\0\0\0")
    code[offset + 3:offset + 7] = rip_disp(text_rva + offset, 7, parameters_rva)
    code.extend(b"\x48\xc7\x44\x24\x20\0\0\0\0")
    code.extend(b"\x48\xc7\x44\x24\x28\x01\0\0\0")
    offset = len(code); code.extend(b"\xff\x15\0\0\0\0")
    code[offset + 2:offset + 6] = rip_disp(text_rva + offset, 6, shell_iat_rva)
    code.extend(b"\x31\xc9")
    offset = len(code); code.extend(b"\xff\x15\0\0\0\0")
    code[offset + 2:offset + 6] = rip_disp(text_rva + offset, 6, exit_iat_rva)
    code.extend(b"\xcc")

    headers_size = FILE_ALIGNMENT
    text_raw_offset = headers_size
    text_raw_size = align(len(code), FILE_ALIGNMENT)
    rdata_raw_offset = text_raw_offset + text_raw_size
    idata_raw_offset = rdata_raw_offset + rdata_raw_size
    idata_raw_size = align(len(idata), FILE_ALIGNMENT)
    image_size = align(idata_rva + len(idata), SECTION_ALIGNMENT)

    dos = bytearray(0x80)
    dos[0:2] = b"MZ"
    struct.pack_into("<I", dos, 0x3C, 0x80)
    dos_message = b"This program cannot be run in DOS mode.\r\r\n$"
    dos[0x40:0x40 + len(dos_message)] = dos_message
    coff = struct.pack("<HHIIIHH", 0x8664, 3, 0, 0, 0, 0xF0, 0x0022)
    optional = pack_optional_header(text_rva, image_size, headers_size,
                                    text_raw_size, rdata_raw_size + idata_raw_size,
                                    idata_rva, len(idata), exit_iat_rva, 32)
    sections = b"".join([
        section_header(b".text", len(code), text_rva, text_raw_size,
                       text_raw_offset, 0x60000020),
        section_header(b".rdata", len(rdata), rdata_rva, rdata_raw_size,
                       rdata_raw_offset, 0x40000040),
        section_header(b".idata", len(idata), idata_rva, idata_raw_size,
                       idata_raw_offset, 0xC0000040),
    ])
    headers = bytes(dos) + b"PE\0\0" + coff + optional + sections
    image = bytearray(headers.ljust(headers_size, b"\0"))
    image.extend(bytes(code).ljust(text_raw_size, b"\0"))
    image.extend(bytes(rdata).ljust(rdata_raw_size, b"\0"))
    image.extend(bytes(idata).ljust(idata_raw_size, b"\0"))
    OUTPUT.write_bytes(image)
    digest = hashlib.sha256(image).hexdigest().upper()
    (ROOT / "OESDK-Setup-0.0.1-x64.sha256").write_text(
        f"{digest}  {OUTPUT.name}\n", encoding="ascii")
    print(f"Built {OUTPUT.name} ({len(image)} bytes)")


if __name__ == "__main__":
    build()
