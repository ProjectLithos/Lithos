from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
for relative in (
    "VisualStudio/Templates/Kernel/OESDKKernel.vcxproj",
    "VisualStudio/Templates/Desktop/OESDKDesktop.vcxproj",
    "Packages/VisualStudio/Templates/Kernel/OESDKKernel.vcxproj",
    "Packages/VisualStudio/Templates/Desktop/OESDKDesktop.vcxproj",
):
    text = (ROOT / relative).read_text(encoding="utf-8")
    assert "$(ProjectDir)Run-Kernel.cmd" in text, relative
    assert "$(ProjectDir)Debug-Kernel.cmd" not in text, relative
print("[ OK ] Normal Visual Studio run path does not launch GDB.")
