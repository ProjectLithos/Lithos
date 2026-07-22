#!/usr/bin/env python3
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "Include" / "oesdk" / "status.h"
text = HEADER.read_text(encoding="utf-8")

required = {
    "OESDK_STATUS_SUCCESS": 0,
    "OESDK_STATUS_INVALID_ARGUMENT": -1,
    "OESDK_STATUS_NOT_SUPPORTED": -2,
    "OESDK_STATUS_OUT_OF_MEMORY": -3,
    "OESDK_STATUS_NOT_FOUND": -4,
    "OESDK_STATUS_ALREADY_EXISTS": -5,
    "OESDK_STATUS_BUSY": -6,
    "OESDK_STATUS_TIMEOUT": -7,
    "OESDK_STATUS_CORRUPT": -8,
}

assert re.search(r"typedef\s+int32_t\s+OesdkStatus\s*;", text), "OesdkStatus must be int32_t"
for name, value in required.items():
    pattern = rf"#define\s+{name}\s+\(\(OesdkStatus\)({value})\)"
    assert re.search(pattern, text), f"Missing or incorrect {name}"
assert "#define OESDK_SUCCEEDED(Status)" in text
assert "#define OESDK_FAILED(Status)" in text
print("[ OK ] OESDK 0.0.16.3 status contract verified.")
