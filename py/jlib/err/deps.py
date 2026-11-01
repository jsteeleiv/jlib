
"""
jlib.err.deps

Lazy dependency loading and availability inspection.

Does not automatically install packages, access the network,
or modify the Python environment.
"""


def _validate(module):
    """Validate a Python module name."""
    if not isinstance(module, str) or not module:
        raise ValueError("Module name must be a non-empty string.")

    parts = module.split(".")

    if not all(part.isidentifier() for part in parts):
        raise ValueError(f"Invalid module name: {module!r}")

    return module


def require(module, package=None):
    """
    Import a dependency only when explicitly requested.

    Args:
        module: Python import name, e.g. 'PIL.Image'.
        package: Installation name, e.g. 'Pillow'.

    Returns:
        The imported module.

    Raises:
        DependencyError: If the requested dependency is missing.
        ImportError: If an installed dependency fails internally.
    """
    from importlib import import_module
    from .excp import DependencyError

    _validate(module)

    if package is not None:
        if not isinstance(package, str) or not package.strip():
            raise ValueError("Package name must be a non-empty string.")

    install_name = package or module.split(".")[0]

    try:
        return import_module(module)

    except ModuleNotFoundError as exc:
        missing = exc.name

        # Only treat missing components of the requested
        # import path as a missing requested dependency.
        requested_parts = module.split(".")
        requested_paths = {
            ".".join(requested_parts[:i])
            for i in range(1, len(requested_parts) + 1)
        }

        if missing not in requested_paths:
            raise

        raise DependencyError(
            f"Required optional dependency '{module}' is unavailable. "
            f"Install package '{install_name}' using: "
            f"python -m pip install {install_name}"
        ) from exc


def available(module):
    """
    Check whether a module can be located without importing it.

    Returns:
        bool: True if located, False otherwise.

    Note:
        Availability does not guarantee successful import.
    """
    from importlib.util import find_spec

    _validate(module)

    try:
        return find_spec(module) is not None

    except ModuleNotFoundError as exc:
        parts = module.split(".")
        requested_paths = {
            ".".join(parts[:i])
            for i in range(1, len(parts) + 1)
        }

        if exc.name in requested_paths:
            return False

        raise


def check(modules):
    """
    Check multiple module names without importing them directly.

    Args:
        modules: Iterable of module names.

    Returns:
        dict: Mapping of module names to availability booleans.
    """
    return {
        module: available(module)
        for module in modules
    }
