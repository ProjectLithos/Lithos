#!/usr/bin/env python3
"""Static verification for the generated OESDK Windows bootstrapper."""

from __future__ import annotations

import base64
import gzip
import hashlib
import pathlib
import struct


ROOT = pathlib.Path(__file__).resolve().parents[1]
VERSION = "0.0.9"
EXE = ROOT / f"OESDK-Setup-{VERSION}-x64.exe"
SCRIPT = ROOT / "Source" / "Installer.ps1"
CHECKSUM = ROOT / f"OESDK-Setup-{VERSION}-x64.sha256"


def verify() -> None:
    image = EXE.read_bytes()
    assert image[:2] == b"MZ", "missing DOS header"
    pe_offset = struct.unpack_from("<I", image, 0x3C)[0]
    assert image[pe_offset:pe_offset + 4] == b"PE\0\0", "missing PE signature"
    assert struct.unpack_from("<H", image, pe_offset + 4)[0] == 0x8664, "not Windows x64"
    optional = pe_offset + 24
    assert struct.unpack_from("<H", image, optional)[0] == 0x20B, "not PE32+"
    assert struct.unpack_from("<H", image, optional + 68)[0] == 2, "not a GUI application"
    assert b"KERNEL32.dll\0" in image
    assert b"SHELL32.dll\0" in image
    assert b"ExitProcess\0" in image
    assert b"ShellExecuteW\0" in image

    marker = "-EncodedCommand ".encode("utf-16le")
    start = image.index(marker) + len(marker)
    end = image.index(b"\0\0", start)
    if (end - start) % 2:
        end += 1
    encoded = image[start:end].decode("utf-16le")
    loader = base64.b64decode(encoded).decode("utf-16le")
    payload_marker = "FromBase64String('"
    payload_start = loader.index(payload_marker) + len(payload_marker)
    payload_end = loader.index("')", payload_start)
    embedded_script = gzip.decompress(base64.b64decode(loader[payload_start:payload_end])).decode("utf-8")
    assert embedded_script == SCRIPT.read_text(encoding="utf-8"), "embedded script differs"
    for product in ("Community", "Professional", "Enterprise"):
        assert f"Microsoft.VisualStudio.Product.{product}" in embedded_script
    ensure_call = "$visualStudioPath = Ensure-VisualStudioSupport $bootstrapVisualStudioManifest"
    assert ensure_call in embedded_script
    assert embedded_script.index("Receive-File $ManifestUri") < embedded_script.index(ensure_call)
    assert "Installing Visual Studio Community..." in embedded_script
    assert "raw.githubusercontent.com/ProjectLithos/Lithos/main/Installer/manifest.json" in embedded_script
    assert "releaseStatus" in embedded_script
    assert "Ensure-QemuSupport $manifest" in embedded_script
    assert "[void]$response.EnsureSuccessStatusCode()" in embedded_script
    assert "Removing every previous OESDK Visual Studio registration..." in embedded_script
    assert "Removing the previous OESDK installation..." in embedded_script
    assert "*OESDK*.zip" in embedded_script
    assert "Visual Studio 2022\\Templates\\ProjectTemplates\\OESDK" in embedded_script
    assert "projectTemplates" in embedded_script
    assert "ReparsePoint" in embedded_script
    assert "/installvstemplates" in embedded_script
    assert "Close every Visual Studio window" in embedded_script
    assert "Starter OS:" not in embedded_script

    expected = CHECKSUM.read_text(encoding="ascii").split()[0]
    actual = hashlib.sha256(image).hexdigest().upper()
    assert actual == expected, "SHA-256 file is stale"
    print(f"[ OK ] Windows x64 PE structure: {len(image)} bytes")
    print("[ OK ] Native API imports: ShellExecuteW, ExitProcess")
    print("[ OK ] Embedded installer source matches Installer.ps1")
    print("[ OK ] Clean replacement, direct VS template installation and catalogue refresh are embedded")
    print(f"[ OK ] SHA-256: {actual}")


if __name__ == "__main__":
    verify()
