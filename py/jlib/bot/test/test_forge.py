
"""Unit tests for JLIB's Bot construction infrastructure."""

import unittest

from jlib.bot.bot import Bot
from jlib.bot.forge import Forge
from jlib.bot.worker import Worker
from typing import Any, cast


class TestForge(unittest.TestCase):
    """Tests for constructing Bots through the Forge."""

    def setUp(self):
        """Create a fresh Forge for each test."""
        self.forge = Forge()

    def test_build_worker(self):
        """Forge constructs a Worker with an assigned identity."""
        worker = self.forge.build(Worker)

        self.assertIsInstance(worker, Worker)
        self.assertIsInstance(worker, Bot)
        self.assertEqual(worker.id, 1)

    def test_sequential_ids(self):
        """Each constructed Bot receives a sequential identity."""
        workers = [self.forge.build(Worker) for _ in range(3)]

        self.assertEqual(
            [worker.id for worker in workers],
            [1, 2, 3],
        )

    def test_custom_start(self):
        """Forge supports a configurable starting identity."""
        forge = Forge(start=100)

        worker = forge.build(Worker)

        self.assertEqual(worker.id, 100)

    def test_specialized_worker(self):
        """Forge constructs concrete Worker subclasses."""

        class SpecializedWorker(Worker):
            def work(self, task):
                return f"Completed: {task}"

        worker = self.forge.build(SpecializedWorker)

        self.assertIsInstance(worker, SpecializedWorker)
        self.assertEqual(worker.id, 1)

    def test_invalid_bot_type(self):
        """Forge rejects objects that are not Bot subclasses."""
        for invalid in (str, int, object, "Worker", None):
            with self.subTest(invalid=invalid):
                with self.assertRaises(TypeError):
                    self.forge.build(cast(Any, invalid))

    def test_invalid_start(self):
        """Forge rejects invalid starting identities."""
        for invalid in (0, -1, True, 1.5, "1"):
            with self.subTest(invalid=invalid):
                with self.assertRaises(ValueError):
                    Forge(start=cast(Any, invalid))

    def test_no_automatic_onboarding(self):
        """Forge constructs Workers without initializing them."""
        worker = self.forge.build(Worker)

        self.assertFalse(worker.initialized)
        self.assertIsNone(worker.imprint)
        self.assertIsNone(worker.state)
        self.assertIsNone(worker.disposition)
        self.assertIsNone(worker.experience)


if __name__ == "__main__":
    unittest.main()
