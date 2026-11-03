
"""Task definitions for the JLIB bot ecosystem."""

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True, slots=True)
class Task:
    """Describe an independent unit of work."""

    id: int
    action: str
    payload: Any = None
    priority: int = 0
