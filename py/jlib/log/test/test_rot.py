
"""
Unit tests for jlib.log.rotate.
"""

import logging
import tempfile
import unittest
from pathlib import Path
from logging.handlers import (
    RotatingFileHandler,
    TimedRotatingFileHandler
)

from jlib.log import sizehandler, timehandler


class TestRotation(unittest.TestCase):

    def make_record(self, message):
        return logging.LogRecord(
            "jlib.test",
            logging.INFO,
            __file__,
            10,
            message,
            (),
            None
        )

    def test_size_handler_type(self):
        with tempfile.TemporaryDirectory() as folder:
            handler = sizehandler(Path(folder) / "test.log")
            try:
                self.assertIsInstance(handler, RotatingFileHandler)
            finally:
                handler.close()

    def test_time_handler_type(self):
        with tempfile.TemporaryDirectory() as folder:
            handler = timehandler(Path(folder) / "test.log")
            try:
                self.assertIsInstance(handler, TimedRotatingFileHandler)
            finally:
                handler.close()

    def test_size_rotation(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "test.log"
            handler = sizehandler(
                path,
                max_bytes=100,
                backups=2,
                style="minimal"
            )

            try:
                for i in range(20):
                    handler.handle(self.make_record(f"Message {i:02d}"))
            finally:
                handler.close()

            self.assertTrue(path.exists())
            self.assertTrue(Path(str(path) + ".1").exists())

    def test_delayed_creation(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "test.log"
            handler = sizehandler(path)

            try:
                self.assertFalse(path.exists())
            finally:
                handler.close()

    def test_invalid_max_bytes(self):
        with self.assertRaises(ValueError):
            sizehandler("test.log", max_bytes=0)

    def test_invalid_backups(self):
        with self.assertRaises(ValueError):
            sizehandler("test.log", backups=0)

    def test_invalid_time_interval(self):
        with self.assertRaises(ValueError):
            timehandler("test.log", interval=0)

    def test_invalid_when(self):
        with self.assertRaises(ValueError):
            timehandler("test.log", when="YEARLY")

    def test_missing_directory(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "missing" / "test.log"

            with self.assertRaises(FileNotFoundError):
                sizehandler(path)

    def test_time_handler_settings(self):
        with tempfile.TemporaryDirectory() as folder:
            handler = timehandler(
                Path(folder) / "test.log",
                when="H",
                interval=2,
                backups=5,
                utc=True
            )

            try:
                self.assertEqual(handler.when, "H")
                self.assertEqual(handler.interval, 7200)
                self.assertEqual(handler.backupCount, 5)
                self.assertTrue(handler.utc)
            finally:
                handler.close()


if __name__ == "__main__":
    unittest.main()
