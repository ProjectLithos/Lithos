from pathlib import Path
import hashlib, zipfile, sys

root = Path(__file__).resolve().parents[1]
output = root / "Output"
files = []
for p in output.rglob("*"):
    if p.is_file() and (p.suffix in {".a", ".o", ".elf"} or "Debug" in p.parts):
        files.append(p)

if not files:
    raise SystemExit("[FAIL] No built debug libraries or symbols were found. Run BuildNewlib first.")

zip_path = root / "ReleaseAssets" / "OESDK-LibC-Symbols-0.0.13.zip"
zip_path.parent.mkdir(parents=True, exist_ok=True)
with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as z:
    for p in files:
        z.write(p, p.relative_to(output))
digest = hashlib.sha256(zip_path.read_bytes()).hexdigest().upper()
(zip_path.with_suffix(zip_path.suffix + ".sha256")).write_text(
    f"{digest}  {zip_path.name}\n", encoding="ascii"
)
print(f"[ OK ] Created {zip_path}")
