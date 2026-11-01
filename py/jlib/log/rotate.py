
"""
jlib.log.rotate

Rotating logging handler factories.

Supports size-based and time-based log rotation.
No handlers or files are created during import.
"""


def _prepare(path, level, style):
    """Validate shared rotating-handler settings."""
    from pathlib import Path
    from .handlers import _level
    from .format import getformatter

    destination = Path(path)

    if not destination.parent.is_dir():
        raise FileNotFoundError(
            f"Log directory does not exist: {destination.parent}"
        )

    return destination, _level(level), getformatter(style)


def sizehandler(path, max_bytes=5_000_000, backups=3,
                level="DEBUG", style="detailed",
                encoding="utf-8"):
    """Create a size-based rotating file handler."""
    from logging.handlers import RotatingFileHandler

    if isinstance(max_bytes, bool) or not isinstance(max_bytes, int) or max_bytes <= 0:
        raise ValueError("max_bytes must be a positive integer.")

    if isinstance(backups, bool) or not isinstance(backups, int) or backups < 1:
        raise ValueError("backups must be a positive integer.")

    destination, log_level, formatter = _prepare(
        path, level, style
    )

    handler = RotatingFileHandler(
        destination,
        maxBytes=max_bytes,
        backupCount=backups,
        encoding=encoding,
        delay=True
    )

    handler.setLevel(log_level)
    handler.setFormatter(formatter)

    return handler


def timehandler(path, when="midnight", interval=1, backups=7,
                level="DEBUG", style="detailed",
                encoding="utf-8", utc=False):
    """Create a time-based rotating file handler."""
    from logging.handlers import TimedRotatingFileHandler

    valid_when = {"S", "M", "H", "D", "MIDNIGHT"}
    valid_when.update(f"W{i}" for i in range(7))

    if not isinstance(when, str) or when.upper() not in valid_when:
        raise ValueError(
            "Invalid rotation interval. Use S, M, H, D, "
            "midnight, or W0-W6."
        )

    if isinstance(interval, bool) or not isinstance(interval, int) or interval < 1:
        raise ValueError("interval must be a positive integer.")

    if isinstance(backups, bool) or not isinstance(backups, int) or backups < 1:
        raise ValueError("backups must be a positive integer.")

    destination, log_level, formatter = _prepare(
        path, level, style
    )

    handler = TimedRotatingFileHandler(
        destination,
        when=when.upper(),
        interval=interval,
        backupCount=backups,
        encoding=encoding,
        delay=True,
        utc=utc
    )

    handler.setLevel(log_level)
    handler.setFormatter(formatter)

    return handler

def configure(
    name="jlib",
    console=True,
    file=None,
    level="DEBUG",
    console_level="INFO",
    file_level="DEBUG",
    style="simple",
    file_style="detailed",
    rotate=None,
    max_bytes=5_000_000,
    backups=3,
    when="midnight",
    interval=1,
    utc=False
):
    """
    Configure a named logger with optional console and file output.

    Replaces only handlers managed by jlib.log.
    Existing configuration remains intact if preparation fails.
    """
    from .handlers import consolehandler, filehandler, _level
    from .rotate import sizehandler, timehandler
    from .logger import getlogger

    if not isinstance(name, str) or not name.strip():
        raise ValueError("Logger name must be a non-empty string.")

    if not isinstance(console, bool):
        raise ValueError("console must be a boolean.")

    if rotate not in (None, "size", "time"):
        raise ValueError("rotate must be None, 'size', or 'time'.")

    if rotate is not None and file is None:
        raise ValueError("Rotation requires a file destination.")

    if not console and file is None:
        raise ValueError("At least one logging destination is required.")

    log_level = _level(level)
    logger = getlogger(name)
    new_handlers = []

    try:
        if console:
            new_handlers.append(
                consolehandler(
                    level=console_level,
                    style=style
                )
            )

        if file is not None:
            options = {
                "path": file,
                "level": file_level,
                "style": file_style
            }

            if rotate == "size":
                handler = sizehandler(
                    **options,
                    max_bytes=max_bytes,
                    backups=backups
                )

            elif rotate == "time":
                handler = timehandler(
                    **options,
                    when=when,
                    interval=interval,
                    backups=backups,
                    utc=utc
                )

            else:
                handler = filehandler(**options)

            new_handlers.append(handler)

    except Exception:
        for handler in new_handlers:
            handler.close()
        raise

    # Preserve previous logger settings.
    old_level = logger.level
    old_propagate = logger.propagate

    # Track only handlers previously created by jlib.
    old_handlers = [
        handler for handler in logger.handlers
        if getattr(handler, "_jlib_managed", False)
    ]

    try:
        for handler in new_handlers:
            handler._jlib_managed = True
            logger.addHandler(handler)

        logger.setLevel(log_level)
        logger.propagate = False

    except Exception:
        for handler in new_handlers:
            logger.removeHandler(handler)
            handler.close()

        logger.setLevel(old_level)
        logger.propagate = old_propagate
        raise

    # Commit: remove the previous managed configuration.
    for handler in old_handlers:
        logger.removeHandler(handler)
        handler.close()

    return logger


def shutdown(name=None):
    """
    Detach and close handlers managed by jlib.log.

    Args:
        name: Specific logger name, or None for all managed loggers.

    Does not remove unrelated handlers.
    """
    import logging

    if name is not None:
        if not isinstance(name, str) or not name.strip():
            raise ValueError("Logger name must be a non-empty string.")
        loggers = [logging.getLogger(name)]

    else:
        registry = logging.Logger.manager.loggerDict

        loggers = [
            logger for logger in registry.values()
            if isinstance(logger, logging.Logger)
        ]

        loggers.append(logging.getLogger())

    for logger in loggers:
        for handler in logger.handlers[:]:
            if getattr(handler, "_jlib_managed", False):
                logger.removeHandler(handler)
                handler.close()
