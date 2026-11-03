
"""Read-only filesystem inventory Worker."""

from pathlib import Path

from ..task import Task
from ..worker import Worker


class Archivist(Worker):
    """Inspect and catalog filesystem contents."""

    __slots__ = ()

    capabilities = frozenset({"inventory"})

    def work(self, task: Task) -> dict:
        """Produce an inventory of a designated directory."""
        if task.action != "inventory":
            raise ValueError("Archivist requires an inventory task.")

        if not isinstance(task.payload, (str, Path)):
            raise TypeError("Inventory requires a directory path.")

        root = Path(task.payload).expanduser().resolve(strict=True)

        if not root.is_dir():
            raise NotADirectoryError(root)

        files = []
        directories = []

        # Nonrecursive for our first implementation.
        for entry in root.iterdir():
            if entry.is_symlink():
                continue

            if entry.is_dir():
                directories.append(entry.name)

            elif entry.is_file():
                files.append({
                    "name": entry.name,
                    "extension": entry.suffix.lower(),
                    "size": entry.stat().st_size,
                })

        return {
            "path": str(root),
            "files": sorted(files, key=lambda item: item["name"]),
            "directories": sorted(directories),
            "file_count": len(files),
            "directory_count": len(directories),
            "total_bytes": sum(item["size"] for item in files),
        }
