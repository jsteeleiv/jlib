
"""Unit tests for JLIB's shared synchronization Arbiter."""

import unittest

from concurrent.futures import ThreadPoolExecutor
from threading import Barrier, Lock
from time import sleep

from jlib.bot.arbiter import Arbiter


class TestArbiter(unittest.TestCase):
    """Tests for synchronized critical operations."""

    def setUp(self):
        """Create a fresh Arbiter for each test."""
        self.arbiter = Arbiter()

    def test_critical_section(self):
        """Critical sections can be entered and exited."""
        with self.arbiter.critical():
            value = 42

        self.assertEqual(value, 42)

    def test_reentrant_lock(self):
        """The same thread can enter nested critical sections."""
        with self.arbiter.critical():
            with self.arbiter.critical():
                result = "nested"

        self.assertEqual(result, "nested")

    def test_exception_cleanup(self):
        """Exceptions must not leave the Arbiter locked."""
        with self.assertRaises(ValueError):
            with self.arbiter.critical():
                raise ValueError("Simulated failure")

        # The Arbiter must remain usable afterward.
        with self.arbiter.critical():
            result = "recovered"

        self.assertEqual(result, "recovered")

    def test_shared_state(self):
        """Concurrent threads safely update shared state."""
        counter = 0

        def increment() -> None:
            nonlocal counter

            with self.arbiter.critical():
                current = counter
                sleep(0.0001)
                counter = current + 1

        with ThreadPoolExecutor(max_workers=8) as executor:
            list(executor.map(
                lambda _: increment(),
                range(100),
            ))

        self.assertEqual(counter, 100)

    def test_mutual_exclusion(self):
        """Only one thread may occupy a critical section at a time."""
        active = 0
        maximum = 0
        guard = Lock()
        barrier = Barrier(8)

        def enter() -> None:
            nonlocal active, maximum

            barrier.wait(timeout=5)

            with self.arbiter.critical():
                with guard:
                    active += 1
                    maximum = max(maximum, active)

                sleep(0.001)

                with guard:
                    active -= 1

        with ThreadPoolExecutor(max_workers=8) as executor:
            list(executor.map(
                lambda _: enter(),
                range(8),
            ))

        self.assertEqual(maximum, 1)
        self.assertEqual(active, 0)


if __name__ == "__main__":
    unittest.main()
