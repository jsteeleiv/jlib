
"""Unit tests for JLIB's Worker Registry."""

import unittest

from concurrent.futures import ThreadPoolExecutor
from unittest.mock import Mock
from typing import Any, cast

from jlib.bot.forge import Forge
from jlib.bot.well import Well
from jlib.bot.worker import Worker
from jlib.bot.registry import Registry


class TestRegistry(unittest.TestCase):
    """Tests for Worker registration and atomic reservations."""

    def setUp(self):
        """Create an isolated Registry and initialized Workers."""
        self.registry = Registry()
        self.forge = Forge()

        context = Mock()

        context.capture_imprint.return_value = {
            "environment": "test",
            "capabilities": ("basic",),
        }
        context.consult.return_value = {}
        context.create_state.return_value = {"status": "ready"}
        context.create_experience.return_value = []
        context.create_disposition.return_value = {}

        self.well = Well(context)

        self.worker_a = self.well.onboard(
            self.forge.build(Worker)
        )
        self.worker_b = self.well.onboard(
            self.forge.build(Worker)
        )

    def test_registration(self):
        """Initialized Workers can register successfully."""
        self.registry.register(self.worker_a)

        self.assertIs(
            self.registry.get(self.worker_a.id),
            self.worker_a,
        )

    def test_duplicate_registration(self):
        """Duplicate Worker identities are rejected."""
        self.registry.register(self.worker_a)

        with self.assertRaises(ValueError):
            self.registry.register(self.worker_a)

    def test_uninitialized_worker(self):
        """Workers must complete onboarding before registration."""
        uninitialized = self.forge.build(Worker)

        with self.assertRaises(RuntimeError):
            self.registry.register(uninitialized)

    def test_available_workers(self):
        """Only unreserved Workers appear as available."""
        self.registry.register(self.worker_a)
        self.registry.register(self.worker_b)

        self.assertEqual(
            len(self.registry.available()),
            2,
        )

        self.registry.reserve(self.worker_a.id, task_id=100)

        available = self.registry.available()

        self.assertEqual(len(available), 1)
        self.assertIs(available[0], self.worker_b)

    def test_reservation_and_release(self):
        """Workers cannot be reserved twice without release."""
        self.registry.register(self.worker_a)

        self.assertTrue(
            self.registry.reserve(self.worker_a.id, 100)
        )
        self.assertFalse(
            self.registry.reserve(self.worker_a.id, 101)
        )

        self.assertEqual(
            self.registry.busy()[self.worker_a.id],
            100,
        )

        self.registry.release(self.worker_a.id)

        self.assertTrue(
            self.registry.reserve(self.worker_a.id, 101)
        )

    def test_unregister(self):
        """Idle Workers can be removed; busy Workers cannot."""
        self.registry.register(self.worker_a)

        self.registry.reserve(self.worker_a.id, 100)

        with self.assertRaises(RuntimeError):
            self.registry.unregister(self.worker_a.id)

        self.registry.release(self.worker_a.id)

        removed = self.registry.unregister(self.worker_a.id)

        self.assertIs(removed, self.worker_a)
        self.assertIsNone(self.registry.get(self.worker_a.id))

    def test_invalid_worker(self):
        """Registry rejects non-Worker objects."""
        for invalid in (None, "Worker", 42):
            with self.subTest(invalid=invalid):
                with self.assertRaises(TypeError):
                    self.registry.register(cast(Any, invalid))

    def test_concurrent_reservation(self):
        """Only one concurrent reservation may succeed."""
        self.registry.register(self.worker_a)

        def attempt(task_id: int) -> bool:
            return self.registry.reserve(
                self.worker_a.id,
                task_id,
            )

        with ThreadPoolExecutor(max_workers=10) as executor:
            results = list(executor.map(attempt, range(10)))

        self.assertEqual(results.count(True), 1)
        self.assertEqual(results.count(False), 9)
        self.assertEqual(len(self.registry.busy()), 1)


if __name__ == "__main__":
    unittest.main()
