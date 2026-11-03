
"""Vim-inspired recovery checkpoints for the JLIB bot ecosystem."""

import json
import os
import tempfile

from datetime import datetime, UTC
from pathlib import Path
from typing import Any

from .codec import encode, decode


_VERSION = 1


def _path(path: str | Path) -> Path:
    """Normalize a checkpoint path."""
    return Path(path).expanduser()


def write(
    data: Any,
    path: str | Path,
    *,
    overwrite: bool = True,
) -> Path:
    """
    Write a recovery checkpoint using atomic replacement.

    Existing checkpoints are replaced by default because swap
    files represent evolving recovery state.
    """
    target = _path(path)

    if target.exists() and not overwrite:
        raise FileExistsError(target)

    # Serialize before touching the filesystem.
    checkpoint = {
        "version": _VERSION,
        "created": datetime.now(UTC).isoformat(),
        "data": encode(data),
    }

    payload = json.dumps(checkpoint, indent=2)

    # The destination directory must already exist.
    if not target.parent.is_dir():
        raise FileNotFoundError(target.parent)

    temporary = None

    try:
        with tempfile.NamedTemporaryFile(
            mode="w",
            encoding="utf-8",
            dir=target.parent,
            prefix=f".{target.name}.",
            suffix=".tmp",
            delete=False,
        ) as file:
            temporary = Path(file.name)
            file.write(payload)
            file.flush()
            os.fsync(file.fileno())

        os.replace(temporary, target)

    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)

    return target


def read(path: str | Path) -> Any:
    """Read and reconstruct a recovery checkpoint."""
    target = _path(path)

    with target.open("r", encoding="utf-8") as file:
        checkpoint = json.load(file)

    if checkpoint["version"] != _VERSION:
        raise ValueError(
            f"Unsupported swap version: {checkpoint['version']}"
        )

    return decode(checkpoint["data"])


def exists(path: str | Path) -> bool:
    """Check whether a recovery checkpoint exists."""
    return _path(path).is_file()


def discard(path: str | Path) -> bool:
    """
    Remove a recovery checkpoint.

    Return True if removed, False if already absent.
    """
    target = _path(path)

    try:
        target.unlink()
    except FileNotFoundError:
        return False

    return True


def inspect(path: str | Path) -> dict[str, Any]:
    """Return checkpoint metadata without reconstructing its data."""
    target = _path(path)

    with target.open("r", encoding="utf-8") as file:
        checkpoint = json.load(file)

    return {
        "path": str(target),
        "version": checkpoint["version"],
        "created": checkpoint["created"],
        "size": target.stat().st_size,
    }
