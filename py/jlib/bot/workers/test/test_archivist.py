
"""Unit tests for JLIB's filesystem Archivist Worker."""

import tempfile
import unittest

from pathlib import Path
from unittest.mock import Mock

from jlib.bot.task import Task
from jlib.bot.workers.archivist import Archivist


class TestArchivist(unittest.TestCase):
    """Tests for read-only filesystem inventory."""

    def setUp(self):
        """Create a temporary directory and initialized Archivist."""
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)

        self.root = Path(self.temp.name)

        self.archivist = Archivist(1)

        context = Mock()
        context.capture_imprint.return_value = {}
        context.consult.return_value = {}
        context.create_state.return_value = {}
        context.create_experience.return_value = []
        context.create_disposition.return_value = {}

        self.archivist.initialize(context)

    def test_file_inventory(self):
        """Archivist catalogs files, extensions, and sizes."""
        (self.root / "notes.txt").write_text(
            "Hello JLIB",
            encoding="utf-8",
        )

        (self.root / "data.json").write_text(
            "{}",
            encoding="utf-8",
        )

        result = self.archivist.execute(
            Task(1, "inventory", self.root)
        )

        self.assertEqual(result["file_count"], 2)
        self.assertEqual(result["directory_count"], 0)
        self.assertEqual(result["total_bytes"], 12)

        self.assertEqual(
            [file["name"] for file in result["files"]],
            ["data.json", "notes.txt"],
        )

    def test_directory_inventory(self):
        """Archivist identifies immediate subdirectories."""
        (self.root / "documents").mkdir()
        (self.root / "images").mkdir()

        result = self.archivist.execute(
            Task(2, "inventory", self.root)
        )

        self.assertEqual(result["directory_count"], 2)
        self.assertEqual(
            result["directories"],
            ["documents", "images"],
        )

    def test_empty_directory(self):
        """Empty directories produce empty inventories."""
        result = self.archivist.execute(
            Task(3, "inventory", self.root)
        )

        self.assertEqual(result["file_count"], 0)
        self.assertEqual(result["directory_count"], 0)
        self.assertEqual(result["total_bytes"], 0)

    def test_nonrecursive(self):
        """Archivist does not inspect nested directory contents."""
        nested = self.root / "nested"
        nested.mkdir()

        (nested / "hidden.txt").write_text(
            "Not included",
            encoding="utf-8",
        )

        result = self.archivist.execute(
            Task(4, "inventory", self.root)
        )

        self.assertEqual(result["file_count"], 0)
        self.assertEqual(result["directory_count"], 1)

    def test_invalid_path(self):
        """Missing directories raise FileNotFoundError."""
        missing = self.root / "missing"

        with self.assertRaises(FileNotFoundError):
            self.archivist.execute(
                Task(5, "inventory", missing)
            )

    def test_wrong_action(self):
        """Archivist rejects unsupported Task actions."""
        with self.assertRaises(ValueError):
            self.archivist.execute(
                Task(6, "scrape", self.root)
            )


if __name__ == "__main__":
    unittest.main()
