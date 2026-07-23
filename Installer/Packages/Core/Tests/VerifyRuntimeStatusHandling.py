from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
paths = [
    ROOT / "Source/runtime.c",
    ROOT / "Packages/Core/Source/runtime.c",
    ROOT / "InstallerSource/Source/runtime.c",
    ROOT / "InstallerSource/Packages/Core/Source/runtime.c",
]
for path in paths:
    text = path.read_text(encoding="utf-8")
    assert "if (!OesdkIdtInitialize())" not in text, path
    assert "OesdkStatus Status = OesdkIdtInitialize();" in text, path
    assert "OESDK_FAILED(Status)" in text, path
print("[ OK ] Runtime status-returning initializers are not treated as booleans.")
