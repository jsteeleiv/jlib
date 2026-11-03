
"""Shared synchronization infrastructure for the JLIB bot ecosystem."""

from contextlib import contextmanager
from threading import RLock
from collections.abc import Generator


class Arbiter:
    """
    Coordinate critical operations across ecosystem components.

    The Arbiter provides shared synchronization without
    deciding what operations should accomplish.

    All participants must use the same Arbiter instance
    when coordinating access to shared resources.
    """

    __slots__ = ("_lock",)

    def __init__(self) -> None:
        """Initialize the shared reentrant lock."""
        self._lock = RLock()

    @contextmanager
    def critical(self) -> Generator[None, None, None]:
        """
        Execute a critical section under shared synchronization.

        The lock is released automatically when the block exits,
        including when an exception occurs.
        """
        with self._lock:
            yield
