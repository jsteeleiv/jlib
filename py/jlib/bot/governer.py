
"""Top-level governance for the JLIB bot ecosystem."""

from typing import TypeVar

from .bot import Bot
from .worker import Worker
from .forge import Forge
from .gauntlet import Gauntlet


TBot = TypeVar("TBot", bound=Bot)


class Governer:
    """
    Govern the lifecycle and construction of a JLIB bot ecosystem.

    The Governer coordinates one Forge and one Gauntlet, maintaining
    centralized authority over ecosystem creation and operation.

    It does not directly execute tasks or manage jobs.
    """

    __slots__ = (
        "_active",
        "_forge",
        "_gauntlet",
        "_registry",
        "_board",
        "_corpus",
    )

    def __init__(
        self,
        forge: Forge,
        gauntlet: Gauntlet,
    ) -> None:
        """Establish the ecosystem's construction infrastructure."""
        if not isinstance(forge, Forge):
            raise TypeError("Governer requires a Forge instance.")

        if not isinstance(gauntlet, Gauntlet):
            raise TypeError("Governer requires a Gauntlet instance.")

        self._active = False
        self._forge = forge
        self._gauntlet = gauntlet

        # Reserved for future ecosystem components.
        self._registry = None
        self._board = None
        self._corpus = None

    # ---------------------------------------------------------
    # Properties
    # ---------------------------------------------------------

    @property
    def active(self) -> bool:
        """Whether the ecosystem is operational."""
        return self._active

    @property
    def forge(self) -> Forge:
        """Return the ecosystem's Forge."""
        return self._forge

    @property
    def gauntlet(self) -> Gauntlet:
        """Return the ecosystem's Gauntlet."""
        return self._gauntlet

    # ---------------------------------------------------------
    # Lifecycle
    # ---------------------------------------------------------

    def start(self) -> None:
        """Activate the ecosystem."""
        if self._active:
            raise RuntimeError("Governer is already active.")

        self._active = True

    def stop(self) -> None:
        """Deactivate the ecosystem."""
        if not self._active:
            raise RuntimeError("Governer is not active.")

        self._active = False

    # ---------------------------------------------------------
    # Construction
    # ---------------------------------------------------------

    def create(self, bot_type: type[TBot]) -> TBot:
        """
        Construct and prepare a new ecosystem entity.

        The Forge constructs the Bot.
        The Well onboards Workers.

        Registration will be introduced when the Registry exists.
        """
        if not self._active:
            raise RuntimeError(
                "Cannot create bots while the Governer is inactive."
            )

        bot = self._forge.build(bot_type)

        if isinstance(bot, Worker):
            self._gauntlet.onboard(bot)

        return bot
