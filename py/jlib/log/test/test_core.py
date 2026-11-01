
"""
Unit tests for jlib.log.core.
"""

import logging
import unittest

from jlib.log import getlogger


class TestLoggerCore(unittest.TestCase):

    def test_returns_logger(self):
        logger = getlogger("jlib.test")
        self.assertIsInstance(logger, logging.Logger)

    def test_logger_name(self):
        logger = getlogger("jlib.test")
        self.assertEqual(logger.name, "jlib.test")

    def test_same_logger_instance(self):
        first = getlogger("jlib.test")
        second = getlogger("jlib.test")
        self.assertIs(first, second)

    def test_default_logger(self):
        logger = getlogger()
        self.assertEqual(logger.name, "jlib")

    def test_invalid_logger_name(self):
        with self.assertRaises(ValueError):
            getlogger("")

        with self.assertRaises(ValueError):
            getlogger("None")

    def test_no_handlers_added(self):
        logger = getlogger("jlib.test.nohandlers")
        self.assertEqual(logger.handlers, [])


if __name__ == "__main__":
    unittest.main()
