#!/usr/bin/env python3
"""Verify OESDK release packages, hashes, manifest, VSIX, and templates."""

from __future__ import annotations

import hashlib
import io
import json
import pathlib
import zipfile
import xml.etree.ElementTree as element_tree


ROOT = pathlib.Path(__file__).resolve().parents[1]
ASSETS = ROOT / "ReleaseAssets"
VERSION = "0.0.6"


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def safe_members(archive: zipfile.ZipFile) -> set[str]:
    names = set(archive.namelist())
    for name in names:
        path = pathlib.PurePosixPath(name)
        assert not path.is_absolute(), f"absolute ZIP member: {name}"
        assert ".." not in path.parts, f"unsafe ZIP member: {name}"
    return names


def open_nested(archive: zipfile.ZipFile, member: str) -> zipfile.ZipFile:
    return zipfile.ZipFile(io.BytesIO(archive.read(member)))


def verify() -> None:
    manifest = json.loads((ROOT / "manifest.json").read_text(encoding="utf-8"))
    assert manifest["schemaVersion"] == 1
    assert manifest["sdkVersion"] == VERSION
    assert manifest["releaseStatus"] == "ready"
    assert len(manifest["packages"]) == 4

    installed_paths: set[str] = set()
    for package in manifest["packages"]:
        path = ASSETS / pathlib.PurePosixPath(package["url"]).name
        assert path.is_file(), f"missing package: {path.name}"
        assert digest(path) == package["sha256"], f"stale hash: {path.name}"
        with zipfile.ZipFile(path) as archive:
            members = safe_members(archive)
            duplicates = installed_paths.intersection(members)
            assert not duplicates, f"packages overwrite the same installed paths: {sorted(duplicates)}"
            installed_paths.update(members)
            assert archive.testzip() is None, f"corrupt package: {path.name}"

    core_path = ASSETS / f"OESDK-Core-{VERSION}.zip"
    with zipfile.ZipFile(core_path) as archive:
        names = safe_members(archive)
        assert "Include/oesdk/kernel.h" in names
        assert "Source/runtime.c" in names
        assert "Source/serial.c" in names

    architecture_path = ASSETS / f"OESDK-x86_64-{VERSION}.zip"
    with zipfile.ZipFile(architecture_path) as archive:
        names = safe_members(archive)
        assert "Architecture/x86_64/boot.S" in names
        assert "Architecture/x86_64/linker.ld" in names
        assert "Tools/Build-Kernel.ps1" in names
        boot = archive.read("Architecture/x86_64/boot.S")
        assert b"MULTIBOOT_MAGIC" in boot and b"MULTIBOOT_FLAGS" in boot and b"long_mode_entry" in boot

    qemu_path = ASSETS / "OESDK-QEMU-x86_64.zip"
    with zipfile.ZipFile(qemu_path) as archive:
        ensure_qemu = archive.read("Tools/Ensure-Qemu.ps1").decode("utf-8")
        assert "SoftwareFreedomConservancy.QEMU" in ensure_qemu
        assert "OESDK_QEMU" in ensure_qemu

    visual_path = ASSETS / f"OESDK-VisualStudio-{VERSION}.zip"
    with zipfile.ZipFile(visual_path) as package:
        names = safe_members(package)
        assert "VisualStudio/OESDK.VisualStudio.vsix" in names
        with open_nested(package, "VisualStudio/OESDK.VisualStudio.vsix") as vsix:
            vsix_names = safe_members(vsix)
            assert "extension.vsixmanifest" in vsix_names
            assert "ProjectTemplates/OESDKKernel.zip" in vsix_names
            assert "ProjectTemplates/OESDKDesktop.zip" in vsix_names
            element_tree.fromstring(vsix.read("extension.vsixmanifest"))
            element_tree.fromstring(vsix.read("[Content_Types].xml"))
            for template_name, project_name in (
                ("ProjectTemplates/OESDKKernel.zip", "OESDKKernel.vcxproj"),
                ("ProjectTemplates/OESDKDesktop.zip", "OESDKDesktop.vcxproj"),
            ):
                with open_nested(vsix, template_name) as template:
                    template_names = safe_members(template)
                    assert project_name in template_names
                    assert "kmain.c" in template_names
                    vstemplate_name = next(name for name in template_names if name.endswith(".vstemplate"))
                    element_tree.fromstring(template.read(vstemplate_name))
                    project = template.read(project_name).decode("utf-8")
                    assert "Build-Kernel.ps1" in project and "Run-Qemu.ps1" in project

    source_path = ROOT / f"OESDK-Installer-{VERSION}-Source.zip"
    with zipfile.ZipFile(source_path) as source:
        names = safe_members(source)
        assert "manifest.json" in names
        assert "Tools/BuildPackages.py" in names
        assert f"ReleaseAssets/OESDK-Core-{VERSION}.zip" in names
        assert "Packages/VisualStudio/Templates/Kernel/kmain.c" in names

    print("[ OK ] Four manifest packages exist and all SHA-256 values match")
    print("[ OK ] Core runtime, x86-64 boot, QEMU integration, and VSIX contents")
    print("[ OK ] Kernel and desktop project templates are structurally valid")
    print("[ OK ] All archives are readable and reject unsafe member paths")
    print("[ OK ] Package extraction paths are unique across the staging area")


if __name__ == "__main__":
    verify()
