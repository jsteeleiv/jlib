
"""Unit tests for JLIB's immutable state snapshots."""

import unittest

from datetime import datetime, UTC

from py.jlib.bot.persist.snap import Snapshot, capture
from py.jlib.bot.persist.state import Freezer


class TestSnapshot(unittest.TestCase):
    """Tests for capturing and restoring immutable snapshots."""

    def test_capture(self):
        """Capture creates a timestamped Snapshot."""
        before = datetime.now(UTC)

        snapshot = capture({"worker": 42})

        after = datetime.now(UTC)

        self.assertIsInstance(snapshot, Snapshot)
        self.assertGreaterEqual(snapshot.created, before)
        self.assertLessEqual(snapshot.created, after)
        self.assertEqual(snapshot.created.tzinfo, UTC)

    def test_source_independence(self):
        """Changes to original data cannot alter a snapshot."""
        original = {
            "worker": 42,
            "experience": ["task_a", "task_b"],
        }

        snapshot = capture(original)

        original["worker"] = 99
        original["experience"].append("task_c")

        restored = snapshot.restore()

        self.assertEqual(restored["worker"], 42)
        self.assertEqual(
            restored["experience"],
            ["task_a", "task_b"],
        )

    def test_nested_collections(self):
        """Nested collection types are preserved."""
        original = {
            "tasks": [1, 2, 3],
            "capabilities": {"read", "write"},
            "coordinates": (10, 20),
        }

        snapshot = capture(original)
        restored = snapshot.restore()

        self.assertEqual(restored, original)
        self.assertIsInstance(restored["tasks"], list)
        self.assertIsInstance(restored["capabilities"], set)
        self.assertIsInstance(restored["coordinates"], tuple)

        self.assertIsInstance(
            snapshot.data["tasks"],
            Freezer,
        )

    def test_independent_restoration(self):
        """Restoring twice produces independent mutable data."""
        snapshot = capture({
            "experience": ["initial"],
        })

        first = snapshot.restore()
        second = snapshot.restore()

        first["experience"].append("changed")

        self.assertEqual(
            second["experience"],
            ["initial"],
        )

        self.assertEqual(
            snapshot.restore()["experience"],
            ["initial"],
        )

    def test_invalid_timestamp(self):
        """Snapshots reject timezone-naive timestamps."""
        with self.assertRaises(ValueError):
            Snapshot(
                created=datetime(2026, 11, 2),
                data={"worker": 42},
            )

    def test_unsupported_data(self):
        """Unsupported objects cannot be captured."""

        class Unsupported:
            pass

        with self.assertRaises(TypeError):
            capture({"object": Unsupported()})


if __name__ == "__main__":
    unittest.main()
