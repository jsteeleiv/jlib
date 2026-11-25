from __future__ import annotations
from typing import Any


class Snowball:
    """
    A mutable, cumulative state structure.

    Each roll incorporates a new observation into the
    existing object while preserving selected history.
    """

    def __init__(self, initial: Any = None) -> None:
        self._present = initial
        self._past: list[Any] = []
        self._iteration = 0

    @property
    def present(self) -> Any:
        """Return the current accumulated state."""
        return self._present

    @property
    def past(self) -> tuple[Any, ...]:
        """Return a read-only sequence of retained history."""
        return tuple(self._past)

    @property
    def iteration(self) -> int:
        """Return the number of completed rolls."""
        return self._iteration


    def roll(self, observation: Any, transform) -> None:
        """
        Incorporate an observation into the current state.

        transform receives the current state and observation,
        and returns the updated state.
        """
        updated = transform(self._present, observation)

        self._present = updated
        self._iteration += 1
