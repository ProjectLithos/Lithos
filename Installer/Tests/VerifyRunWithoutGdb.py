from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
for relative in (
    "VisualStudio/Templates/Kernel/OESDKKernel.vcxproj",
    "VisualStudio/Templates/Desktop/OESDKDesktop.vcxproj",
    "Packages/VisualStudio/Templates/Kernel/OESDKKernel.vcxproj",
    "Packages/VisualStudio/Templates/Desktop/OESDKDesktop.vcxproj",
):
    text = (ROOT / relative).read_text(encoding="utf-8")
    assert "VisualStudio-Launch.ps1" in text, relative
    assert "$(ProjectDir)Debug-Kernel.cmd" not in text, relative
launcher = (ROOT / "Tools/VisualStudio-Launch.ps1").read_text(encoding="utf-8")
assert "IsDebuggerPresent" in launcher
assert "Run-Kernel.ps1" in launcher
assert "Debug-Kernel.ps1" in launcher
print("[ OK ] F5 and Ctrl+F5 are separated by the OESDK Visual Studio launcher.")
