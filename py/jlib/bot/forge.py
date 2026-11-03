
"""Construction infrastructure for the JLIB bot ecosystem."""

from itertools import count
from typing import TypeVar

from .bot import Bot


TBot = TypeVar("TBot", bound=Bot)


class Forge:
    """Construct JLIB bots with unique runtime identities."""

    __slots__ = ("_ids",)

    def __init__(self, start: int = 1) -> None:
        """Initialize the identity sequence."""
        if isinstance(start, bool) or not isinstance(start, int) or start < 1:
            raise ValueError("Starting ID must be a positive integer.")

        self._ids = count(start)

    def build(self, bot_type: type[TBot]) -> TBot:
        """
        Construct a concrete Bot subclass.

        The Forge assigns an identity but does not register,
        activate, or authorize the resulting bot.
        """
        if not isinstance(bot_type, type):
            raise TypeError("Expected a Bot class.")

        if not issubclass(bot_type, Bot):
            raise TypeError("Class must inherit from Bot.")

        identity = next(self._ids)

        return bot_type(identity)
