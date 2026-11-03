
"""Unit tests for JLIB bot state and immutable imprints."""

import unittest

from datetime import datetime, UTC

from py.jlib.bot.persist.state import Freezer, Imprint, freeze, thaw


class TestState(unittest.TestCase):
    """Tests for freezing and preserving Worker imprints."""

    def test_primitives(self):
        """Primitive values remain unchanged."""
        for value in (None, True, 42, 3.14, "JLIB", b"data"):
            with self.subTest(value=value):
                self.assertEqual(freeze(value), value)

    def test_collection_types(self):
        """Freezing preserves original collection types."""
        values = {
            "list": [1, 2],
            "tuple": (1, 2),
            "set": {1, 2},
            "frozenset": frozenset({1, 2}),
        }

        frozen = freeze(values)

        self.assertIsInstance(frozen["list"], Freezer)
        self.assertEqual(frozen["list"].type, "list")

        self.assertIsInstance(frozen["tuple"], tuple)

        self.assertIsInstance(frozen["set"], Freezer)
        self.assertEqual(frozen["set"].type, "set")

        self.assertIsInstance(frozen["frozenset"], frozenset)

    def test_nested_freezing(self):
        """Nested collections are frozen recursively."""
        original = {
            "hardware": {
                "features": ["network", "filesystem"],
            }
        }

        frozen = freeze(original)
        features = frozen["hardware"]["features"]

        self.assertIsInstance(features, Freezer)
        self.assertEqual(
            features.value,
            ("network", "filesystem"),
        )

    def test_source_independence(self):
        """Changes to original data do not alter frozen values."""
        original = {"features": ["network"]}

        frozen = freeze(original)
        original["features"].append("filesystem")

        self.assertEqual(
            frozen["features"].value,
            ("network",),
        )

    def test_imprint_creation(self):
        """Imprint stores its original characteristics."""
        born = datetime.now(UTC)

        imprint = Imprint(
            born=born,
            environment="Windows",
            capabilities=("read", "inspect"),
        )

        self.assertEqual(imprint.born, born)
        self.assertEqual(imprint.environment, "Windows")
        self.assertEqual(
            imprint.capabilities,
            ("read", "inspect"),
        )

    def test_imprint_immutability(self):
        """Imprint metadata remains unchanged after source mutation."""
        original = {"cores": 8, "features": ["network"]}

        imprint = Imprint(
            born=datetime.now(UTC),
            environment="Windows",
            metadata=original,
        )

        original["cores"] = 16
        original["features"].append("filesystem")

        self.assertEqual(imprint.metadata["cores"], 8)
        self.assertEqual(
            imprint.metadata["features"].value,
            ("network",),
        )

    def test_invalid_timestamp(self):
        """Imprint rejects timestamps without timezone information."""
        with self.assertRaises(ValueError):
            Imprint(
                born=datetime(2026, 11, 2),
                environment="Windows",
            )

    def test_unsupported_object(self):
        """Unsupported objects raise TypeError."""

        class Unsupported:
            pass

        with self.assertRaises(TypeError):
            freeze(Unsupported())

    def test_freeze_thaw_roundtrip(self):
        """Freezing and thawing preserve values and collection types."""
        original = {
            "numbers": [1, 2, 3],
            "coordinates": (10, 20),
            "tags": {"python", "jlib"},
        }

        restored = thaw(freeze(original))

        self.assertEqual(restored, original)
        self.assertIsInstance(restored["numbers"], list)
        self.assertIsInstance(restored["coordinates"], tuple)
        self.assertIsInstance(restored["tags"], set)

if __name__ == "__main__":
    unittest.main()
