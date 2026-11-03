
"""Unit tests for JLIB bot swap and recovery checkpoints."""

import json
import tempfile
import unittest

from pathlib import Path

from jlib.bot import swap


class TestSwap(unittest.TestCase):
    """Tests for Vim-inspired recovery checkpoints."""

    def setUp(self):
        """Create an isolated temporary directory for each test."""
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)

        self.path = Path(self.temp.name) / "worker.swap"

        self.data = {
            "worker_id": 42,
            "state": {
                "status": "working",
                "tasks": [1, 2, 3],
            },
        }

    def test_write_read(self):
        """Writing and reading preserves checkpoint data."""
        result = swap.write(self.data, self.path)

        self.assertEqual(result, self.path)
        self.assertTrue(self.path.is_file())
        self.assertEqual(swap.read(self.path), self.data)

    def test_exists(self):
        """Checkpoint existence is reported correctly."""
        self.assertFalse(swap.exists(self.path))

        swap.write(self.data, self.path)

        self.assertTrue(swap.exists(self.path))

    def test_overwrite(self):
        """Existing checkpoints can be updated or protected."""
        swap.write(self.data, self.path)

        updated = {"worker_id": 42, "status": "idle"}
        swap.write(updated, self.path)

        self.assertEqual(swap.read(self.path), updated)

        with self.assertRaises(FileExistsError):
            swap.write(self.data, self.path, overwrite=False)

        self.assertEqual(swap.read(self.path), updated)

    def test_discard(self):
        """Discard removes checkpoints and handles missing files."""
        swap.write(self.data, self.path)

        self.assertTrue(swap.discard(self.path))
        self.assertFalse(swap.exists(self.path))
        self.assertFalse(swap.discard(self.path))

    def test_inspect(self):
        """Inspection returns metadata without restoring data."""
        swap.write(self.data, self.path)

        info = swap.inspect(self.path)

        self.assertEqual(info["path"], str(self.path))
        self.assertEqual(info["version"], 1)
        self.assertIn("created", info)
        self.assertGreater(info["size"], 0)

    def test_missing_file(self):
        """Reading a nonexistent checkpoint raises FileNotFoundError."""
        with self.assertRaises(FileNotFoundError):
            swap.read(self.path)

    def test_unsupported_version(self):
        """Checkpoints with unsupported versions are rejected."""
        swap.write(self.data, self.path)

        checkpoint = json.loads(
            self.path.read_text(encoding="utf-8")
        )
        checkpoint["version"] = 999

        self.path.write_text(
            json.dumps(checkpoint),
            encoding="utf-8",
        )

        with self.assertRaises(ValueError):
            swap.read(self.path)

    def test_failed_serialization(self):
        """Serialization failure must preserve an existing checkpoint."""
        swap.write(self.data, self.path)

        original = self.path.read_bytes()

        class Unsupported:
            pass

        with self.assertRaises(TypeError):
            swap.write(Unsupported(), self.path)

        self.assertEqual(self.path.read_bytes(), original)


if __name__ == "__main__":
    unittest.main()
