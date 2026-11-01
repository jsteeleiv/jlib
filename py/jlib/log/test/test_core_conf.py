
"""
jlib.log.test.test_core_conf

Integration tests for logging configuration and cleanup.

Tests:
    - Console and file output
    - Independent logging thresholds
    - Automatic logger-level selection
    - Handler ownership and reconfiguration
    - Failed configuration rollback
    - Size-based and time-based rotation
    - Shutdown and resource cleanup

Run:
    python.exe -m unittest jlib.log.test.test_core_conf -v
"""

import io
import logging
import tempfile
import unittest

from pathlib import Path
from unittest.mock import patch
from logging.handlers import (
    RotatingFileHandler,
    TimedRotatingFileHandler
)

from jlib.log import configure, shutdown


class TestCoreConfiguration(unittest.TestCase):

    def setUp(self):
        self.name = f"jlib.test.config.{self._testMethodName}"
        self.logger = logging.getLogger(self.name)

        self.original_level = self.logger.level
        self.original_propagate = self.logger.propagate

    def tearDown(self):
        shutdown(self.name)

        self.logger.setLevel(self.original_level)
        self.logger.propagate = self.original_propagate

    def test_console_configuration(self):
        stream = io.StringIO()

        with patch("sys.stderr", stream):
            logger = configure(
                name=self.name,
                console=True,
                console_level="INFO"
            )

            logger.info("Console test message")

        self.assertIn(
            "Console test message",
            stream.getvalue()
        )

    def test_file_configuration(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "activity.log"

            logger = configure(
                name=self.name,
                console=False,
                file=path
            )

            self.assertFalse(path.exists())

            logger.info("File test message")

            self.assertTrue(path.exists())
            self.assertIn(
                "File test message",
                path.read_text(encoding="utf-8")
            )

            shutdown(self.name)

    def test_independent_levels(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "activity.log"
            stream = io.StringIO()

            with patch("sys.stderr", stream):
                logger = configure(
                    name=self.name,
                    console=True,
                    file=path,
                    console_level="WARNING",
                    file_level="DEBUG"
                )

                logger.debug("Debug message")
                logger.warning("Warning message")

            console_output = stream.getvalue()
            file_output = path.read_text(encoding="utf-8")

            self.assertNotIn("Debug message", console_output)
            self.assertIn("Warning message", console_output)

            self.assertIn("Debug message", file_output)
            self.assertIn("Warning message", file_output)

            shutdown(self.name)

    def test_automatic_logger_level(self):
        logger = configure(
            name=self.name,
            console=True,
            console_level="WARNING"
        )

        self.assertEqual(logger.level, logging.WARNING)

    def test_reconfiguration_no_duplicates(self):
        configure(name=self.name)
        configure(name=self.name)

        managed = [
            handler for handler in self.logger.handlers
            if getattr(handler, "_jlib_managed", False)
        ]

        self.assertEqual(len(managed), 1)

    def test_failed_configuration_preserves_old(self):
        configure(name=self.name, console_level="INFO")

        old_handlers = self.logger.handlers[:]
        old_level = self.logger.level
        old_propagate = self.logger.propagate

        with self.assertRaises(ValueError):
            configure(
                name=self.name,
                console=True,
                style="nonexistent"
            )

        self.assertEqual(self.logger.handlers, old_handlers)
        self.assertEqual(self.logger.level, old_level)
        self.assertEqual(self.logger.propagate, old_propagate)

    def test_external_handler_preserved(self):
        external = logging.NullHandler()
        self.logger.addHandler(external)

        try:
            configure(name=self.name)

            self.assertIn(external, self.logger.handlers)

            shutdown(self.name)

            self.assertIn(external, self.logger.handlers)

        finally:
            self.logger.removeHandler(external)
            external.close()

    def test_size_rotation_configuration(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "rotating.log"

            configure(
                name=self.name,
                console=False,
                file=path,
                rotate="size",
                max_bytes=100,
                backups=2
            )

            handlers = [
                handler for handler in self.logger.handlers
                if isinstance(handler, RotatingFileHandler)
            ]

            self.assertEqual(len(handlers), 1)
            shutdown(self.name)

    def test_time_rotation_configuration(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "timed.log"

            configure(
                name=self.name,
                console=False,
                file=path,
                rotate="time",
                when="midnight",
                backups=7
            )

            handlers = [
                handler for handler in self.logger.handlers
                if isinstance(handler, TimedRotatingFileHandler)
            ]

            self.assertEqual(len(handlers), 1)
            shutdown(self.name)

    def test_invalid_destination(self):
        with self.assertRaises(ValueError):
            configure(
                name=self.name,
                console=False,
                file=None
            )

    def test_shutdown_removes_managed_handlers(self):
        configure(name=self.name)

        self.assertTrue(
            any(
                getattr(h, "_jlib_managed", False)
                for h in self.logger.handlers
            )
        )

        shutdown(self.name)

        self.assertFalse(
            any(
                getattr(h, "_jlib_managed", False)
                for h in self.logger.handlers
            )
        )

    def test_shutdown_closes_file_handler(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "activity.log"

            logger = configure(
                name=self.name,
                console=False,
                file=path
            )

            logger.info("Open the log file")

            handler = next(
                h for h in logger.handlers
                if getattr(h, "_jlib_managed", False)
            )

            self.assertIsNotNone(handler.stream)

            shutdown(self.name)

            self.assertIsNone(handler.stream)

    def test_shutdown_restores_original_settings(self):
        self.logger.setLevel(logging.ERROR)
        self.logger.propagate = True

        configure(
            name=self.name,
            console=True,
            console_level="DEBUG",
            propagate=False
        )

        self.assertEqual(self.logger.level, logging.DEBUG)
        self.assertFalse(self.logger.propagate)

        shutdown(self.name)

        self.assertEqual(self.logger.level, logging.ERROR)
        self.assertTrue(self.logger.propagate)
        
    def test_reconfigure_preserves_original_settings(self):
        self.logger.setLevel(logging.WARNING)
        self.logger.propagate = True

        configure(
            name=self.name,
            console_level="INFO",
            propagate=False
        )

        configure(
            name=self.name,
            console_level="DEBUG",
            propagate=False
        )

        shutdown(self.name)

        self.assertEqual(self.logger.level, logging.WARNING)
        self.assertTrue(self.logger.propagate)

if __name__ == "__main__":
    unittest.main()
