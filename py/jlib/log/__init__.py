
"""
jlib.log

Modular logging utilities for the jlib package.

Supports logger configuration, formatting, console and file
output, log rotation, and managed resource cleanup.
"""

from .core import getlogger, configure, shutdown
from .fmt import getformatter
from .handlers import consolehandler, filehandler
from .rotate import sizehandler, timehandler


__all__ = [
    "getlogger",
    "configure",
    "shutdown",
    "getformatter",
    "consolehandler",
    "filehandler",
    "sizehandler",
    "timehandler",
]

