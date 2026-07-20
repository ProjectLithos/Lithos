#!/usr/bin/env python3
"""Build deterministic OESDK package archives, VSIX, manifest, and source ZIP."""

from __future__ import annotations

import hashlib
import io
import json
import pathlib
import zipfile


ROOT = pathlib.Path(__file__).resolve().parents[1]
PACKAGES = ROOT / "Packages"
ASSETS = ROOT / "ReleaseAssets"
VERSION = "0.0.8"
RAW_BASE = "https://raw.githubusercontent.com/ProjectLithos/Lithos/main/Installer/ReleaseAssets"
ZIP_TIME = (2026, 1, 1, 0, 0, 0)


def zip_info(name: str) -> zipfile.ZipInfo:
    info = zipfile.ZipInfo(name.replace("\\", "/"), ZIP_TIME)
    info.compress_type = zipfile.ZIP_DEFLATED
    info.external_attr = 0o100644 << 16
    return info


def add_bytes(archive: zipfile.ZipFile, name: str, content: bytes) -> None:
    archive.writestr(zip_info(name), content, compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)


def directory_bytes(root: pathlib.Path, prefix: str = "") -> bytes:
    output = io.BytesIO()
    with zipfile.ZipFile(output, "w") as archive:
        for path in sorted(item for item in root.rglob("*") if item.is_file()):
            add_bytes(archive, prefix + path.relative_to(root).as_posix(), path.read_bytes())
    return output.getvalue()


def write_archive(name: str, content: bytes) -> pathlib.Path:
    ASSETS.mkdir(parents=True, exist_ok=True)
    path = ASSETS / name
    path.write_bytes(content)
    return path


def build_visual_studio_package() -> pathlib.Path:
    visual_studio = PACKAGES / "VisualStudio"
    kernel_template = directory_bytes(visual_studio / "Templates" / "Kernel")
    desktop_template = directory_bytes(visual_studio / "Templates" / "Desktop")

    vsix_buffer = io.BytesIO()
    with zipfile.ZipFile(vsix_buffer, "w") as archive:
        add_bytes(archive, "[Content_Types].xml", (visual_studio / "Content_Types.xml").read_bytes())
        add_bytes(archive, "extension.vsixmanifest", (visual_studio / "extension.vsixmanifest").read_bytes())
        add_bytes(archive, "ProjectTemplates/OESDKKernel.zip", kernel_template)
        add_bytes(archive, "ProjectTemplates/OESDKDesktop.zip", desktop_template)

    package_buffer = io.BytesIO()
    with zipfile.ZipFile(package_buffer, "w") as archive:
        add_bytes(archive, "VisualStudio/OESDK.VisualStudio.vsix", vsix_buffer.getvalue())
        add_bytes(archive, "VisualStudio/ProjectTemplates/OESDKKernel.zip", kernel_template)
        add_bytes(archive, "VisualStudio/ProjectTemplates/OESDKDesktop.zip", desktop_template)
        add_bytes(archive, "VisualStudio/README.md", (visual_studio / "README.md").read_bytes())
    return write_archive(f"OESDK-VisualStudio-{VERSION}.zip", package_buffer.getvalue())


def sha256(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def package_entry(name: str, path: pathlib.Path) -> dict[str, str]:
    return {
        "name": name,
        "url": f"{RAW_BASE}/{path.name}",
        "sha256": sha256(path),
        "destination": ".",
    }


def build_source_archive() -> pathlib.Path:
    excluded_names = {
        f"OESDK-Installer-{VERSION}-Source.zip",
        f"OESDK-Setup-{VERSION}-x64.exe",
        f"OESDK-Setup-{VERSION}-x64.sha256",
    }
    output = ROOT / f"OESDK-Installer-{VERSION}-Source.zip"
    with zipfile.ZipFile(output, "w") as archive:
        for path in sorted(item for item in ROOT.rglob("*") if item.is_file()):
            relative = path.relative_to(ROOT)
            if path.name in excluded_names or "__pycache__" in relative.parts or "Build" in relative.parts:
                continue
            add_bytes(archive, relative.as_posix(), path.read_bytes())
    return output


def build() -> None:
    core = write_archive(f"OESDK-Core-{VERSION}.zip", directory_bytes(PACKAGES / "Core"))
    architecture = write_archive(f"OESDK-x86_64-{VERSION}.zip", directory_bytes(PACKAGES / "x86_64"))
    qemu = write_archive("OESDK-QEMU-x86_64.zip", directory_bytes(PACKAGES / "QEMU"))
    visual_studio = build_visual_studio_package()

    manifest = {
        "schemaVersion": 1,
        "sdkVersion": VERSION,
        "releaseStatus": "ready",
        "packages": [
            package_entry("OESDK Core", core),
            package_entry("OESDK x86-64 Support", architecture),
            package_entry("OESDK QEMU x86-64", qemu),
            package_entry("OESDK Visual Studio Integration", visual_studio),
        ],
        "visualStudio": {
            "communityBootstrapperUrl": "https://aka.ms/vs/17/release/vs_community.exe",
            "components": [
                "Microsoft.VisualStudio.Workload.NativeDesktop",
                "Microsoft.VisualStudio.Component.VC.Llvm.Clang",
                "Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Llvm.Clang",
            ],
            "projectTemplates": [
                "VisualStudio/ProjectTemplates/OESDKKernel.zip",
                "VisualStudio/ProjectTemplates/OESDKDesktop.zip",
            ],
        },
        "qemu": {"ensureScript": "Tools/Ensure-Qemu.ps1"},
    }
    manifest_text = json.dumps(manifest, indent=2) + "\n"
    (ROOT / "manifest.json").write_text(manifest_text, encoding="utf-8")
    (ROOT / "manifest.example.json").write_text(manifest_text, encoding="utf-8")
    source = build_source_archive()

    for path in (core, architecture, qemu, visual_studio, source):
        print(f"[ OK ] {path.name}: {path.stat().st_size} bytes, SHA-256 {sha256(path)}")


if __name__ == "__main__":
    build()
