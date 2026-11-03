
"""Unit tests for JLIB's thread-safe Task Queue."""

import unittest

from concurrent.futures import ThreadPoolExecutor
from typing import Any, cast

from jlib.bot.task import Task
from jlib.bot.queue import TaskQueue


class TestTaskQueue(unittest.TestCase):
    """Tests for priority-based task queuing."""

    def setUp(self):
        """Create an empty Task Queue for each test."""
        self.queue = TaskQueue()

    def test_put_get(self):
        """Tasks can be added and retrieved."""
        task = Task(1, "inspect")

        self.queue.put(task)

        self.assertEqual(len(self.queue), 1)
        self.assertIs(self.queue.get(), task)
        self.assertEqual(len(self.queue), 0)

    def test_priority_order(self):
        """Lower priority numbers are retrieved first."""
        tasks = [
            Task(1, "low", priority=5),
            Task(2, "critical", priority=0),
            Task(3, "normal", priority=3),
            Task(4, "high", priority=1),
        ]

        for task in tasks:
            self.queue.put(task)

        self.assertEqual(
            [self.queue.get().id for _ in tasks],
            [2, 4, 3, 1],
        )

    def test_fifo_order(self):
        """Equal-priority Tasks retain insertion order."""
        for identity in (10, 20, 30):
            self.queue.put(
                Task(identity, "process", priority=2)
            )

        self.assertEqual(
            [self.queue.get().id for _ in range(3)],
            [10, 20, 30],
        )

    def test_duplicate_task(self):
        """Duplicate Task IDs cannot coexist in the Queue."""
        self.queue.put(Task(1, "inspect"))

        with self.assertRaises(ValueError):
            self.queue.put(Task(1, "process"))

        self.assertEqual(len(self.queue), 1)

    def test_peek_and_snapshot(self):
        """Inspection does not remove or reorder queued Tasks."""
        self.queue.put(Task(1, "low", priority=3))
        self.queue.put(Task(2, "high", priority=0))

        next_task = self.queue.peek()

        assert next_task is not None
        self.assertEqual(next_task.id, 2)

        snapshot = self.queue.snapshot()

        self.assertEqual(
            [task.id for task in snapshot],
            [2, 1],
        )

        self.assertEqual(len(self.queue), 2)

    def test_empty_queue(self):
        """Empty Queue operations behave predictably."""
        self.assertFalse(self.queue)
        self.assertIsNone(self.queue.peek())
        self.assertEqual(self.queue.snapshot(), ())

        with self.assertRaises(IndexError):
            self.queue.get()

    def test_invalid_input(self):
        """The Queue rejects invalid Tasks and priorities."""
        with self.assertRaises(TypeError):
            self.queue.put(cast(Any, "not a task"))

        for invalid in (-1, True, 1.5):
            with self.subTest(priority=invalid):
                task = Task(
                    id=100,
                    action="invalid",
                    priority=cast(Any, invalid),
                )

                with self.assertRaises(ValueError):
                    self.queue.put(task)

        self.assertEqual(len(self.queue), 0)

    def test_concurrent_insertion(self):
        """Concurrent insertions preserve every unique Task."""
        tasks = [
            Task(identity, "process", priority=identity % 4)
            for identity in range(100)
        ]

        with ThreadPoolExecutor(max_workers=8) as executor:
            list(executor.map(self.queue.put, tasks))

        self.assertEqual(len(self.queue), 100)

        retrieved = [
            self.queue.get()
            for _ in range(100)
        ]

        self.assertEqual(
            {task.id for task in retrieved},
            set(range(100)),
        )

        priorities = [task.priority for task in retrieved]

        self.assertEqual(priorities, sorted(priorities))
        self.assertFalse(self.queue)


if __name__ == "__main__":
    unittest.main()
