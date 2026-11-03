
"""Unit tests for JLIB's ecosystem Governer."""

import unittest

from unittest.mock import Mock
from typing import Any, cast

from jlib.bot.bot import Bot
from jlib.bot.worker import Worker
from jlib.bot.forge import Forge
from py.jlib.bot.gauntlet import Gauntlet
from jlib.bot.governer import Governer


class TestGoverner(unittest.TestCase):
    """Tests for ecosystem governance and bot construction."""

    def setUp(self):
        """Create isolated infrastructure for each test."""
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

        self.forge = Forge()
        self.gauntlet = Gauntlet(self.context)
        self.governer = Governer(self.forge, self.gauntlet)

    def test_initialization(self):
        """Governer starts inactive with its assigned infrastructure."""
        self.assertFalse(self.governer.active)
        self.assertIs(self.governer.forge, self.forge)
        self.assertIs(self.governer.gauntlet, self.gauntlet)

    def test_lifecycle(self):
        """Governer can be started and stopped."""
        self.governer.start()
        self.assertTrue(self.governer.active)

        self.governer.stop()
        self.assertFalse(self.governer.active)

    def test_invalid_lifecycle(self):
        """Repeated lifecycle transitions raise RuntimeError."""
        with self.assertRaises(RuntimeError):
            self.governer.stop()

        self.governer.start()

        with self.assertRaises(RuntimeError):
            self.governer.start()

    def test_create_worker(self):
        """Governer constructs and onboards Workers."""
        self.governer.start()

        worker = self.governer.create(Worker)

        self.assertIsInstance(worker, Worker)
        self.assertIsInstance(worker, Bot)
        self.assertEqual(worker.id, 1)
        self.assertTrue(worker.initialized)
        self.assertIsNotNone(worker.imprint)
        self.assertIsNotNone(worker.disposition)

    def test_create_specialized_worker(self):
        """Governer supports specialized Worker subclasses."""

        class Specialist(Worker):
            def work(self, task):
                return f"Completed: {task}"

        self.governer.start()

        worker = self.governer.create(Specialist)

        self.assertIsInstance(worker, Specialist)
        self.assertTrue(worker.initialized)
        self.assertEqual(
            worker.execute("inspection"),
            "Completed: inspection",
        )

    def test_inactive_creation(self):
        """Governer rejects construction while inactive."""
        with self.assertRaises(RuntimeError):
            self.governer.create(Worker)

    def test_invalid_dependencies(self):
        """Governer requires valid Forge and Gauntlet instances."""
        with self.assertRaises(TypeError):
            Governer(cast(Any, None), self.gauntlet)

        with self.assertRaises(TypeError):
            Governer(self.forge, cast(Any, None))


if __name__ == "__main__":
    unittest.main()
