
"""State models for the JLIB bot ecosystem."""

from collections.abc import Mapping
from dataclasses import dataclass, field
from datetime import datetime
from types import MappingProxyType
from typing import Any


@dataclass(frozen=True, slots=True)
class Freezer:
    """Preserve a frozen value and its original Python type."""

    type: str
    value: tuple[Any, ...] | frozenset[Any]

def freeze(value: Any) -> Any:
    """Recursively freeze values, preserving changed collection types."""

    if isinstance(value, Freezer):
        return Freezer(
            type=value.type,
            value=freeze(value.value),
        )

    if value is None or type(value) in (str, int, float, bool, bytes):
        return value

    if isinstance(value, datetime):
        return value

    if isinstance(value, Mapping):
        return MappingProxyType({
            freeze(key): freeze(item)
            for key, item in value.items()
        })

    if isinstance(value, list):
        return Freezer(
            type="list",
            value=tuple(freeze(item) for item in value),
        )

    if isinstance(value, tuple):
        return tuple(freeze(item) for item in value)

    if isinstance(value, set):
        return Freezer(
            type="set",
            value=frozenset(freeze(item) for item in value),
        )

    if isinstance(value, frozenset):
        return frozenset(freeze(item) for item in value)

    raise TypeError(
        f"Cannot freeze unsupported type: {type(value).__name__}"
    )


def thaw(value: Any) -> Any:
    """Recursively restore frozen values to their original types."""

    if isinstance(value, Freezer):
        match value.type:
            case "list":
                return [thaw(item) for item in value.value]

            case "set":
                return {thaw(item) for item in value.value}

            case _:
                raise TypeError(
                    f"Unknown frozen type: {value.type}"
                )

    if isinstance(value, Mapping):
        return {
            thaw(key): thaw(item)
            for key, item in value.items()
        }

    if isinstance(value, tuple):
        return tuple(thaw(item) for item in value)

    if isinstance(value, frozenset):
        return frozenset(thaw(item) for item in value)

    return value


@dataclass(frozen=True, slots=True)
class Imprint:
    """Immutable snapshot of a Worker's original conditions."""

    born: datetime
    environment: str
    capabilities: tuple[str, ...] = ()
    metadata: Mapping[str, Any] = field(default_factory=dict)

    def __post_init__(self) -> None:
        """
        Normalize and freeze values after initialization.

        object.__setattr__ is used because frozen dataclasses
        prohibit ordinary attribute assignment.
        """
        if self.born.tzinfo is None or self.born.utcoffset() is None:
            raise ValueError("Imprint birth timestamp must be timezone-aware.")

        object.__setattr__(
            self,
            "capabilities",
            tuple(self.capabilities),
        )

        object.__setattr__(
            self,
            "metadata",
            freeze(self.metadata),
        )
