
import unittest
from datetime import datetime, UTC
from unittest.mock import Mock

from jlib.bot.bot import Bot
from jlib.bot.worker import Worker


class TestWorker(unittest.TestCase):
    """Tests for the foundational Worker class."""

    def setUp(self):
        """Create a fresh Worker and simulated ecosystem context."""
        self.worker = Worker(1)

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
            "exploration": 0.5,
        }

    # ---------------------------------------------------------
    # Inheritance and Construction
    # ---------------------------------------------------------

    def test_worker_inherits_bot(self):
        """Worker must inherit from Bot."""
        self.assertIsInstance(self.worker, Bot)
        self.assertIsInstance(self.worker, Worker)

    def test_worker_identity(self):
        """Worker inherits its identity from Bot."""
        self.assertEqual(self.worker.id, 1)

    def test_worker_birth_timestamp(self):
        """Worker receives a UTC-aware birth timestamp."""
        self.assertIsInstance(self.worker.born, datetime)
        self.assertEqual(self.worker.born.tzinfo, UTC)

    def test_worker_initial_values(self):
        """Worker starts without completed onboarding."""
        self.assertFalse(self.worker.initialized)
        self.assertIsNone(self.worker.imprint)
        self.assertIsNone(self.worker.state)
        self.assertIsNone(self.worker.disposition)
        self.assertIsNone(self.worker.experience)

    # ---------------------------------------------------------
    # Initialization
    # ---------------------------------------------------------

    def test_initialize(self):
        """Initialization establishes the Worker's initial state."""
        self.worker.initialize(self.context)

        self.assertTrue(self.worker.initialized)
        self.assertEqual(
            self.worker.imprint["environment"], "test"
        )
        self.assertEqual(
            self.worker.state["status"], "ready"
        )
        self.assertEqual(self.worker.experience, [])
        self.assertEqual(
            self.worker.disposition["caution"], 0.5
        )

    def test_initialize_only_once(self):
        """A Worker cannot complete onboarding twice."""
        self.worker.initialize(self.context)

        with self.assertRaises(RuntimeError):
            self.worker.initialize(self.context)

    def test_initialize_rejects_none(self):
        """Initialization requires a valid context."""
        with self.assertRaises(ValueError):
            self.worker.initialize(None)

    def test_initialize_calls_context(self):
        """Onboarding invokes the required context operations."""
        self.worker.initialize(self.context)

        self.context.capture_imprint.assert_called_once_with(
            self.worker
        )
        self.context.consult.assert_called_once_with(self.worker)

        self.context.create_state.assert_called_once_with(
            self.worker,
            self.worker.imprint,
        )

        self.context.create_experience.assert_called_once()
        self.context.create_disposition.assert_called_once()

    def test_imprint_captured_before_consultation(self):
        """The imprint must precede Corpus consultation."""
        calls = []

        self.context.capture_imprint.side_effect = (
            lambda worker: (
                calls.append("imprint") or {"environment": "test"}
            )
        )

        self.context.consult.side_effect = (
            lambda worker: (
                calls.append("consult") or {}
            )
        )

        self.worker.initialize(self.context)

        self.assertEqual(calls[:2], ["imprint", "consult"])

    def test_failed_initialization(self):
        """Failed onboarding must not mark the Worker initialized."""
        self.context.create_disposition.side_effect = RuntimeError(
            "Disposition initialization failed."
        )

        with self.assertRaises(RuntimeError):
            self.worker.initialize(self.context)

        self.assertFalse(self.worker.initialized)
        self.assertIsNone(self.worker.imprint)
        self.assertIsNone(self.worker.state)
        self.assertIsNone(self.worker.experience)
        self.assertIsNone(self.worker.disposition)

    # ---------------------------------------------------------
    # Ecosystem Communication
    # ---------------------------------------------------------

    def test_request(self):
        """Workers can request authorized ecosystem operations."""
        self.worker.initialize(self.context)

        self.context.request.return_value = True

        result = self.worker.request(
            "corpus.read",
            scope="shared",
        )

        self.assertTrue(result)

        self.context.request.assert_called_once_with(
            self.worker,
            "corpus.read",
            scope="shared",
        )

    def test_consult(self):
        """Workers can consult the Corpus after initialization."""
        self.worker.initialize(self.context)

        self.context.consult.reset_mock()
        self.context.consult.return_value = {
            "result": "shared knowledge"
        }

        result = self.worker.consult("filesystem")

        self.assertEqual(
            result["result"], "shared knowledge"
        )

        self.context.consult.assert_called_once_with(
            self.worker,
            "filesystem",
        )

    def test_contribute(self):
        """Workers can submit knowledge to the Corpus."""
        self.worker.initialize(self.context)

        knowledge = {
            "topic": "filesystem",
            "observation": "New discovery",
        }

        self.context.contribute.return_value = True

        result = self.worker.contribute(knowledge)

        self.assertTrue(result)

        self.context.contribute.assert_called_once_with(
            self.worker,
            knowledge,
        )

    def test_communication_before_initialization(self):
        """Uninitialized Workers cannot access ecosystem services."""
        with self.assertRaises(RuntimeError):
            self.worker.request("corpus.read")

        with self.assertRaises(RuntimeError):
            self.worker.consult("test")

        with self.assertRaises(RuntimeError):
            self.worker.contribute({"test": True})

    # ---------------------------------------------------------
    # Task Execution
    # ---------------------------------------------------------

    def test_execute_before_initialization(self):
        """Uninitialized Workers cannot execute tasks."""
        with self.assertRaises(RuntimeError):
            self.worker.execute("example")

    def test_generic_worker_has_no_specialized_work(self):
        """Generic Workers reject unsupported task execution."""
        self.worker.initialize(self.context)

        with self.assertRaises(NotImplementedError):
            self.worker.execute("example")

    def test_specialized_worker_execution(self):
        """Specialized Workers override work(), not execute()."""

        class SpecializedWorker(Worker):
            def work(self, task):
                return f"Completed: {task}"

        worker = SpecializedWorker(2)
        worker.initialize(self.context)

        result = worker.execute("example")

        self.assertEqual(result, "Completed: example")

    def test_execute_preserves_task_identity(self):
        """The original task object reaches work() unchanged."""

        class EchoWorker(Worker):
            def work(self, task):
                return task

        worker = EchoWorker(3)
        worker.initialize(self.context)

        task = {"action": "inspect"}
        result = worker.execute(task)

        self.assertIs(result, task)

    # ---------------------------------------------------------
    # Representation
    # ---------------------------------------------------------

    def test_worker_representation(self):
        """Workers inherit the Bot debugging representation."""
        representation = repr(self.worker)

        self.assertIn("Worker", representation)
        self.assertIn("id=1", representation)
        self.assertIn("born=", representation)


if __name__ == "__main__":
    unittest.main()
