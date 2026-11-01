
"""
Unit tests for jlib.log.fmt.
"""

import logging
import unittest

from jlib.log import getformatter


class TestFormatter(unittest.TestCase):

    def setUp(self):
        self.record = logging.LogRecord(
            name="jlib.dirt",
            level=logging.INFO,
            pathname="scan.py",
            lineno=42,
            msg="Directory scan started",
            args=(),
            exc_info=None
        )
        self.record.created = 0

    def test_returns_formatter(self):
        formatter = getformatter()
        self.assertIsInstance(formatter, logging.Formatter)

    def test_minimal_format(self):
        formatter = getformatter("minimal")
        self.assertEqual(
            formatter.format(self.record),
            "Directory scan started"
        )

    def test_simple_format(self):
        formatter = getformatter("simple")
        self.assertEqual(
            formatter.format(self.record),
            "INFO | Directory scan started"
        )

    def test_detailed_format(self):
        formatter = getformatter("detailed")

        result = formatter.format(self.record)

        self.assertIn("INFO", result)
        self.assertIn("jlib.dirt", result)
        self.assertIn("Directory scan started", result)

    def test_debug_format(self):
        formatter = getformatter("debug")

        result = formatter.format(self.record)

        self.assertIn("scan.py:42", result)

    def test_invalid_style(self):
        with self.assertRaises(ValueError):
            getformatter("unknown")

    def test_case_insensitive_style(self):
        formatter = getformatter("SIMPLE")

        self.assertEqual(
            formatter.format(self.record),
            "INFO | Directory scan started"
        )

    def test_custom_date_format(self):
        formatter = getformatter(
            "detailed",
            datefmt="%Y"
        )

        result = formatter.format(self.record)

        #self.assertTrue(result.startswith("1970 |"))
        
        from datetime import datetime

        expected_year = datetime.fromtimestamp(0).strftime("%Y")
        self.assertTrue(result.startswith(f"{expected_year} |"))


if __name__ == "__main__":
    unittest.main()
