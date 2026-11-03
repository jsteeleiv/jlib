
"""Unit tests for JLIB's Task data model."""

import unittest

from dataclasses import FrozenInstanceError

from py.jlib.bot.work.task import Task


class TestTask(unittest.TestCase):
    """Tests for independent task definitions."""

    def test_creation(self):
        """Tasks retain their assigned instructions."""
        task = Task(
            id=101,
            action="inspect",
            payload={"path": "example.txt"},
            priority=5,
        )

        self.assertEqual(task.id, 101)
        self.assertEqual(task.action, "inspect")
        self.assertEqual(task.payload, {"path": "example.txt"})
        self.assertEqual(task.priority, 5)

    def test_defaults(self):
        """Optional fields receive their expected defaults."""
        task = Task(id=102, action="process")

        self.assertIsNone(task.payload)
        self.assertEqual(task.priority, 0)

    def test_immutability(self):
        """Task fields cannot be reassigned after construction."""
        task = Task(id=103, action="inspect")

        with self.assertRaises(FrozenInstanceError):
            setattr(task, "action", "delete")

    def test_independence(self):
        """Tasks contain no knowledge of ecosystem hierarchy."""
        task = Task(id=104, action="inspect")

        for attribute in (
            "job_id",
            "supervisor",
            "dispatcher",
            "worker",
        ):
            with self.subTest(attribute=attribute):
                self.assertFalse(hasattr(task, attribute))


if __name__ == "__main__":
    unittest.main()
