
"""Unit tests for JLIB's Worker onboarding infrastructure."""

import unittest

from datetime import datetime, UTC
from unittest.mock import Mock
from typing import Any, cast

from jlib.bot.forge import Forge
from jlib.bot.well import Well
from jlib.bot.worker import Worker


class TestWell(unittest.TestCase):
    """Tests for onboarding newly constructed Workers."""

    def setUp(self):
        """Create a Forge, Worker, and simulated onboarding context."""
        self.forge = Forge()
        self.worker = self.forge.build(Worker)

        self.context = Mock()

        self.context.capture_imprint.return_value = {
            "environment": "test",
            "capabilities": ("basic",),
        }
        self.context.consult.return_value = {
            "knowledge": "initial corpus data",
        }
        self.context.create_state.return_value = {
            "status": "ready",
        }
        self.context.create_experience.return_value = []
        self.context.create_disposition.return_value = {
            "caution": 0.5,
        }

        self.well = Well(self.context)

    def test_onboard(self):
        """The Well initializes a newly forged Worker."""
        result = self.well.onboard(self.worker)

        self.assertTrue(result.initialized)
        self.assertEqual(
            result.imprint["environment"], "test"
        )
        self.assertEqual(
            result.state["status"], "ready"
        )

    def test_preserves_identity(self):
        """Onboarding must not change the Worker's identity."""
        original_id = self.worker.id
        original_born = self.worker.born

        self.well.onboard(self.worker)

        self.assertEqual(self.worker.id, original_id)
        self.assertEqual(self.worker.born, original_born)
        self.assertIsInstance(self.worker.born, datetime)
        self.assertEqual(self.worker.born.tzinfo, UTC)

    def test_returns_same_worker(self):
        """The Well returns the original Worker instance."""
        result = self.well.onboard(self.worker)

        self.assertIs(result, self.worker)

    def test_onboarding_once(self):
        """Workers cannot undergo onboarding more than once."""
        self.well.onboard(self.worker)

        with self.assertRaises(RuntimeError):
            self.well.onboard(self.worker)

    def test_invalid_worker(self):
        """The Well rejects objects that are not Workers."""
        for invalid in (None, "Worker", 42, object()):
            with self.subTest(invalid=invalid):
                with self.assertRaises(TypeError):
                    self.well.onboard(cast(Any, invalid))

    def test_invalid_context(self):
        """The Well requires a valid initialization context."""
        with self.assertRaises(ValueError):
            Well(cast(Any, None))

    def test_failed_onboarding(self):
        """Failed onboarding must not mark a Worker initialized."""
        self.context.create_disposition.side_effect = RuntimeError(
            "Simulated onboarding failure."
        )

        with self.assertRaises(RuntimeError):
            self.well.onboard(self.worker)

        self.assertFalse(self.worker.initialized)
        self.assertIsNone(self.worker.imprint)
        self.assertIsNone(self.worker.state)
        self.assertIsNone(self.worker.disposition)
        self.assertIsNone(self.worker.experience)


if __name__ == "__main__":
    unittest.main()
