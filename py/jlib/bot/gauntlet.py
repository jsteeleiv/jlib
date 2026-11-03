
"""Onboarding infrastructure for the JLIB bot ecosystem."""

from typing import Any

from .worker import Worker


class Gauntlet:
    """
    Prepare newly forged Workers for ecosystem participation.

    The Gauntlet coordinates one-time onboarding, including the
    initial imprint, foundational knowledge, experience,
    and disposition.

    It does not construct, register, or activate Workers.
    """

    __slots__ = ("_context",)

    def __init__(self, context: Any) -> None:
        """Establish the onboarding environment."""
        if context is None:
            raise ValueError("Gauntlet requires an onboarding context.")

        self._context = context

    def onboard(self, worker: Worker) -> Worker:
        """
        Initialize a newly constructed Worker.

        Return the initialized Worker without registering it.
        """
        if not isinstance(worker, Worker):
            raise TypeError("Gauntlet requires a Worker instance.")

        if worker.initialized:
            raise RuntimeError(
                f"Worker {worker.id} has already completed onboarding."
            )

        worker.initialize(self._context)

        return worker
