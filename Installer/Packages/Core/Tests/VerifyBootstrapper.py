#!/usr/bin/env python3
"""Verify either the optional native bootstrapper or script-only setup model."""
from __future__ import annotations
import base64, gzip, hashlib, json, pathlib, struct

ROOT = pathlib.Path(__file__).resolve().parents[1]

def verify_native(exe: pathlib.Path, script: pathlib.Path, checksum: pathlib.Path) -> None:
    image = exe.read_bytes()
    assert image[:2] == b"MZ", "missing DOS header"
    pe_offset = struct.unpack_from("<I", image, 0x3C)[0]
    assert image[pe_offset:pe_offset + 4] == b"PE\0\0", "missing PE signature"
    assert struct.unpack_from("<H", image, pe_offset + 4)[0] == 0x8664, "not Windows x64"
    marker = "-EncodedCommand ".encode("utf-16le")
    start = image.index(marker) + len(marker)
    end = image.index(b"\0\0", start)
    if (end - start) % 2: end += 1
    loader = base64.b64decode(image[start:end].decode("utf-16le")).decode("utf-16le")
    payload_marker = "FromBase64String('"
    payload_start = loader.index(payload_marker) + len(payload_marker)
    payload_end = loader.index("')", payload_start)
    embedded = gzip.decompress(base64.b64decode(loader[payload_start:payload_end])).decode("utf-8")
    assert embedded == script.read_text(encoding="utf-8"), "embedded script differs"
    expected = checksum.read_text(encoding="ascii").split()[0].upper()
    assert hashlib.sha256(image).hexdigest().upper() == expected, "SHA-256 file is stale"

def verify() -> None:
    version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
    manifest = json.loads((ROOT / "manifest.json").read_text(encoding="utf-8"))
    script = ROOT / "Source" / "Installer.ps1"
    assert script.is_file(), "missing installer source"

    if manifest.get("containsCustomExecutable"):
        exe = ROOT / f"OESDK-Setup-{version}-x64.exe"
        checksum = ROOT / f"OESDK-Setup-{version}-x64.sha256"
        assert exe.is_file() and checksum.is_file()
        verify_native(exe, script, checksum)
        print("[ OK ] Native Windows bootstrapper matches Installer.ps1")
        return

    assert manifest.get("packageType") == "complete-script-only"
    setup = (ROOT / "Setup.bat").read_text(encoding="utf-8")
    visual_installer = ROOT / "Tools" / "Install-VisualStudio.ps1"
    assert visual_installer.is_file(), "missing script-only setup payload"
    assert "Install-VisualStudio.ps1" in setup
    assert "powershell.exe" in setup and "-ExecutionPolicy" in setup
    assert "-Verb RunAs" in setup, "Setup.bat must request elevation when needed"
    assert not list(ROOT.glob("OESDK-Setup-*-x64.exe")), "manifest says no custom executable but one is packaged"
    assert not list(ROOT.glob("OESDK-Setup-*-x64.sha256")), "orphan bootstrapper checksum packaged"
    print("[ OK ] Script-only bootstrapper model matches manifest")
    print("[ OK ] Setup.bat invokes the packaged PowerShell installer with elevation")
    print("[ OK ] No undeclared native bootstrapper or checksum is present")

if __name__ == "__main__":
    verify()
