"""
jlib.log.fmt

Reusable logging formatters.

Defines formatting presets without configuring loggers,
creating handlers, or producing output.
"""

def getformatter(style="simple", datefmt="%Y-%m-%d %H:%M:%S"):
    """
    Return a logging.Formatter using a named preset.

    Args:
        style (str): Formatting preset.
        datefmt (str): Timestamp formatting string.

    Returns:
        logging.Formatter: Configured formatter instance.
    """
    import logging

    formats = {
        "minimal": "%(message)s",

        "simple": "%(levelname)s | %(message)s",

        "detailed": (
            "%(asctime)s | %(levelname)s | "
            "%(name)s | %(message)s"
        ),

        "debug": (
            "%(asctime)s | %(levelname)s | "
            "%(name)s | %(filename)s:%(lineno)d | %(message)s"
        ),
    }

    if not isinstance(style, str):
        raise ValueError("Formatter style must be a string.")

    style = style.lower()

    if style not in formats:
        raise ValueError(
            f"Unknown formatter style '{style}'. "
            f"Available styles: {', '.join(formats)}"
        )

    return logging.Formatter(
        fmt=formats[style],
        datefmt=datefmt
    )


def makeformatter(style="standard", datefmt="%Y-%m-%d %H:%M:%S"):
    import logging

    formats = {
        "standard": "%(asctime)s | %(levelname)-8s | %(name)s | %(message)s",
        "simple": "%(levelname)s: %(message)s",
        "detailed": "%(asctime)s | %(levelname)-8s | %(name)s | %(filename)s:%(lineno)d | %(message)s",
    }
    if style not in formats:
        raise ValueError(f"Unknown log format {style!r}; choose from {', '.join(formats)}")
    return logging.Formatter(formats[style], datefmt=datefmt)
