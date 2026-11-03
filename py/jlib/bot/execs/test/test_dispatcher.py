
"""Unit tests for JLIB's task Dispatcher."""

import unittest

from concurrent.futures import ThreadPoolExecutor
from threading import Barrier
from unittest.mock import Mock

from py.jlib.bot.core.arbiter import Arbiter
from py.jlib.bot.execs.dispatcher import Dispatcher
from py.jlib.bot.work.queue import TaskQueue
from py.jlib.bot.core.registry import Registry
from py.jlib.bot.work.task import Task
from jlib.bot.worker import Worker


class TestWorker(Worker):
    """Concrete Worker used to test real task execution."""

    __slots__ = ("calls", "failure")

    capabilities = frozenset({
        "inspect",
        "process",
        "fail",
        "high",
        "low",
        "shared",
    })

    def __init__(self, id: int):
        super().__init__(id)
        self.calls: list[Task] = []
        self.failure = False

    def work(self, task: Task) -> str:
        """Record execution and optionally simulate failure."""
        self.calls.append(task)

        if self.failure:
            raise RuntimeError("Simulated execution failure.")

        return "completed"


class TestDispatcher(unittest.TestCase):
    """Tests for coordinated Task assignment and execution."""

    def setUp(self):
        """Create shared infrastructure and an initialized Worker."""
        self.registry = Registry()
        self.queue = TaskQueue()
        self.arbiter = Arbiter()

        self.dispatcher = Dispatcher(
            100,
            self.registry,
            self.queue,
            self.arbiter,
        )

        self.worker = TestWorker(1)

        # Simulate the Gauntlet's onboarding context.
        context = Mock()

        context.capture_imprint.return_value = {
            "environment": "test",
        }
        context.consult.return_value = {}
        context.create_state.return_value = {}
        context.create_experience.return_value = []
        context.create_disposition.return_value = {}

        self.worker.initialize(context)
        self.registry.register(self.worker)

    def test_successful_dispatch(self):
        """Dispatcher assigns a queued Task to a Worker."""
        task = Task(1, "inspect")
        self.queue.put(task)

        result = self.dispatcher.dispatch()

        self.assertEqual(result, "completed")
        self.assertEqual(self.worker.calls, [task])
        self.assertFalse(self.queue)
        self.assertEqual(self.registry.busy(), {})

    def test_empty_queue(self):
        """Dispatcher returns None when no Tasks are queued."""
        self.assertIsNone(self.dispatcher.dispatch())
        self.assertEqual(self.registry.busy(), {})

    def test_unavailable_worker(self):
        """Tasks remain queued when all Workers are reserved."""
        task = Task(2, "inspect")
        self.queue.put(task)

        self.registry.reserve(self.worker.id, 999)

        result = self.dispatcher.dispatch()

        self.assertIsNone(result)
        self.assertIs(self.queue.peek(), task)
        self.assertEqual(len(self.queue), 1)

        self.registry.release(self.worker.id)

    def test_reservation_cleanup(self):
        """Successful execution releases the Worker reservation."""
        self.queue.put(Task(3, "process"))

        self.dispatcher.dispatch()

        self.assertEqual(self.registry.busy(), {})
        self.assertEqual(len(self.registry.available()), 1)

    def test_execution_failure(self):
        """Worker reservations are released when execution fails."""
        task = Task(4, "fail")
        self.queue.put(task)

        self.worker.failure = True

        with self.assertRaises(RuntimeError):
            self.dispatcher.dispatch()

        self.assertEqual(self.registry.busy(), {})
        self.assertFalse(self.queue)
        self.assertEqual(self.worker.calls, [task])

    def test_priority_order(self):
        """Higher-priority Tasks are dispatched first."""
        low = Task(5, "low", priority=5)
        high = Task(6, "high", priority=0)

        self.queue.put(low)
        self.queue.put(high)

        self.dispatcher.dispatch()

        self.assertEqual(self.worker.calls, [high])
        self.assertIs(self.queue.peek(), low)

    def test_concurrent_dispatchers(self):
        """Two Dispatchers cannot claim the same Task."""
        second = Dispatcher(
            101,
            self.registry,
            self.queue,
            self.arbiter,
        )

        task = Task(7, "shared")
        self.queue.put(task)

        barrier = Barrier(2)

        def attempt(dispatcher: Dispatcher) -> str | None:
            barrier.wait(timeout=5)
            return dispatcher.dispatch()

        with ThreadPoolExecutor(max_workers=2) as executor:
            results = list(
                executor.map(
                    attempt,
                    (self.dispatcher, second),
                )
            )

        self.assertEqual(results.count("completed"), 1)
        self.assertEqual(results.count(None), 1)
        self.assertEqual(self.worker.calls, [task])
        self.assertFalse(self.queue)
        self.assertEqual(self.registry.busy(), {})

    def test_capability_matching(self):
        """Dispatcher skips Workers lacking the required capability."""

        class OtherWorker(TestWorker):
            capabilities = frozenset({"scrape"})

        scraper = OtherWorker(2)

        context = Mock()
        context.capture_imprint.return_value = {}
        context.consult.return_value = {}
        context.create_state.return_value = {}
        context.create_experience.return_value = []
        context.create_disposition.return_value = {}

        scraper.initialize(context)
        self.registry.register(scraper)

        task = Task(8, "scrape")
        self.queue.put(task)

        result = self.dispatcher.dispatch()

        self.assertEqual(result, "completed")
        self.assertEqual(scraper.calls, [task])
        self.assertEqual(self.worker.calls, [])
        self.assertFalse(self.queue)


if __name__ == "__main__":
    unittest.main()
