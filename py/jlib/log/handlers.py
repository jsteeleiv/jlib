"""
jlib.log.handlers

Console and file logging handler factories.

Handlers are created only when explicitly requested.
No logging configuration occurs during module import.
"""


def _level(level):
    """Validate and normalize a logging level."""
    import logging

    if isinstance(level, bool):
        raise ValueError("Logging level cannot be a boolean.")

    if isinstance(level, int):
        if level < 0:
            raise ValueError("Logging level cannot be negative.")
        return level

    if isinstance(level, str):
        name = level.upper()

        if name in logging.getLevelNamesMapping():
            return logging.getLevelNamesMapping()[name]

    raise ValueError(f"Invalid logging level: {level!r}")


def consolehandler(level="INFO", style="simple", stream=None):
    """
    Create a console logging handler.

    Args:
        level: Minimum logging level.
        style: Formatter preset.
        stream: Optional output stream. Defaults to stderr.

    Returns:
        logging.StreamHandler
    """
    import logging
    from .fmt import getformatter

    handler = logging.StreamHandler(stream)
    handler.setLevel(_level(level))
    handler.setFormatter(getformatter(style))

    return handler


def filehandler(path, level="DEBUG", style="detailed",
                encoding="utf-8", mode="a"):
    """
    Create a file logging handler.

    Args:
        path: Destination file path.
        level: Minimum logging level.
        style: Formatter preset.
        encoding: File encoding.
        mode: 'a' to append or 'w' to overwrite.

    Returns:
        logging.FileHandler
    """
    import logging
    from pathlib import Path
    from .fmt import getformatter

    if mode not in ("a", "w"):
        raise ValueError(
            "File handler mode must be 'a' or 'w'."
        )

    destination = Path(path)

    if not destination.parent.is_dir():
        raise FileNotFoundError(
            f"Log directory does not exist: {destination.parent}"
        )

    # Validate before opening the destination.
    normalized_level = _level(level)
    formatter = getformatter(style)

    handler = logging.FileHandler(
        destination,
        mode=mode,
        encoding=encoding,
        delay=True
    )

    handler.setLevel(normalized_level)
    handler.setFormatter(formatter)

    return handler

