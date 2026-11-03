
"""Task dispatching for the JLIB bot ecosystem."""

from typing import Any

from ..core.arbiter import Arbiter
from ..executive import Executive
from ..work.queue import TaskQueue
from ..core.registry import Registry
from ..work.task import Task


class Dispatcher(Executive):
    """
    Match queued Tasks with available Workers.

    The Arbiter coordinates assignment across shared resources.
    The Registry manages Worker reservations.
    The TaskQueue maintains pending work.

    Task execution occurs outside the Arbiter's critical section.
    """

    __slots__ = (
        "_registry",
        "_queue",
        "_arbiter",
    )

    def __init__(
        self,
        id: int,
        registry: Registry,
        queue: TaskQueue,
        arbiter: Arbiter,
    ) -> None:
        """Initialize the Dispatcher and its shared resources."""
        super().__init__(id)

        if not isinstance(registry, Registry):
            raise TypeError("Dispatcher requires a Registry.")

        if not isinstance(queue, TaskQueue):
            raise TypeError("Dispatcher requires a TaskQueue.")

        if not isinstance(arbiter, Arbiter):
            raise TypeError("Dispatcher requires an Arbiter.")

        self._registry = registry
        self._queue = queue
        self._arbiter = arbiter

    # ---------------------------------------------------------
    # Task Execution
    # ---------------------------------------------------------

    def work(self, task: Task) -> Any:
        """Perform a dispatch operation."""
        return self.dispatch()

    def dispatch(self) -> Any:
        """
        Assign and execute the highest-priority queued Task.

        Return None when no Task or Worker is available.

        A Task remains queued until a Worker is reserved.
        """
        with self._arbiter.critical():
            task = self._queue.peek()

            if task is None:
                return None

            selected = None

            for worker in self._registry.available():
                if worker is self:
                    continue

                capabilities = getattr(worker, "capabilities", frozenset())

                if task.action not in capabilities:
                    continue

                if self._registry.reserve(worker.id, task.id):
                    selected = worker
                    break

            if selected is None:
                return None

            try:
                # Claim the Task only after reserving its Worker.
                assigned = self._queue.get()

            except Exception:
                # Undo the reservation if claiming fails.
                self._registry.release(selected.id)
                raise

        # The Arbiter is released before execution begins.
        try:
            return selected.execute(assigned)

        finally:
            self._registry.release(selected.id)
