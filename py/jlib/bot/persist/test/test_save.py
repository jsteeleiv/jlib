
"""Unit tests for JLIB's directory-based save system."""

import unittest
import tempfile

from pathlib import Path
from unittest.mock import patch
from py.jlib.bot.persist import save


class TestSave(unittest.TestCase):
    """Tests for ecosystem save slots."""

    def setUp(self):
        """Create an isolated save directory for each test."""
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)

        self.root = Path(self.temp.name)

        self.data = {
            "worker_001": {
                "id": 1,
                "experience": ["task_a", "task_b"],
            },
            "registry": {
                "workers": [1],
            },
            "board": {
                "pending": [],
            },
        }

    def test_save_creation(self):
        """Saving creates the expected slot structure."""
        path = save.save(self.data, self.root, "slot_001")

        self.assertTrue(path.is_dir())
        self.assertTrue((path / "manifest.json").is_file())
        self.assertTrue((path / "data" / "worker_001.json").is_file())
        self.assertTrue((path / "data" / "registry.json").is_file())
        self.assertTrue((path / "data" / "board.json").is_file())

    def test_load_component(self):
        """Individual components can be loaded independently."""
        save.save(self.data, self.root, "slot_001")

        worker = save.load(
            self.root,
            "slot_001",
            component="worker_001",
        )

        self.assertEqual(worker, self.data["worker_001"])

    def test_load_complete_save(self):
        """Loading an entire slot reconstructs all components."""
        save.save(self.data, self.root, "slot_001")

        restored = save.load(self.root, "slot_001")

        self.assertEqual(restored, self.data)

    def test_inspect(self):
        """Inspection reads metadata without loading components."""
        save.save(self.data, self.root, "slot_001")

        info = save.inspect(self.root, "slot_001")

        self.assertEqual(info["version"], 1)
        self.assertEqual(info["slot"], "slot_001")
        self.assertEqual(
            set(info["components"]),
            set(self.data),
        )
        self.assertIn("created", info)

    def test_list_saves(self):
        """Available save slots are listed alphabetically."""
        save.save(self.data, self.root, "slot_002")
        save.save(self.data, self.root, "slot_001")

        slots = save.list_saves(self.root)

        self.assertEqual(
            slots,
            ["slot_001", "slot_002"],
        )

    def test_overwrite_protection(self):
        """Existing saves cannot be overwritten without permission."""
        save.save(self.data, self.root, "slot_001")

        with self.assertRaises(FileExistsError):
            save.save(
                {"worker_002": {"id": 2}},
                self.root,
                "slot_001",
            )

        # The original save must remain unchanged.
        restored = save.load(self.root, "slot_001")

        self.assertEqual(restored, self.data)

    def test_invalid_names(self):
        """Invalid slot and component names are rejected."""
        with self.assertRaises(ValueError):
            save.save(self.data, self.root, "../escape")

        with self.assertRaises(ValueError):
            save.save(
                {"../escape": {"id": 1}},
                self.root,
                "slot_001",
            )

    def test_delete(self):
        """Deleting a save removes its directory."""
        save.save(self.data, self.root, "slot_001")

        self.assertTrue(
            save.delete(self.root, "slot_001")
        )

        self.assertFalse(
            (self.root / "slot_001").exists()
        )

        self.assertFalse(
            save.delete(self.root, "slot_001")
        )

    def test_overwrite_rollback(self):
        """A failed replacement must preserve the original save."""
        save.save(self.data, self.root, "slot_001")

        original = save.load(self.root, "slot_001")
        target = self.root / "slot_001"

        real_rename = Path.rename

        def fail_publication(path, destination):
            if (
                path.name.startswith(".slot_001.")
                and path.name != target.name
                and path.suffix != ".bak"
                and Path(destination) == target
            ):
                raise OSError("Simulated publication failure")

            return real_rename(path, destination)

        with patch.object(Path, "rename", fail_publication):
            with self.assertRaises(OSError):
                save.save(
                    {"worker_002": {"id": 2}},
                    self.root,
                    "slot_001",
                    overwrite=True,
                )

        self.assertEqual(
            save.load(self.root, "slot_001"),
            original,
        )

if __name__ == "__main__":
    unittest.main()
