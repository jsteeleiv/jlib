
"""Thread-safe priority queue for JLIB bot tasks."""

from heapq import heappush, heappop
from itertools import count
from threading import Lock

from .task import Task


class TaskQueue:
    """
    Maintain pending Tasks in priority order.

    Lower priority numbers are processed first.
    Equal priorities follow FIFO ordering.

    The Queue does not execute or assign Tasks.
    """

    __slots__ = (
        "_heap",
        "_ids",
        "_sequence",
        "_lock",
    )

    def __init__(self) -> None:
        """Initialize an empty Task Queue."""
        self._heap: list[tuple[int, int, Task]] = []
        self._ids: set[int] = set()
        self._sequence = count()
        self._lock = Lock()

    # ---------------------------------------------------------
    # Queue Operations
    # ---------------------------------------------------------

    def put(self, task: Task) -> None:
        """Add a Task to the priority queue."""
        if not isinstance(task, Task):
            raise TypeError("TaskQueue requires a Task instance.")

        if isinstance(task.priority, bool) or not isinstance(
            task.priority, int
        ) or task.priority < 0:
            raise ValueError(
                "Task priority must be a non-negative integer."
            )

        with self._lock:
            if task.id in self._ids:
                raise ValueError(
                    f"Task {task.id} is already queued."
                )

            heappush(
                self._heap,
                (task.priority, next(self._sequence), task),
            )

            self._ids.add(task.id)

    def get(self) -> Task:
        """Remove and return the highest-priority Task."""
        with self._lock:
            if not self._heap:
                raise IndexError("Task Queue is empty.")

            _, _, task = heappop(self._heap)
            self._ids.remove(task.id)

            return task

    def peek(self) -> Task | None:
        """Return the next Task without removing it."""
        with self._lock:
            if not self._heap:
                return None

            return self._heap[0][2]

    # ---------------------------------------------------------
    # Inspection
    # ---------------------------------------------------------

    def __len__(self) -> int:
        """Return the number of queued Tasks."""
        with self._lock:
            return len(self._heap)

    def __bool__(self) -> bool:
        """Return True when the Queue contains Tasks."""
        return len(self) > 0

    def contains(self, task_id: int) -> bool:
        """Check whether a Task ID is currently queued."""
        with self._lock:
            return task_id in self._ids

    def snapshot(self) -> tuple[Task, ...]:
        """Return queued Tasks in retrieval order."""
        with self._lock:
            ordered = sorted(self._heap)

            return tuple(
                task for _, _, task in ordered
            )
