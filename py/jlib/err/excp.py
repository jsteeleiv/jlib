
"""
jlib.err.excp

Custom exception classes for the jlib package.

Defines reusable exception types without performing I/O,
configuring logging, or importing other jlib modules.
"""


class JLibError(Exception):
    """Base exception for jlib-specific failures."""


class DependencyError(JLibError, ImportError):
    """Raised when a required optional dependency is unavailable."""


class ConfigurationError(JLibError, ValueError):
    """Raised when jlib-specific configuration is invalid."""


class OperationError(JLibError):
    """Raised when a jlib operation fails for a library-specific reason."""
