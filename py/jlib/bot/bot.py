from abc import ABC
from datetime import datetime, timezone
from zoneinfo import ZoneInfo


def format_time(dt: datetime, tz: str = "UTC") -> str:
    """Format a UTC timestamp in the requested timezone."""
    zones = {
        "UTC": "UTC",
        "EST": "America/New_York",
        "CST": "America/Chicago",
        "MST": "America/Denver",
        "PST": "America/Los_Angeles",
    }

    zone = ZoneInfo(zones[tz.upper()])
    return dt.astimezone(zone).strftime("%Y-%m-%d %I:%M:%S %p %Z")

class Bot(ABC):    #abstract base class
    """Abstract ancestor of all JLIB bots."""

    # Defines permitted instance attributes 
    # Python creates slot descriptors for each name, providing per-instance
    # storage without a normal __dict__, reducing memory overhead and
    # preventing arbitrary new attributes (unless a subclass allows them).
    __slots__ = (
        "_id",
        "_born",
        "_started",
        "_stopped",
        "_died",
    )

    def __new__(cls, *args, **kwargs):
        """Prevent direct instantiation of the abstract Bot class."""
        if cls is Bot:
            raise TypeError("Bot cannot be instantiated directly.")

        return super().__new__(cls)

    def __init__(self, id: int):
        self._id = id
        self._born = datetime.now(timezone.utc)
        self._started = None
        self._stopped = None
        self._died = None

    def __repr__(self) -> str:
        """Return a developer-friendly representation of the bot."""
        return (
            f"{type(self).__name__}("
            f"id={self._id}, "
            f"born={self._born!r})"
        )

    @property
    def born(self) -> datetime:
        """Return the bot's creation timestamp in UTC."""
        return self._born
    
    @property
    def id(self) -> int:
        """Return the bot's assigned identity."""
        return self._id

    @property
    def started(self) -> datetime | None:
        """Return the most recent activation timestamp."""
        return self._started

    @property
    def stopped(self) -> datetime | None:
        """Return the most recent deactivation timestamp."""
        return self._stopped

    @property
    def died(self) -> datetime | None:
        """Return the permanent termination timestamp."""
        return self._died
