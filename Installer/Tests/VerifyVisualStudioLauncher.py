from pathlib import Path
root=Path(__file__).resolve().parents[1]
for rel in ['Tools/VisualStudio-Launch.ps1','Packages/x86_64/Tools/VisualStudio-Launch.ps1','InstallerSource/Packages/x86_64/Tools/VisualStudio-Launch.ps1']:
    text=(root/rel).read_text(encoding='utf-8-sig')
    assert 'oesdk-visualstudio-launch.log' in text
    assert "[Console]::ReadLine()" in text
    assert 'Child launcher exit code' in text
print('[ OK ] Visual Studio launcher diagnostics contract.')
