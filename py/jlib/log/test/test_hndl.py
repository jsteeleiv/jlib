
"""
Unit tests for jlib.log.handlers.

Run from project root:
python.exe -m unittest jlib.log.test.test_handlers -v
"""

import io
import logging
import tempfile
import unittest
from pathlib import Path

from jlib.log import consolehandler, filehandler


class TestHandlers(unittest.TestCase):

    def test_console_handler_type(self):
        handler = consolehandler()
        self.assertIsInstance(handler, logging.StreamHandler)
        handler.close()

    def test_console_output(self):
        stream = io.StringIO()
        handler = consolehandler(stream=stream)

        record = logging.LogRecord(
            "jlib.test",
            logging.WARNING,
            __file__,
            10,
            "Console test message",
            (),
            None
        )

        try:
            handler.handle(record)
            self.assertIn("Console test message", stream.getvalue())
        finally:
            handler.close()

    def test_console_level(self):
        handler = consolehandler(level="WARNING")

        try:
            self.assertEqual(handler.level, logging.WARNING)
        finally:
            handler.close()

    def test_file_handler_type(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "test.log"
            handler = filehandler(path)

            try:
                self.assertIsInstance(handler, logging.FileHandler)
            finally:
                handler.close()

    def test_delayed_file_creation(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "delayed.log"
            handler = filehandler(path)

            try:
                self.assertFalse(path.exists())
            finally:
                handler.close()

    def test_file_output(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "output.log"
            handler = filehandler(path)

            record = logging.LogRecord(
                "jlib.test",
                logging.INFO,
                __file__,
                10,
                "File test message",
                (),
                None
            )

            try:
                handler.handle(record)
            finally:
                handler.close()

            self.assertTrue(path.exists())
            self.assertIn(
                "File test message",
                path.read_text(encoding="utf-8")
            )

    def test_invalid_file_mode(self):
        with self.assertRaises(ValueError):
            filehandler("test.log", mode="x")

    def test_invalid_level(self):
        with self.assertRaises(ValueError):
            consolehandler(level="NOT_A_LEVEL")

    def test_missing_parent_directory(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "missing" / "test.log"

            with self.assertRaises(FileNotFoundError):
                filehandler(path)

    def test_invalid_formatter(self):
        with self.assertRaises(ValueError):
            consolehandler(style="nonexistent")


if __name__ == "__main__":
    unittest.main()
