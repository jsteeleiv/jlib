
"""Unit tests for JLIB's type-aware JSON codec."""

import unittest

from datetime import datetime, UTC
from types import MappingProxyType

from jlib.bot.codec import encode, decode, dumps, loads
from jlib.bot.state import Freezer, Imprint, freeze


class TestCodec(unittest.TestCase):
    """Test encoding, decoding, and JSON round trips."""

    def test_primitives(self):
        """Primitive values survive encoding and decoding."""
        for value in (None, True, False, 42, 3.14, "JLIB"):
            with self.subTest(value=value):
                self.assertEqual(loads(dumps(value)), value)

    def test_collections(self):
        """Collection types survive JSON serialization."""
        original = {
            "list": [1, 2],
            "tuple": (3, 4),
            "set": {5, 6},
            "frozenset": frozenset({7, 8}),
        }

        restored = loads(dumps(original))

        self.assertEqual(restored, original)

        for key, value in original.items():
            self.assertIs(type(restored[key]), type(value))

    def test_nested_structures(self):
        """Nested collections retain their values and types."""
        original = {
            42: {
                "items": [1, (2, 3), {"JLIB", "Python"}],
            }
        }

        restored = loads(dumps(original))

        self.assertEqual(restored, original)
        self.assertIn(42, restored)
        self.assertIsInstance(restored[42]["items"][1], tuple)

    def test_mapping_proxy(self):
        """Read-only mappings use the Mapping encoder."""
        original = MappingProxyType({"cores": 8})

        encoded = encode(original)
        restored = decode(encoded)

        self.assertEqual(restored, {"cores": 8})
        self.assertIsInstance(restored, dict)

    def test_datetime_and_bytes(self):
        """Datetime and bytes values survive round trips."""
        original = {
            "timestamp": datetime.now(UTC),
            "data": b"\x00\x01\xff",
        }

        restored = loads(dumps(original))

        self.assertEqual(restored, original)
        self.assertIsInstance(restored["timestamp"], datetime)
        self.assertIsInstance(restored["data"], bytes)

    def test_freezer(self):
        """Freezer preserves its stored type and frozen contents."""
        original = freeze([1, 2, 3])

        restored = loads(dumps(original))

        self.assertIsInstance(restored, Freezer)
        self.assertEqual(restored.type, "list")
        self.assertEqual(restored.value, (1, 2, 3))

    def test_imprint(self):
        """Imprints survive serialization with immutable metadata."""
        original = Imprint(
            born=datetime.now(UTC),
            environment="Windows",
            capabilities=("read", "inspect"),
            metadata={"features": ["network", "filesystem"]},
        )

        restored = loads(dumps(original))

        self.assertEqual(restored.born, original.born)
        self.assertEqual(restored.environment, original.environment)
        self.assertEqual(restored.capabilities, original.capabilities)

        self.assertIsInstance(restored.metadata, MappingProxyType)
        self.assertIsInstance(restored.metadata["features"], Freezer)

    def test_unsupported_type(self):
        """Unsupported objects must raise TypeError."""

        class Unsupported:
            pass

        with self.assertRaises(TypeError):
            encode(Unsupported())

    def test_invalid_encoded_type(self):
        """Unknown encoded type identifiers are rejected."""
        with self.assertRaises(TypeError):
            decode({"type": "unknown", "value": 123})


if __name__ == "__main__":
    unittest.main()
