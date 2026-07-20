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
VERSION = "0.0.12"


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
    assert manifest["visualStudio"]["projectTemplates"] == [
        "VisualStudio/ProjectTemplates/OESDKKernel.zip",
        "VisualStudio/ProjectTemplates/OESDKDesktop.zip",
    ]
    assert "vsix" not in manifest["visualStudio"]

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
        assert "Licenses/Project/MIT.txt" in names
        assert "Licenses/Project/GPL-3.0-or-later.txt" in names
        assert "Source/runtime.c" in names
        assert "Source/serial.c" in names
        stddef = archive.read("Include/stddef.h").decode("utf-8")
        stdint = archive.read("Include/stdint.h").decode("utf-8")
        stdbool = archive.read("Include/stdbool.h").decode("utf-8")
        kernel_header = archive.read("Include/oesdk/kernel.h").decode("utf-8")
        assert "#define OESDK_VERSION_PATCH 12" in kernel_header
        assert "__INTELLISENSE__" in stddef and "unsigned long long size_t" in stddef
        assert "__INTELLISENSE__" in stdint and "unsigned long long uintptr_t" in stdint
        assert "__INTELLISENSE__" in stdbool and "#define bool unsigned char" in stdbool
        graphics = archive.read("Source/graphics.c").decode("utf-8")
        assert graphics.index("#ifdef OESDK_DESKTOP") < graphics.index("static inline void out8")
        serial = archive.read("Source/serial.c").decode("utf-8")
        assert serial.index("#ifdef DEBUG") < serial.index("static void serial_emit")

    architecture_path = ASSETS / f"OESDK-x86_64-{VERSION}.zip"
    with zipfile.ZipFile(architecture_path) as archive:
        names = safe_members(archive)
        assert "Architecture/x86_64/boot.S" in names
        assert "Architecture/x86_64/linker.ld" in names
        assert "Tools/Build-Kernel.ps1" in names
        assert "Tools/New-OESDKProject.ps1" in names
        assert "Tools/New-OESDKProject.bat" in names
        assert "Tools/Repair-OESDKTemplates.ps1" in names
        assert "Tools/Test-OESDKProjectMetadata.ps1" in names
        assert "Tools/Set-OESDKFileVersion.ps1" in names
        boot = archive.read("Architecture/x86_64/boot.S")
        assert b"MULTIBOOT_MAGIC" in boot and b"MULTIBOOT_FLAGS" in boot and b"long_mode_entry" in boot
        generator = archive.read("Tools/New-OESDKProject.ps1").decode("utf-8")
        assert "Microsoft.Cpp.Default.props" in generator
        assert ".vcxproj" in generator
        assert "Microsoft.NET.Sdk" not in generator
        assert "($ProjectName + '.csproj')" not in generator
        assert "-ProjectRoot &quot;$(ProjectDir).&quot;" in generator
        assert "<IncludePath>$(OESDK_ROOT)\\Include;$(ProjectDir)Include;$(IncludePath)</IncludePath>" in generator
        assert "<NMakeIncludeSearchPath>$(OESDK_ROOT)\\Include;$(ProjectDir)Include</NMakeIncludeSearchPath>" in generator
        assert "__INTELLISENSE__=1;DEBUG=1__PROJECT_DEFINITION__" in generator
        assert "OESDK_DESKTOP=1" in generator
        assert "AuthorName" in generator and "AuthorEmail" in generator
        assert "OESDKFileVersions.json" in generator
        assert "File Version:" in generator
        assert "SPDX-License-Identifier:" in generator
        build_script = archive.read("Tools/Build-Kernel.ps1").decode("utf-8")
        assert "$rawProjectRoot.IndexOf('" + '"' + "')" in build_script
        assert "$rawProjectRoot.Substring(0, $embeddedQuote)" in build_script
        assert "-Configuration\\s+[^A-Za-z]*(Debug|Release)" in build_script
        assert "-ProjectType\\s+[^A-Za-z]*(Kernel|Desktop)" in build_script
        assert "(?:^|\\s)-Clean(?:\\s|$)" in build_script
        repair = archive.read("Tools/Repair-OESDKTemplates.ps1").decode("utf-8")
        assert "*OESDK*.zip" in repair
        assert "Visual Studio 2022\\Templates\\ProjectTemplates" in repair
        assert "OESDKKernel.zip" in repair and "OESDKDesktop.zip" in repair
        assert "VSIX installation failed" not in repair

    qemu_path = ASSETS / "OESDK-QEMU-x86_64.zip"
    with zipfile.ZipFile(qemu_path) as archive:
        ensure_qemu = archive.read("Tools/Ensure-Qemu.ps1").decode("utf-8")
        assert "SoftwareFreedomConservancy.QEMU" in ensure_qemu
        assert "OESDK_QEMU" in ensure_qemu

    visual_path = ASSETS / f"OESDK-VisualStudio-{VERSION}.zip"
    with zipfile.ZipFile(visual_path) as package:
        names = safe_members(package)
        assert "VisualStudio/OESDK.VisualStudio.vsix" in names
        assert "VisualStudio/ProjectTemplates/OESDKKernel.zip" in names
        assert "VisualStudio/ProjectTemplates/OESDKDesktop.zip" in names
        for direct_template, project_name in (
            ("VisualStudio/ProjectTemplates/OESDKKernel.zip", "OESDKKernel.vcxproj"),
            ("VisualStudio/ProjectTemplates/OESDKDesktop.zip", "OESDKDesktop.vcxproj"),
        ):
            with open_nested(package, direct_template) as template:
                direct_names = safe_members(template)
                assert project_name in direct_names
                assert "OESDKProject.json" in direct_names
                assert "OESDKFileVersions.json" in direct_names
                assert "OSVersion.h" in direct_names
                assert "AUTHORS" in direct_names and "LICENSE" in direct_names
                assert any(name.endswith(".vstemplate") for name in direct_names)
        with open_nested(package, "VisualStudio/OESDK.VisualStudio.vsix") as vsix:
            vsix_names = safe_members(vsix)
            assert "extension.vsixmanifest" in vsix_names
            assert "ProjectTemplates/OESDKKernel.zip" in vsix_names
            assert "ProjectTemplates/OESDKDesktop.zip" in vsix_names
            element_tree.fromstring(vsix.read("extension.vsixmanifest"))
            element_tree.fromstring(vsix.read("[Content_Types].xml"))
            for template_name, project_name, debug_definitions, release_definitions in (
                ("ProjectTemplates/OESDKKernel.zip", "OESDKKernel.vcxproj", "__INTELLISENSE__=1;DEBUG=1", "__INTELLISENSE__=1"),
                ("ProjectTemplates/OESDKDesktop.zip", "OESDKDesktop.vcxproj", "__INTELLISENSE__=1;DEBUG=1;OESDK_DESKTOP=1", "__INTELLISENSE__=1;OESDK_DESKTOP=1"),
            ):
                with open_nested(vsix, template_name) as template:
                    template_names = safe_members(template)
                    assert project_name in template_names
                    assert "kmain.c" in template_names
                    assert "OSVersion.h" in template_names
                    assert "OESDKProject.json" in template_names
                    assert "OESDKFileVersions.json" in template_names
                    source = template.read("kmain.c").decode("utf-8")
                    header = source.split("*/", 1)[0]
                    assert "File Version: 0.0.1" in header
                    assert "Author: __OESDK_AUTHOR_NAME__" in header
                    assert "Email: __OESDK_AUTHOR_EMAIL__" in header
                    assert "License: __OESDK_LICENSE_ID__" in header
                    assert "OS Version:" not in header
                    assert not any(name.endswith(".csproj") for name in template_names)
                    vstemplate_name = next(name for name in template_names if name.endswith(".vstemplate"))
                    template_xml = template.read(vstemplate_name)
                    element_tree.fromstring(template_xml)
                    assert b"OESDK 0.0.12 - Clang C" in template_xml
                    assert b"ProjectLithos.OESDK.v0012" in template_xml
                    assert b"Microsoft.NET.Sdk" not in template_xml
                    project = template.read(project_name).decode("utf-8")
                    assert "Build-Kernel.ps1" in project and "Run-Qemu.ps1" in project
                    assert "Microsoft.NET.Sdk" not in project and ".csproj" not in project
                    assert "-ProjectRoot &quot;$(ProjectDir).&quot;" in project
                    assert "-ProjectRoot &quot;$(ProjectDir)&quot;" not in project
                    assert "<IncludePath>$(OESDK_ROOT)\\Include;$(ProjectDir)Include;$(IncludePath)</IncludePath>" in project
                    assert "<NMakeIncludeSearchPath>$(OESDK_ROOT)\\Include;$(ProjectDir)Include</NMakeIncludeSearchPath>" in project
                    assert f">{debug_definitions}</NMakePreprocessorDefinitions>" in project
                    assert f">{release_definitions}</NMakePreprocessorDefinitions>" in project

    source_path = ROOT / f"OESDK-Installer-{VERSION}-Source.zip"
    with zipfile.ZipFile(source_path) as source:
        names = safe_members(source)
        assert "manifest.json" in names
        assert "Tools/BuildPackages.py" in names
        assert f"ReleaseAssets/OESDK-Core-{VERSION}.zip" in names
        assert "Packages/VisualStudio/Templates/Kernel/kmain.c" in names

    print("[ OK ] Four manifest packages exist and all SHA-256 values match")
    print("[ OK ] Core runtime, x86-64 boot, QEMU integration, and VSIX contents")
    print("[ OK ] Freestanding headers include Visual Studio IntelliSense type fallbacks")
    print("[ OK ] Desktop-only and debug-only helpers do not produce unused warnings")
    print("[ OK ] Exactly two native .vcxproj templates are packaged for direct Visual Studio discovery")
    print("[ OK ] Direct native project generator and template repair utility are packaged")
    print("[ OK ] Visual Studio project paths avoid trailing-backslash quote corruption")
    print("[ OK ] VS IncludePath, NMake include path, and IntelliSense definitions are preconfigured")
    print("[ OK ] Author, email, licence, OS metadata, and independent file versions are packaged")
    print("[ OK ] All archives are readable and reject unsafe member paths")
    print("[ OK ] Package extraction paths are unique across the staging area")


if __name__ == "__main__":
    verify()
