
"""Type-aware JSON serialization for the JLIB bot ecosystem."""

import json

from datetime import datetime
from functools import singledispatch
from typing import Any
from collections.abc import Mapping
from .state import Freezer, Imprint, thaw


# ---------------------------------------------------------
# Encoding
# ---------------------------------------------------------

@singledispatch
def encode(value: Any) -> Any:
    """Encode supported Python values into JSON-compatible data."""
    raise TypeError(
        f"Unsupported type for encoding: {type(value).__name__}"
    )


@encode.register(type(None))
def _(value: None) -> None:
    return None


@encode.register
def _(value: bool) -> bool:
    return value


@encode.register
def _(value: int) -> int:
    return value


@encode.register
def _(value: float) -> float:
    return value


@encode.register
def _(value: str) -> str:
    return value


@encode.register
def _(value: list) -> dict:
    return {
        "type": "list",
        "value": [encode(item) for item in value],
    }


@encode.register
def _(value: tuple) -> dict:
    return {
        "type": "tuple",
        "value": [encode(item) for item in value],
    }


@encode.register
def _(value: set) -> dict:
    return {
        "type": "set",
        "value": [encode(item) for item in value],
    }


@encode.register
def _(value: frozenset) -> dict:
    return {
        "type": "frozenset",
        "value": [encode(item) for item in value],
    }


# @encode.register
# def _(value: dict) -> dict:
#     return {
#         "type": "dict",
#         "value": [
#             [encode(key), encode(item)]
#             for key, item in value.items()
#         ],
#     }

@encode.register
def _(value: Mapping) -> dict:
    return {
        "type": "dict",
        "value": [
            [encode(key), encode(item)]
            for key, item in value.items()
        ],
    }


@encode.register
def _(value: datetime) -> dict:
    return {
        "type": "datetime",
        "value": value.isoformat(),
    }


@encode.register
def _(value: bytes) -> dict:
    return {
        "type": "bytes",
        "value": list(value),
    }


@encode.register
def _(value: Freezer) -> dict:
    return {
        "type": "freezer",
        "value": {
            "original_type": value.type,
            "data": encode(value.value),
        },
    }


@encode.register
def _(value: Imprint) -> dict:
    return {
        "type": "imprint",
        "value": {
            "born": encode(value.born),
            "environment": value.environment,
            "capabilities": encode(value.capabilities),
            "metadata": encode(value.metadata),
        },
    }


# ---------------------------------------------------------
# Decoding
# ---------------------------------------------------------

def decode(data: Any) -> Any:
    """Reconstruct supported Python values from encoded data."""

    if data is None or type(data) in (bool, int, float, str):
        return data

    if not isinstance(data, dict):
        raise TypeError("Encoded data must be a typed dictionary.")

    kind = data["type"]
    value = data["value"]

    match kind:
        case "list":
            return [decode(item) for item in value]

        case "tuple":
            return tuple(decode(item) for item in value)

        case "set":
            return {decode(item) for item in value}

        case "frozenset":
            return frozenset(decode(item) for item in value)

        case "dict":
            return {
                decode(key): decode(item)
                for key, item in value
            }

        case "datetime":
            return datetime.fromisoformat(value)

        case "bytes":
            return bytes(value)

        case "freezer":
            return Freezer(
                type=value["original_type"],
                value=decode(value["data"]),
            )

        case "imprint":
            return Imprint(
                born=decode(value["born"]),
                environment=value["environment"],
                capabilities=decode(value["capabilities"]),
                metadata=decode(value["metadata"]),
            )

        case _:
            raise TypeError(f"Unsupported encoded type: {kind}")


# ---------------------------------------------------------
# JSON Conversion
# ---------------------------------------------------------

def dumps(value: Any, **kwargs: Any) -> str:
    """Serialize a supported Python value into a JSON string."""
    return json.dumps(encode(value), **kwargs)


def loads(data: str) -> Any:
    """Deserialize a JSON string into its original Python value."""
    return decode(json.loads(data))
