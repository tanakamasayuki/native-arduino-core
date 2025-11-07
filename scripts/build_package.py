#!/usr/bin/env python3
"""Build a Boards Manager package zip and update package_index.json.

This script packages the `cores/` and optionally a top-level `variants/`
directory if it exists in the repository root. It will NOT create a
placeholder `variants/` directory — that was the recent change request.

It also writes a minimal `package_index.json` describing the generated ZIP
and copies it into `package/package_index.json`.
"""

import argparse
import hashlib
import json
import shutil
from pathlib import Path
import sys


def sha256_file(p: Path):
    h = hashlib.sha256()
    with p.open('rb') as f:
        for chunk in iter(lambda: f.read(8192), b''):
            h.update(chunk)
    return h.hexdigest()


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--version', required=True)
    p.add_argument('--repo', required=True, help='owner/repo')
    p.add_argument('--package-index', default='package_index.json')
    args = p.parse_args()

    root = Path(__file__).resolve().parents[1]
    pkg_name = 'native-arduino-core'
    ver = args.version
    owner_repo = args.repo
    owner = owner_repo.split('/')[0]

    package_dir = root / 'package'
    if package_dir.exists():
        shutil.rmtree(package_dir)
    package_dir.mkdir(parents=True)

    work = package_dir / pkg_name
    work.mkdir()

    # Copy cores
    cores_src = root / 'cores'
    if not cores_src.exists():
        print('Error: cores/ not found in repo root', file=sys.stderr)
        sys.exit(2)
    shutil.copytree(cores_src, work / 'cores')

    # Copy boards/platform files
    for fn in ('platform.txt', 'boards.txt'):
        src = root / fn
        if src.exists():
            shutil.copy2(src, work / fn)

    # Only include variants if present at repo root
    variants_src = root / 'variants'
    if variants_src.exists() and variants_src.is_dir():
        shutil.copytree(variants_src, work / 'variants')

    # Create zip
    zip_name = f"{pkg_name}-{ver}.zip"
    zip_path = package_dir / zip_name
    # shutil.make_archive expects base_name without .zip
    shutil.make_archive(str(zip_path.with_suffix('')), 'zip', root_dir=package_dir, base_dir=pkg_name)

    sha = sha256_file(zip_path)
    size = zip_path.stat().st_size

    # Build minimal package_index.json structure
    package_url = f"https://{owner}.github.io/{pkg_name}/{zip_name}"
    pkg_entry = {
        'packages': [
            {
                'name': 'lang-ship',
                'maintainer': owner,
                'websiteURL': f'https://github.com/{owner_repo}',
                'platforms': [
                    {
                        'name': 'Native Arduino Core',
                        'architecture': 'native',
                        'version': ver,
                        'category': 'Contributed',
                        'size': size,
                        'archiveFileName': zip_name,
                        'url': package_url,
                        'checksum': {
                            'type': 'sha256',
                            'value': sha
                        }
                    }
                ]
            }
        ]
    }

    # Update root package_index.json by attempting to merge platforms if file exists
    pkg_index_path = root / args.package_index
    if pkg_index_path.exists():
        try:
            existing = json.loads(pkg_index_path.read_text(encoding='utf-8'))
        except Exception:
            existing = None
        if isinstance(existing, dict) and 'packages' in existing:
            merged = existing
            names = [p.get('name') for p in merged.get('packages', [])]
            if 'lang-ship' in names:
                merged['packages'] = [p for p in merged['packages'] if p.get('name') != 'lang-ship']
            merged['packages'].append(pkg_entry['packages'][0])
            pkg_entry = merged

    # Write package_index.json to root and to package/ for gh-pages
    pkg_index_path.write_text(json.dumps(pkg_entry, indent=2, ensure_ascii=False), encoding='utf-8')
    (package_dir / 'package_index.json').write_text(json.dumps(pkg_entry, indent=2, ensure_ascii=False), encoding='utf-8')

    print(f"Created {zip_path} ({size} bytes)")
    print(f"SHA-256: {sha}")
    print(f"Wrote package_index.json with platform version {ver}")


if __name__ == '__main__':
    main()
