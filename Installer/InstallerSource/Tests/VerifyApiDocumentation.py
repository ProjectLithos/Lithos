#!/usr/bin/env python3
"""Verify that every public OESDK API item has a page and index entry."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
HEADERS = ROOT / "Include" / "oesdk"
API_DIR = ROOT / "Docs" / "API"
INDEX = API_DIR / "README.md"

EXCLUDED_MACROS = {
    "OESDK_BOOT_CONTEXT_H", "OESDK_CONSOLE_H", "OESDK_CPU_H", "OESDK_DEBUG_H",
    "OESDK_GDT_H", "OESDK_GRAPHICS_H", "OESDK_INTERRUPT_H", "OESDK_KERNEL_H",
    "OESDK_LIBC_H", "OESDK_PANIC_H", "OESDK_STATUS_H", "OESDK_NORETURN",
}

def public_items(text: str) -> set[str]:
    items: set[str] = set()
    # Function declarations whose public names follow OESDK or established k* / kgfx* naming.
    for declaration in re.findall(r"(?m)^\s*[^#\n][^\n;]*\([^;]*\)\s*;", text):
        match = re.search(r"\b(Oesdk[A-Za-z0-9_]+|oesdk_[A-Za-z0-9_]+|k(?:clear|putc|puts|printf)|kgfx_[A-Za-z0-9_]+)\s*\(", declaration)
        if match:
            items.add(match.group(1))
    # Function-like public macros.
    for name in re.findall(r"(?m)^\s*#define\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(", text):
        if name not in EXCLUDED_MACROS:
            items.add(name)
    # Public typedef names.
    for name in re.findall(r"(?s)typedef\s+(?:struct|enum)\s+[A-Za-z_][A-Za-z0-9_]*\s*\{.*?\}\s*([A-Za-z_][A-Za-z0-9_]*)\s*;", text):
        items.add(name)
    for name in re.findall(r"(?m)^\s*typedef\s+(?!struct|enum)(?:[^;]+\s+)(Oesdk[A-Za-z0-9_]+)\s*;", text):
        items.add(name)
    return items

items: set[str] = set()
for header in HEADERS.glob("*.h"):
    if header.name == "kernel.h":
        continue
    items |= public_items(header.read_text(encoding="utf-8"))

index_text = INDEX.read_text(encoding="utf-8") if INDEX.exists() else ""
missing_pages = sorted(name for name in items if not (API_DIR / f"{name}.md").is_file())
missing_links = sorted(name for name in items if f"({name}.md)" not in index_text and f"(./{name}.md)" not in index_text)
extra_pages = sorted(p.stem for p in API_DIR.glob("*.md") if p.name != "README.md" and p.stem not in items)

if missing_pages or missing_links or extra_pages:
    if missing_pages: print("Missing API pages:", ", ".join(missing_pages))
    if missing_links: print("Missing API index links:", ", ".join(missing_links))
    if extra_pages: print("API pages without public declarations:", ", ".join(extra_pages))
    sys.exit(1)

print(f"[ OK ] API documentation covers {len(items)} public items.")
