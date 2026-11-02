from abc import ABC
from datetime import datetime, timezone
from zoneinfo import ZoneInfo


class Bot(ABC):    #abstract base class
    """Abstract ancestor of all JLIB bots."""

    # Defines permitted instance attributes using a tuple of names.
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

    def __init__(self, id: int):
        self._id = id
        self._born = datetime.now(timezone.utc)
        self._started = None
        self._stopped = None
        self._died = None

    @property
    def born(self) -> datetime:
        """Return the bot's creation timestamp in UTC."""
        return self._born
    

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