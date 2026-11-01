# `jlib.err` — Error Handling Field Manual

> **Status:** v0.1 foundation implemented and tested. **40 `err` tests passed**, with **88 tests passing across `jlib`** at the last full regression run.
>
> **Placement:** Save this document as `jlib/err/README.md`.

## Why this exists

`jlib` is a portable collection of Python utilities intended to work across machines with different permissions, environments, and installed packages. `jlib.err` gives those utilities a shared vocabulary for failures without taking control away from the calling application.

The design goal is **diagnose clearly, preserve the original failure, and let the caller decide what to do**. A formatter returns data or text; it does not print, log, write files, retry, or install packages. Missing optional dependencies should affect only the functions that actually need them.

## Package map

```text
jlib/
└── err/
    ├── __init__.py       # Public exports
    ├── excp.py           # Custom exception classes
    ├── deps.py           # Lazy dependency loading and inspection
    ├── fmt.py            # Exception formatting and diagnostics
    ├── README.md         # This field manual
    └── test/
        ├── __init__.py
        ├── test_excp.py
        ├── test_deps.py
        ├── test_fmt.py
        └── test_integration.py
```

`redo.py` (automatic retry/recovery) is **deferred**, not part of the implemented API.

### Dependency direction

```text
excp.py  ←  deps.py

fmt.py   (independent; formats any BaseException)

log/     (independent of err/)
```

`err` must not require `log` to import successfully. A caller may combine them, but neither formatter nor exception definition should automatically configure logging.

## Quick reference

### Exceptions — `excp.py`

| Name | Inherits from | Intended use |
|---|---|---|
| `JLibError` | `Exception` | Base class for library-specific errors |
| `DependencyError` | `JLibError`, `ImportError` | Requested optional dependency is unavailable |
| `ConfigurationError` | `JLibError`, `ValueError` | Invalid library-specific configuration |
| `OperationError` | `JLibError` | A genuinely library-specific operation failure |

**Do not replace useful built-ins without a reason.** A missing file should generally remain `FileNotFoundError`; a permissions problem should remain `PermissionError`.

### Dependencies — `deps.py`

| Function | Returns | Behavior |
|---|---|---|
| `require(module, package=None)` | Imported module | Imports on demand; raises `DependencyError` for a missing requested module |
| `available(module)` | `bool` | Uses `importlib.util.find_spec` to check discoverability |
| `check(modules)` | `dict[str, bool]` | Checks multiple module names |

### Diagnostics — `fmt.py`

| Function | Returns | Behavior |
|---|---|---|
| `format_error(error, include_module=False)` | `str` | Concise exception type and message |
| `error_details(error, include_traceback=False)` | `dict` | Type, module, message, args, cause, context, suppression flag, optional traceback |
| `format_traceback(error, chain=True)` | `str` | Complete formatted traceback, optionally including chained exceptions |

All three formatting functions accept an **exception instance**, not an exception class or plain string.

## Everyday usage

### 1. Catch a familiar built-in error and return a message

```python
from jlib.err import format_error


def read_setting(path):
    try:
        with open(path, encoding="utf-8") as file:
            return file.read()
    except OSError as error:
        return format_error(error)
```

This example returns either file content or an error string, which can be awkward for callers to distinguish. For a reusable API, it is often better to **let the exception propagate** and format it at the application boundary:

```python
from jlib.err import format_error

try:
    result = read_setting("settings.txt")
except OSError as error:
    print(format_error(error))  # The application chooses to print.
```

### 2. Load an optional package only inside the function that needs it

```python
def load_spreadsheet(path):
    from jlib.err import require

    pd = require("pandas")
    return pd.read_excel(path)
```

`import jlib` or importing an unrelated utility does **not** require pandas. Only calling `load_spreadsheet()` does.

### 3. Import name differs from installation name

```python
def image_dimensions(path):
    from jlib.err import require

    image_module = require("PIL.Image", package="Pillow")
    with image_module.open(path) as image:
        return image.size
```

The `package` argument is used for the **installation hint**, not for the Python import statement.

### 4. Check capabilities before presenting optional features

```python
from jlib.err import check

capabilities = check(["pandas", "PIL", "json"])

if capabilities["PIL"]:
    # Offer image-related functionality.
    pass
```

**Important:** `available()` and `check()` report whether a module can be *located*, not whether it will successfully import or work. Checking a dotted name may import its parent package.

## Wizardry: the mechanisms worth remembering

### A. `raise ... from ...` preserves the original cause

```python
try:
    int("banana")
except ValueError as original:
    raise RuntimeError("Could not parse the value") from original
```

The resulting `RuntimeError` has an explicit `__cause__` pointing to the `ValueError`. This is useful when converting a low-level failure into a more meaningful public error **without destroying its origin**.

```python
from jlib.err import error_details, format_traceback

try:
    try:
        int("banana")
    except ValueError as original:
        raise RuntimeError("Parsing failed") from original
except RuntimeError as error:
    details = error_details(error)
    report = format_traceback(error)
    # Both values are returned data, not automatically printed.
```

### B. `__cause__`, `__context__`, and `__suppress_context__`

- **`__cause__`**: Explicit link created by `raise NewError(...) from original`.
- **`__context__`**: Implicit link when an exception occurs while handling another.
- **`__suppress_context__`**: Indicates whether implicit context should be hidden in standard traceback rendering, as with `raise ... from None`.

`error_details()` exposes all three. `format_traceback(chain=False)` excludes chained exceptions from its rendered output; it does not erase the exception objects.

### C. Missing dependency versus broken dependency

`require()` catches `ModuleNotFoundError` only when the missing module name matches the requested import path (or one of its parents). For example, requesting `PIL.Image` when `PIL` is absent should give an actionable `DependencyError`.

But if an installed package fails while importing an **unrelated internal dependency**, that original error should propagate. Recommending `pip install` for every `ImportError` would misdiagnose broken installations, incompatible versions, or bugs.

**Edge case:** A package can deliberately raise a `ModuleNotFoundError` naming itself during initialization. No simple exception-name check can perfectly distinguish that from absence.

### D. One exception, multiple catch strategies

```python
from jlib.err import DependencyError, JLibError

assert issubclass(DependencyError, JLibError)
assert issubclass(DependencyError, ImportError)
```

A caller may catch all library-specific failures with `except JLibError`, or use standard Python categories such as `except ImportError`. Multiple inheritance is used sparingly because built-in exception classes may have implementation-specific constraints.

### E. `pprint.pformat` versus `pprint.pp`

```python
from pprint import pformat
from jlib.err import error_details

report = pformat(
    error_details(ValueError("Bad value")),
    width=100,
    sort_dicts=False,
)
```

`pformat()` **returns a string**. `pp()` writes output. Our convention is to return formatted results almost always; the caller decides where to send them. `err/fmt.py` itself does not need to call `pprint` to return a structured dictionary.

### F. Logger integration without circular imports

```python
from jlib.err import format_error, format_traceback
from jlib.log import getlogger

logger = getlogger("jlib.dirt")

try:
    raise ValueError("Example failure")
except ValueError as error:
    logger.error(format_error(error))
    # Or, when the full traceback is wanted:
    logger.error(format_traceback(error))
```

Inside an active exception handler, standard logging can do this more directly:

```python
try:
    raise ValueError("Example failure")
except ValueError:
    logger.exception("Operation failed")
```

`logger.exception()` includes the current exception traceback. It should be called from an exception-handling context. The logger must be configured by the application if output is desired.

## Design decisions and tradeoffs

| Decision | Reason | Tradeoff |
|---|---|---|
| Function-local imports | Unrelated features remain usable without optional dependencies | Import statement executes on each call, though modules are cached |
| No automatic `pip install` | Avoid unexpected changes, network access, and workplace policy issues | User must choose how to install packages |
| Return diagnostics, don't print | Reusable in CLI, tests, logging, and other callers | Caller must explicitly display or record results |
| Preserve native exceptions | Familiar, informative error categories | Not every failure inherits from `JLibError` |
| No automatic retries | Avoid repeating writes or operations with partial side effects | Caller handles recovery manually |
| No dependency on `jlib.log` | Prevent tight coupling and circular imports | Application coordinates formatting and logging |
| No generic `except Exception: pass` | Failures remain visible | Caller must make explicit recovery choices |
| Keep `__init__.py` minimal | Prevent unfinished modules from breaking imports | Exports expand only after implementation |

## Gotchas and security notes

1. **Tracebacks can contain sensitive information.** Paths, usernames, file content fragments, credentials embedded in exception messages, and student data may appear. Review before logging, sharing, or exporting.
2. **`error_details()["args"]` is not guaranteed JSON-safe.** Exception arguments may contain arbitrary Python objects.
3. **An exception object can exist without a traceback.** A traceback becomes available when the exception is raised; formatting an unraised exception still yields useful type/message text.
4. **`BaseException` includes `KeyboardInterrupt` and `SystemExit`.** Formatting them is supported, but ordinary application code should generally not swallow them.
5. **Module discoverability is not successful importability.** `available()` is a diagnostic hint, not a dependency guarantee.
6. **Don't accidentally shadow standard modules.** Naming a top-level file `logging.py`, `traceback.py`, or `pprint.py` can interfere with imports. A scoped `jlib/err/fmt.py` is fine.
7. **A returned error string is not a substitute for an exception contract.** Consider whether the caller needs to distinguish success from failure programmatically.

## Testing and verification

Run from the **parent directory containing `jlib/`**, using PowerShell:

```powershell
python.exe -m unittest jlib.err.test.test_excp -v
python.exe -m unittest jlib.err.test.test_deps -v
python.exe -m unittest jlib.err.test.test_fmt -v
python.exe -m unittest jlib.err.test.test_integration -v
```

Run the complete library regression suite:

```powershell
python.exe -m unittest discover -s . -t . -p "test*.py" -v
```

Last confirmed results:

| Suite | Tests passed |
|---|---:|
| `err/excp.py` | 8 |
| `err/deps.py` | 12 |
| `err/fmt.py` | 12 |
| `err` integration | 8 |
| **`err` total** | **40** |
| **Entire `jlib`** | **88** |

Passing tests verify the exercised behaviors; they do not prove absence of every bug, import side effect, security issue, or environmental incompatibility.

## Future experiments — not implemented

- Optional sanitization/redaction for sensitive exception messages and tracebacks.
- JSON-safe diagnostic serialization with explicit handling of arbitrary exception arguments.
- Diagnostic reports with configurable traceback depth and contextual metadata.
- `ExceptionGroup` / `except*` utilities for concurrent or batched operations.
- Carefully scoped retry helpers (`redo.py`) **only when a concrete use case justifies them**.
- Optional structured error-to-log integration, controlled by the calling application.

## The rule to remember

> **Errors should be informative, predictable, and recoverable when appropriate — but never hidden.**
>
> **Formatters return. Callers decide what happens next.**

`jlib.err` is a foundation, not a global exception trap. Its job is to help you understand failures without taking control away from your program.
