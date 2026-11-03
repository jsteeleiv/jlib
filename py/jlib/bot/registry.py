
"""Worker registration, discovery, and availability for JLIB."""

from threading import Lock

from .worker import Worker


class Registry:
    """
    Maintain the ecosystem's registered workforce.

    The Registry tracks Worker identities and availability.
    Dispatchers use atomic reservations to prevent conflicting
    assignments.
    """

    __slots__ = (
        "_workers",
        "_busy",
        "_lock",
    )

    def __init__(self) -> None:
        """Initialize an empty Registry."""
        self._workers: dict[int, Worker] = {}
        self._busy: dict[int, int] = {}
        self._lock = Lock()

    # ---------------------------------------------------------
    # Registration
    # ---------------------------------------------------------

    def register(self, worker: Worker) -> None:
        """Register an initialized Worker."""
        if not isinstance(worker, Worker):
            raise TypeError("Registry requires a Worker instance.")

        if not worker.initialized:
            raise RuntimeError(
                "Cannot register a Worker before onboarding."
            )

        with self._lock:
            if worker.id in self._workers:
                raise ValueError(
                    f"Worker ID {worker.id} is already registered."
                )

            self._workers[worker.id] = worker

    def unregister(self, worker_id: int) -> Worker:
        """Remove and return an idle registered Worker."""
        with self._lock:
            if worker_id not in self._workers:
                raise KeyError(worker_id)

            if worker_id in self._busy:
                raise RuntimeError(
                    "Cannot unregister a busy Worker."
                )

            return self._workers.pop(worker_id)

    # ---------------------------------------------------------
    # Discovery
    # ---------------------------------------------------------

    def get(self, worker_id: int) -> Worker | None:
        """Return a registered Worker or None."""
        with self._lock:
            return self._workers.get(worker_id)

    def available(self) -> tuple[Worker, ...]:
        """Return a snapshot of currently available Workers."""
        with self._lock:
            return tuple(
                worker
                for identity, worker in self._workers.items()
                if identity not in self._busy
            )

    def busy(self) -> dict[int, int]:
        """Return a copy of active Worker-to-Task reservations."""
        with self._lock:
            return self._busy.copy()

    # ---------------------------------------------------------
    # Atomic Reservations
    # ---------------------------------------------------------

    def reserve(self, worker_id: int, task_id: int) -> bool:
        """
        Atomically reserve a Worker for one Task.

        Return True when reserved, False when already busy.
        """
        with self._lock:
            if worker_id not in self._workers:
                raise KeyError(worker_id)

            if worker_id in self._busy:
                return False

            self._busy[worker_id] = task_id
            return True

    def release(self, worker_id: int) -> None:
        """Release a Worker's current reservation."""
        with self._lock:
            if worker_id not in self._busy:
                raise KeyError(
                    f"Worker {worker_id} has no active reservation."
                )

            del self._busy[worker_id]
