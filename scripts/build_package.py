#!/usr/bin/env python3
"""
build_package.py

Create a release ZIP with the layout:

native-arduino-core/
  cores/
    native/

Compute SHA-256 checksum for the ZIP and update package_index.json in the repo root
while preserving older entries.

Usage:
  python3 scripts/build_package.py --version 0.1.4 --repo owner/repo

The script writes: native-arduino-core-<version>.zip and updates package_index.json
in the repository root.
"""
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()


def repo_base_url_from_repo(repo: str) -> str:
    # repo is owner/repo
    return f"https://github.com/{repo}"


def copy_tree_contents(src: Path, dst: Path):
    dst.mkdir(parents=True, exist_ok=True)
    for item in src.iterdir():
        if item.is_dir():
            shutil.copytree(item, dst / item.name, dirs_exist_ok=True)
        else:
            shutil.copy2(item, dst / item.name)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--version", required=True, help="Version string, e.g. 0.1.4")
    parser.add_argument("--repo", required=False, help="GitHub owner/repo (e.g. owner/repo). If provided, used to build download URLs")
    parser.add_argument("--output-zip", required=False, help="Output zip filename (optional)")
    parser.add_argument("--package-index", default="package_index.json", help="Path to package_index.json to update")
    args = parser.parse_args()

    root = Path.cwd()
    ver = args.version
    top_name = "native-arduino-core"
    top_dir = Path("package") / top_name

    # Clean package dir
    if Path("package").exists():
        shutil.rmtree("package")
    top_dir.mkdir(parents=True, exist_ok=True)

    # Copy cores
    cores_src = root / "cores"
    cores_dst = top_dir / "cores"
    if cores_src.exists() and cores_src.is_dir():
        copy_tree_contents(cores_src, cores_dst)
        print(f"Copied cores from {cores_src} to {cores_dst}")
    else:
        print("Warning: cores/ directory not found in repo; creating empty cores/native placeholder")
        (cores_dst / "native").mkdir(parents=True, exist_ok=True)

    # Copy libraries (if present)
    libs_src = root / "libraries"
    libs_dst = top_dir / "libraries"
    if libs_src.exists() and libs_src.is_dir():
        copy_tree_contents(libs_src, libs_dst)
        print(f"Copied libraries from {libs_src} to {libs_dst}")
    else:
        print("No libraries/ directory found in repo; skipping libraries copy")

    # Optionally put platform.txt and boards.txt into appropriate places
    # Many Arduino packages include platform.txt under the top-level folder or in cores;
    # we will copy platform.txt and boards.txt from repo root if present into top_dir
    for fname in ("platform.txt", "boards.txt"):
        src = root / fname
        if src.exists():
            shutil.copy2(src, top_dir / fname)
            print(f"Copied {fname} to package/{top_name}/{fname}")

    # Create zip
    zip_name = args.output_zip or f"{top_name}-{ver}.zip"
    zip_path = root / zip_name
    # Use shutil.make_archive to produce the zip in package directory, then move
    # but make_archive wants a base_name without extension. We'll create directly
    # from package dir using subprocess zip for consistent behavior.
    cwd = Path("package")
    print(f"Creating zip {zip_path} from {top_dir}...")
    try:
        subprocess.check_call(["zip", "-r", str(zip_path), top_name], cwd=str(cwd))
    except FileNotFoundError:
        # 'zip' tool not available, fallback to shutil.make_archive
        print("zip tool not found; falling back to shutil.make_archive")
        base = root / f"{top_name}-{ver}"
        archive_path = shutil.make_archive(str(base), 'zip', root_dir='package', base_dir=top_name)
        zip_path = Path(archive_path)
    except subprocess.CalledProcessError as e:
        print(f"zip command failed: {e}; attempting fallback to shutil.make_archive")
        base = root / f"{top_name}-{ver}"
        archive_path = shutil.make_archive(str(base), 'zip', root_dir='package', base_dir=top_name)
        zip_path = Path(archive_path)

    # Compute checksum
    checksum = sha256_file(zip_path)
    print(f"SHA-256: {checksum}")

    # Update package_index.json
    pindex_path = root / args.package_index
    if pindex_path.exists():
        with pindex_path.open("r", encoding="utf-8") as f:
            try:
                data = json.load(f)
            except Exception:
                print("Warning: existing package_index.json is invalid JSON; starting fresh")
                data = {"packages": []}
    else:
        data = {"packages": []}

    # Ensure package entry exists
    pkg_name = "lang-ship"
    pkg = None
    for p in data.get("packages", []):
        if p.get("name") == pkg_name:
            pkg = p
            break
    if pkg is None:
        pkg = {"name": pkg_name, "maintainer": "tanakamasayuki", "platforms": []}
        data.setdefault("packages", []).append(pkg)

    # Build platform entry
    if args.repo:
        base = repo_base_url_from_repo(args.repo)
        url = f"{base}/releases/download/v{ver}/{zip_name}"
    else:
        url = f"REPLACE_WITH_RELEASE_URL/{zip_name}"

    platform_entry = {
        "name": "Native Arduino Core",
        "architecture": "native",
        "version": ver,
        "url": url,
        "archiveFileName": zip_name,
        "checksum": f"SHA-256:{checksum}"
    }

    # Replace or append
    platforms = pkg.setdefault("platforms", [])
    replaced = False
    for i, pl in enumerate(platforms):
        if pl.get("version") == ver:
            platforms[i] = platform_entry
            replaced = True
            break
    if not replaced:
        platforms.append(platform_entry)

    # Write updated package_index.json (write to root and to package dir)
    with pindex_path.open("w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
    # Also write package/package_index.json for convenience
    with (root / "package" / "package_index.json").open("w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)

    print(f"Wrote package_index.json with {len(platforms)} platform(s) for package '{pkg_name}'")
    print("Done.")


if __name__ == "__main__":
    main()
