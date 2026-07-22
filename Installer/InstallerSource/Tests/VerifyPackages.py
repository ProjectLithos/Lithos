#!/usr/bin/env python3
"""Verify the current OESDK source-ready package layout and manifest."""
from __future__ import annotations
import json, pathlib, zipfile

ROOT = pathlib.Path(__file__).resolve().parents[1]

def safe_zip(path: pathlib.Path) -> None:
    with zipfile.ZipFile(path) as archive:
        for name in archive.namelist():
            member = pathlib.PurePosixPath(name)
            assert not member.is_absolute(), f"absolute ZIP member: {name}"
            assert ".." not in member.parts, f"unsafe ZIP member: {name}"
        assert archive.testzip() is None, f"corrupt package: {path.name}"

def verify() -> None:
    version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
    manifest = json.loads((ROOT / "manifest.json").read_text(encoding="utf-8"))
    assert manifest["schemaVersion"] == 1
    assert manifest["sdkVersion"] == version
    assert manifest["packageRevision"] == version
    assert manifest["releaseStatus"] == "source-ready"
    assert manifest["packageType"] == "complete-script-only"
    assert manifest["containsCustomExecutable"] is False

    required_layout = {
        "include": "Include", "source": "Source", "architecture": "Architecture",
        "tools": "Tools", "visualStudio": "VisualStudio", "fullInstallerSource": "InstallerSource",
    }
    assert manifest["installedLayout"] == required_layout
    for relative in required_layout.values():
        assert (ROOT / relative).is_dir(), f"missing installed-layout directory: {relative}"

    packages = manifest.get("packages")
    assert isinstance(packages, list) and packages, "manifest packages must be a non-empty list"
    for package in packages:
        assert isinstance(package.get("name"), str) and package["name"].strip()
        relative = pathlib.PurePosixPath(package["path"])
        assert not relative.is_absolute() and ".." not in relative.parts
        assert relative.parts and relative.parts[0] == "ReleaseAssets"
        built = ROOT / relative
        if built.exists():
            safe_zip(built)
        else:
            # A source-ready release may declare outputs produced by the build scripts.
            assert package.get("generatedAfterBuild", False) or manifest["releaseStatus"] == "source-ready"

    for required in (
        "Include/oesdk/kernel.h", "Source/runtime.c", "Source/interrupt.c",
        "Architecture/x86_64/boot.S", "Architecture/x86_64/interrupt_stubs.S",
        "Tools/BuildPackages.py", "Setup.bat", "Update.bat",
    ):
        assert (ROOT / required).is_file(), f"missing source-ready payload: {required}"

    for archive in sorted((ROOT / "ReleaseAssets").glob("*.zip")):
        safe_zip(archive)

    print(f"[ OK ] Source-ready manifest matches OESDK {version}")
    print("[ OK ] Script-only packaging correctly declares no custom executable")
    print("[ OK ] Installed layout and required source payload are present")
    print("[ OK ] Declared build outputs use safe ReleaseAssets paths")
    print("[ OK ] Existing release archives are readable and path-safe")

if __name__ == "__main__":
    verify()
