
"""
jlib.err

Shared exception types, dependency utilities,
and diagnostic formatting.
"""

from .excp import (
    JLibError,
    DependencyError,
    ConfigurationError,
    OperationError,
)

from .deps import (
    require,
    available,
    check,
)

from .fmt import (
    format_error,
    error_details,
    format_traceback,
)

__all__ = [
    "JLibError",
    "DependencyError",
    "ConfigurationError",
    "OperationError",
    "require",
    "available",
    "check",
    "format_error",
    "error_details",
    "format_traceback",
]

