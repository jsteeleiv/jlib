
"""Immutable state snapshots for the JLIB bot ecosystem."""

from dataclasses import dataclass
from datetime import datetime, UTC
from typing import Any

from .state import freeze, thaw


@dataclass(frozen=True, slots=True)
class Snapshot:
    """Immutable record of state captured at a point in time."""

    created: datetime
    data: Any

    def __post_init__(self) -> None:
        """Ensure snapshot contents are frozen."""
        if self.created.tzinfo is None or self.created.utcoffset() is None:
            raise ValueError("Snapshot timestamp must be timezone-aware.")

        object.__setattr__(
            self,
            "data",
            freeze(self.data),
        )

    def restore(self) -> Any:
        """Return an independent, mutable reconstruction of the data."""
        return thaw(self.data)


def capture(data: Any) -> Snapshot:
    """Capture an immutable snapshot of supported state."""
    return Snapshot(
        created=datetime.now(UTC),
        data=data,
    )
