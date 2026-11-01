
"""
jlib.err.fmt

Exception formatting and diagnostic utilities.

Produces readable or structured representations of exceptions
without printing, logging, writing files, or suppressing errors.
"""


def _validate(error):
    """Ensure the supplied object is an exception instance."""
    if not isinstance(error, BaseException):
        raise TypeError(
            "Expected an exception instance derived from BaseException."
        )

    return error


def format_error(error, include_module=False):
    """
    Return a concise, human-readable exception description.

    Args:
        error: Exception instance.
        include_module: Include the exception class's module name.

    Returns:
        str: Formatted exception description.
    """
    _validate(error)

    error_type = type(error)

    if include_module:
        name = f"{error_type.__module__}.{error_type.__qualname__}"
    else:
        name = error_type.__name__

    message = str(error)

    return f"{name}: {message}" if message else name


def error_details(error, include_traceback=False):
    """
    Return structured diagnostic information about an exception.

    Args:
        error: Exception instance.
        include_traceback: Include formatted traceback text.

    Returns:
        dict: Structured error information.
    """
    _validate(error)

    details = {
        "type": type(error).__name__,
        "module": type(error).__module__,
        "message": str(error),
        "args": error.args,
        "cause": (
            format_error(error.__cause__)
            if error.__cause__ is not None
            else None
        ),
        "context": (
            format_error(error.__context__)
            if error.__context__ is not None
            else None
        ),
        "suppress_context": error.__suppress_context__,
    }

    if include_traceback:
        details["traceback"] = format_traceback(error)

    return details


def format_traceback(error, chain=True):
    """
    Return a complete formatted traceback as a string.

    Args:
        error: Exception instance.
        chain: Include chained exceptions when True.

    Returns:
        str: Formatted traceback text.
    """
    import traceback

    _validate(error)

    return "".join(
        traceback.format_exception(
            error,
            chain=chain
        )
    )
