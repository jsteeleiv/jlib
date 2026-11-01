_MANAGED = "_jlib_managed_handler"
_ORIGINAL_SETTINGS = {}

"""
jlib.log.logger

Core logging utilities.

Provides named loggers without automatically configuring
handlers, writing files, or modifying global logging settings.
"""

def getlogger(name="jlib"):
    """
    Return a named Python logger.

    Args:
        name (str): Logger name, defaulting to 'jlib'.

    Returns:
        logging.Logger: The requested logger instance.
    """
    
    if not isinstance(name, str) or not name.strip():
        raise ValueError("Logger name must be a non-empty string.")
    
    import logging
    return logging.getLogger(name)



def configure(
    name="jlib",
    *,
    console=True,
    file=None,
    console_level="INFO",
    file_level="DEBUG",
    style="simple",
    file_style="detailed",
    rotate=None,
    max_bytes=5_000_000,
    backups=3,
    when="midnight",
    interval=1,
    utc=False,
    propagate=False
):
    """
    Configure a named logger with console and/or file output.

    Supports regular, size-rotated, and time-rotated files.
    Automatically selects the lowest handler logging level.

    Replaces only jlib-managed handlers.
    """
    from .handlers import consolehandler, filehandler, _level
    from .rotate import sizehandler, timehandler

    if not isinstance(name, str) or not name.strip():
        raise ValueError("Logger name must be a non-empty string.")

    if not isinstance(console, bool):
        raise ValueError("console must be True or False.")

    if not isinstance(propagate, bool):
        raise ValueError("propagate must be True or False.")

    if rotate not in (None, "size", "time"):
        raise ValueError(
            "rotate must be None, 'size', or 'time'."
        )

    if rotate is not None and file is None:
        raise ValueError("Rotation requires a file destination.")

    if not console and file is None:
        raise ValueError(
            "At least one logging destination is required."
        )

    # Prepare new handlers before changing existing ones.
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
            if rotate == "size":
                handler = sizehandler(
                    path=file,
                    max_bytes=max_bytes,
                    backups=backups,
                    level=file_level,
                    style=file_style
                )

            elif rotate == "time":
                handler = timehandler(
                    path=file,
                    when=when,
                    interval=interval,
                    backups=backups,
                    level=file_level,
                    style=file_style,
                    utc=utc
                )

            else:
                handler = filehandler(
                    path=file,
                    level=file_level,
                    style=file_style
                )

            new_handlers.append(handler)

    except Exception:
        for handler in new_handlers:
            handler.close()
        raise

    logger = getlogger(name)

    old_handlers = [
        handler for handler in logger.handlers
        if getattr(handler, "_jlib_managed", False)
    ]

    old_level = logger.level
    old_propagate = logger.propagate
    first_configuration = name not in _ORIGINAL_SETTINGS

    try:
        for handler in new_handlers:
            handler._jlib_managed = True
            logger.addHandler(handler)

        # Automatically use the lowest handler threshold.
        logger.setLevel(
            min(handler.level for handler in new_handlers)
        )

        logger.propagate = propagate

    except Exception:
        for handler in new_handlers:
            logger.removeHandler(handler)
            handler.close()

        logger.setLevel(old_level)
        logger.propagate = old_propagate
        raise

    # Commit the new configuration.
    for handler in old_handlers:
        logger.removeHandler(handler)
        handler.close()

    # Preserve the original settings only once.
    if first_configuration:
        _ORIGINAL_SETTINGS[name] = {
            "level": old_level,
            "propagate": old_propagate
        }
    
    return logger




def shutdown(name=None):
    """
    Detach and close handlers managed by jlib.log.

    Args:
        name: Logger name to clean up.
              None cleans up all jlib-managed handlers.

    Does not modify unrelated handlers.
    """
    import logging

    if name is not None:
        if not isinstance(name, str) or not name.strip():
            raise ValueError(
                "Logger name must be a non-empty string."
            )

        loggers = [logging.getLogger(name)]

    else:
        registry = logging.Logger.manager.loggerDict

        loggers = [
            logger
            for logger in registry.values()
            if isinstance(logger, logging.Logger)
        ]

        loggers.append(logging.getLogger())

    for logger in loggers:
        for handler in logger.handlers[:]:
            if getattr(handler, "_jlib_managed", False):
                logger.removeHandler(handler)
                handler.close()
        
        original = _ORIGINAL_SETTINGS.pop(logger.name, None)

        if original is not None:
            logger.setLevel(original["level"])
            logger.propagate = original["propagate"]