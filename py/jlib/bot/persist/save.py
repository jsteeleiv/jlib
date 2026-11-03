
"""Directory-based save slots for the JLIB bot ecosystem."""

import json
import shutil
import tempfile

from datetime import datetime, UTC
from pathlib import Path
from typing import Any
from uuid import uuid4
from .codec import dumps, loads


_VERSION = 1


def _slot(root: str | Path, name: str) -> Path:
    """Resolve a save slot within its root directory."""
    if (
        not name
        or name in (".", "..")
        or not name.replace("_", "").replace("-", "").isalnum()
    ):
        raise ValueError("Invalid save slot name.")

    return Path(root).expanduser() / name


def save(
    data: dict[str, Any],
    root: str | Path,
    slot: str,
    *,
    overwrite: bool = False,
) -> Path:
    """
    Save named components into a directory-based slot.

    Each component is stored in its own JSON file.
    Existing slots are protected unless overwrite=True.
    """
    target = _slot(root, slot)

    if target.exists() and not overwrite:
        raise FileExistsError(target)

    target.parent.mkdir(parents=True, exist_ok=True)

    # Validate component names before creating any files.
    for name in data:
        if (
            not name
            or not name.replace("_", "").replace("-", "").isalnum()
        ):
            raise ValueError(f"Invalid component name: {name!r}")

    staging = Path(
        tempfile.mkdtemp(
            prefix=f".{slot}.",
            dir=target.parent,
        )
    )

    try:
        components = staging / "data"
        components.mkdir()

        for name, value in data.items():
            (components / f"{name}.json").write_text(
                dumps(value, indent=2),
                encoding="utf-8",
            )

        manifest = {
            "version": _VERSION,
            "slot": slot,
            "created": datetime.now(UTC).isoformat(),
            "components": list(data),
        }

        (staging / "manifest.json").write_text(
            json.dumps(manifest, indent=2),
            encoding="utf-8",
        )

        # # Directory replacement is not universally atomic.
        # # Existing slots are removed only when explicitly allowed.
        # if target.exists():
        #     shutil.rmtree(target)

        # staging.rename(target)

        # Preserve the existing save until the replacement succeeds.
        backup = None

        if target.exists():
            backup = target.with_name(
                f".{target.name}.{uuid4().hex}.bak"
            )
            target.rename(backup)

        try:
            staging.rename(target)

        except OSError:
            # Restore the previous save if publication fails.
            if backup is not None:
                backup.rename(target)
            raise

        else:
            # The replacement succeeded; the old save is no longer needed.
            if backup is not None:
                shutil.rmtree(backup)

    finally:
        if staging.exists():
            shutil.rmtree(staging)

    return target


def inspect(root: str | Path, slot: str) -> dict[str, Any]:
    """Read a save's manifest without loading component data."""
    target = _slot(root, slot)

    with (target / "manifest.json").open(
        "r", encoding="utf-8"
    ) as file:
        manifest = json.load(file)

    if manifest["version"] != _VERSION:
        raise ValueError(
            f"Unsupported save version: {manifest['version']}"
        )

    return manifest


def load(
    root: str | Path,
    slot: str,
    component: str | None = None,
) -> Any:
    """
    Load one component or all components from a save slot.

    Returns a dictionary when component is None.
    """
    target = _slot(root, slot)
    manifest = inspect(root, slot)

    names = manifest["components"]

    if component is not None:
        if component not in names:
            raise KeyError(f"Unknown component: {component}")

        return loads(
            (target / "data" / f"{component}.json").read_text(
                encoding="utf-8"
            )
        )

    return {
        name: loads(
            (target / "data" / f"{name}.json").read_text(
                encoding="utf-8"
            )
        )
        for name in names
    }


def list_saves(root: str | Path) -> list[str]:
    """Return available save slot names."""
    directory = Path(root).expanduser()

    if not directory.is_dir():
        return []

    return sorted(
        path.name
        for path in directory.iterdir()
        if path.is_dir()
        and (path / "manifest.json").is_file()
    )


def delete(root: str | Path, slot: str) -> bool:
    """Delete a save slot, returning False if it does not exist."""
    target = _slot(root, slot)

    if not target.exists():
        return False

    if not (target / "manifest.json").is_file():
        raise ValueError("Target is not a valid save slot.")

    shutil.rmtree(target)
    return True
