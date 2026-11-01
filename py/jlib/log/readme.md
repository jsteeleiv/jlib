# `jlib.log` — Logging Field Manual

> **Purpose:** A portable, standard-library-only logging toolkit with interchangeable destinations, predictable cleanup, and no configuration at import time.
>
> **Status:** Core logging functionality implemented; 48 logging tests passed in the development workflow. This guide reflects the interfaces established during development, not a fresh inspection of your local files.

## Why this exists

`jlib.log` lets reusable code report *what happened* without deciding *where the report goes*. A directory scanner, game, or data processor can emit the same log message whether the calling application wants console output, a rotating file, or—eventually—another destination.

This matters for a personal library that moves between machines. Importing a module should not create files, install packages, contact services, or take over the host application's logging configuration. The caller opts in to destinations by calling `configure()`.

**Design maxim:** The code that *produces* an event should not need to know how that event is *delivered*.

## Package map

```text
jlib/
  log/
    __init__.py      Public exports
    log.py          getlogger(), configure(), shutdown()
    fmt.py           getformatter() and formatting presets
    handlers.py      consolehandler(), filehandler()
    rotate.py        sizehandler(), timehandler()
    test/            Portable unittest suite
      __init__.py
      test_core.py
      test_fmt.py
      test_handlers.py
      test_rotate.py
      test_core_conf.py
```

The filenames shown for tests are conventional examples; keep the actual names you've chosen as long as they start with `test` and end with `.py`.

**Dependency direction:** `log` uses `handlers` and `rotate`; those use `fmt`. The logging package uses the Python standard library directly, **not** `jlib.dirt` or `jlib.err`. This avoids circular dependencies when filesystem and error utilities themselves need logging.

## Quick reference

| API | What it does | Side effects |
| --- | --- | --- |
| `getlogger(name="jlib")` | Get the same named `logging.Logger` object each time | Registers a logger in memory if absent |
| `configure(...)` | Attach managed console/file handlers; set logger level and propagation | May connect streams; delayed file handlers create files on first eligible write |
| `shutdown(name=None)` | Detach and close managed handlers; restore saved logger settings | Closes open streams; no deletion of log files |
| `getformatter(style="simple", datefmt=...)` | Create a formatter | None beyond object creation |
| `consolehandler(level="INFO", style="simple", stream=None)` | Make a stream handler (stderr by default) | Captures an output stream; no message until used |
| `filehandler(path, level="DEBUG", style="detailed", ...)` | Make a delayed file handler | Checks parent directory; opens file on first eligible emission |
| `sizehandler(path, max_bytes=5_000_000, backups=3, ...)` | Size-rotating file handler | Renames/creates log files when emitting/rotating |
| `timehandler(path, when="midnight", interval=1, backups=7, ...)` | Time-rotating file handler | Renames/creates log files when emitting/rotating |

`configure()` as implemented in our current design accepts `name`, `console`, `file`, `console_level`, `file_level`, `style`, `file_style`, `rotate`, `max_bytes`, `backups`, `when`, `interval`, `utc`, and `propagate`. The logger's effective entry threshold is automatically selected from its configured handlers; there is **no separate `level=` argument**.

## Everyday usage

### Console only

```python
from jlib.log import configure, shutdown

log = configure(name="jlib.demo", console=True, console_level="INFO")
try:
    log.info("Starting")
    log.warning("Something needs attention")
finally:
    shutdown("jlib.demo")
```

### File only, with no surprise console output

```python
from jlib.log import configure, shutdown

log = configure(
    name="jlib.report",
    console=False,
    file="report.log",
    file_level="DEBUG",
)
try:
    log.debug("Preparing report")
    log.info("Report complete")
finally:
    shutdown("jlib.report")
```

**Important:** The destination's parent directory must already exist. A relative path such as `report.log` is relative to the *process working directory*, not necessarily the module's directory. `delay=True` postpones opening the file until an eligible record is emitted; permission errors may therefore occur at emission time.

### Quiet console, verbose file

```python
from jlib.log import configure, shutdown

log = configure(
    name="jlib.dirt",
    console=True,
    file="activity.log",
    console_level="WARNING",
    file_level="DEBUG",
    style="simple",
    file_style="debug",
    rotate="size",
    max_bytes=2_000_000,
    backups=4,
)
try:
    log.debug("Checking file metadata")   # File only
    log.info("Traversal started")         # File only
    log.warning("Skipped one file")       # Both
finally:
    shutdown("jlib.dirt")
```

### Time-based rotation

```python
log = configure(
    name="jlib.daily",
    console=False,
    file="daily.log",
    rotate="time",
    when="midnight",
    interval=1,
    backups=7,
    utc=False,
)
```

Rotation occurs **when messages are emitted**, not through a background timer. `W0`–`W6` designate weekdays for weekly rotation (Monday through Sunday). File rotation is not a multiprocess-safe logging solution.

## Wizardry: concepts worth remembering

### 1. Logger identity is a registry, not a new object every call

```python
from jlib.log import getlogger
assert getlogger("jlib.dirt") is getlogger("jlib.dirt")
```

Python's `logging.getLogger()` keeps named logger objects in a process-wide registry. This is why a module can call `getlogger("jlib.dirt")` without being passed a logger object by the application. It does **not** mean calling `getlogger()` writes a message.

### 2. Logger levels and handler levels are two gates

A message must pass the originating logger's level **and** the destination handler's level. Our `configure()` sets the logger's level to the lowest configured handler threshold, so a DEBUG file handler is not accidentally starved by a WARNING console handler.

```text
DEBUG record -> logger (DEBUG) -> console (WARNING): skip
                                -> file (DEBUG): write
```

This is the trick behind independent console/file verbosity.

### 3. Names form a hierarchy, even without matching folders

`jlib.dirt.search` is a child logger of `jlib.dirt`, which is a child of `jlib`. The dots are a **logging namespace**, not a filesystem path. With propagation enabled, records can reach ancestor handlers. With propagation disabled, they stop at that logger.

**Gotcha:** Configuring both a parent and child with handlers and enabling propagation can produce duplicate output. Our `configure()` defaults to `propagate=False`, but use deliberate naming and configuration when mixing loggers.

### 4. Reconfiguration is ownership-aware

Our implementation marks its own handlers using the internal `_jlib_managed` marker. Reconfiguring a logger replaces only those handlers; unrelated handlers are preserved. Keep the marker centralized as `_MANAGED` in `log.py` so configuration and shutdown cannot disagree about ownership.

**Boundary:** This is a private implementation convention, not a security boundary or a public Python logging protocol. The implementation is designed for sequential configuration, not concurrent reconfiguration across threads.

### 5. Shutdown restores the previous logger settings

On the first successful configuration, the module records the logger's previous `level` and `propagate` values. Reconfiguration must **not** overwrite that original snapshot. `shutdown()` closes managed handlers and restores those values.

**Caveat:** If another part of the application changes those settings while our configuration is active, restoring the earlier snapshot can overwrite that intervening change. Treat the logger's configuration as owned by one coordinator during its lifetime.

### 6. Lazy file opening is not a guarantee that writing will succeed

A delayed file handler may be constructed successfully even if the file becomes unwritable later. Handle real filesystem failures where records are emitted, and always close handlers before deleting temporary directories on Windows.

### 7. Structured messages are better than preformatted strings

```python
log.debug("Scanning %s (%d bytes)", filename, size)
```

Logging keeps the format string and arguments separate until formatting is needed. This avoids doing unnecessary interpolation for filtered-out records. Compare with `log.debug(f"Scanning {filename} ({size} bytes)")`, which constructs the string immediately.

### 8. Capture tracebacks when handling exceptions

```python
try:
    process_file()
except OSError:
    log.exception("File processing failed")
    raise
```

`log.exception()` adds the active exception traceback. Re-raising preserves the failure instead of silently treating it as success. **Do not log and re-raise at every layer** or you may record the same failure repeatedly.

## Formatting presets

The `getformatter()` implementation we tested supports:

| Preset | Fields |
| --- | --- |
| `minimal` | Message only |
| `simple` | Level and message |
| `detailed` | Timestamp, level, logger name, message |
| `debug` | Timestamp, level, logger name, source filename/line, message |

Custom date formatting uses `strftime` directives such as `%Y-%m-%d %H:%M:%S`. A timestamp based on Unix epoch zero may display **1969** in timezones behind UTC; tests should calculate the expected local year rather than hardcoding 1970.

**Legacy note:** Earlier development versions included `makeformatter()` and a `standard` style. The public interface described here uses `getformatter()` and the presets above. Avoid mixing the two generations of code.

## Safety, privacy, and operational boundaries

- Logging must be explicitly configured; imports must not create handlers, files, or network connections.
- `filehandler(..., mode="w")` can overwrite an existing file; default append mode is safer.
- Do not log API tokens, passwords, cookies, student records, or other sensitive data. Redaction is **not yet implemented**.
- Use paths in writable, approved locations; no administrator privileges are needed merely to log there.
- `shutdown()` closes only handlers managed by `jlib.log`; avoid calling global `logging.shutdown()` as a substitute for package-specific cleanup.
- A `TemporaryDirectory` test on Windows must close open file handlers **before** the directory context exits, or cleanup may fail with `WinError 32`.
- Python's standard rotating file handlers are not designed for multiple processes simultaneously writing the same destination.
- Passing arbitrary untrusted strings into logs may introduce line breaks or misleading entries; sanitization is a future enhancement.

## Testing and verification

Run from the **parent directory of `jlib/`**:

```powershell
python.exe -m unittest jlib.log.test.test_core -v
python.exe -m unittest jlib.log.test.test_fmt -v
python.exe -m unittest jlib.log.test.test_handlers -v
python.exe -m unittest jlib.log.test.test_rotate -v
python.exe -m unittest jlib.log.test.test_core_conf -v
```

Run all package tests:

```powershell
python.exe -m unittest discover -s . -t . -p "test*.py" -v
```

The `jlib/`, `jlib/log/`, and `jlib/log/test/` directories should each have `__init__.py` for predictable discovery. The tests use `unittest`, `tempfile`, `io.StringIO`, and `unittest.mock`; no third-party test runner is required.

**Development checkpoint:** 48 logging tests passed, including integration tests for cleanup and restoring original logger settings. Later, 68 tests passed across logging, exception classes, and dependency helpers. These counts describe our previous verified development run; rerun locally after changing the code.

## Design decisions: why we chose this

| Decision | Rationale | Tradeoff |
| --- | --- | --- |
| Standard-library `logging` | Mature logger/handler/formatter ecosystem | Must understand propagation and global registry |
| Function-local imports | Keep import-time behavior minimal | Slightly more repetition in functions |
| Console/file destinations as handlers | Swap destinations without changing event producers | Requires handler lifecycle management |
| Explicit `configure()` | Calling application controls I/O | More setup than `basicConfig()` |
| Managed-handler marker | Preserve unrelated application handlers | Private convention needs consistent use |
| Original-settings registry | Reversible configuration | Assumes no competing configuration owner |
| No `dirt` dependency | Prevent `dirt -> log -> dirt` cycles | Logging uses standard file handlers directly |
| Tests colocated with module | Tests travel with portable library | Slightly larger package directory |
| Delayed file creation | Avoid unnecessary files | First write can fail later |

## Future experiments (not implemented)

- **JSON formatter:** Machine-readable events for later analysis.
- **Redaction filters:** Scrub configured secrets and sensitive fields before delivery.
- **Email handler:** Opt-in critical alerts via an institution-approved mail transport; never silently send messages.
- **Contextual logging:** Attach operation IDs, task names, or correlation IDs using `LoggerAdapter` or filters.
- **Memory buffering:** Retain recent diagnostics and flush only on failure.
- **Timed rotation tests:** Simulate rollovers without sleeping or waiting for real time.
- **Concurrency hardening:** Define ownership and locking for configuration changes.

## Final mental model

```text
Application decides destinations
          |
       configure()
          |
     Named logger
          |
   logger + handler thresholds
          |
     formatter -> handler -> destination
          |
       shutdown()
   closes owned resources and restores settings
```

**Remember:** `getlogger()` retrieves; `configure()` connects; `shutdown()` cleans up. The logger describes an event. The handlers decide where it goes.
