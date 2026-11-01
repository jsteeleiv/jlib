
"""
Unit tests for jlib.err.excp.

Run:
    python.exe -m unittest jlib.err.test.test_excp -v
"""

import unittest

from jlib.err import (
    JLibError,
    DependencyError,
    ConfigurationError,
    OperationError,
)


class TestExceptions(unittest.TestCase):

    def test_base_exception(self):
        self.assertTrue(issubclass(JLibError, Exception))

    def test_dependency_inheritance(self):
        self.assertTrue(issubclass(DependencyError, JLibError))
        self.assertTrue(issubclass(DependencyError, ImportError))

    def test_configuration_inheritance(self):
        self.assertTrue(issubclass(ConfigurationError, JLibError))
        self.assertTrue(issubclass(ConfigurationError, ValueError))

    def test_operation_inheritance(self):
        self.assertTrue(issubclass(OperationError, JLibError))

    def test_dependency_can_be_caught_as_importerror(self):
        with self.assertRaises(ImportError):
            raise DependencyError("Missing optional package")

    def test_configuration_can_be_caught_as_valueerror(self):
        with self.assertRaises(ValueError):
            raise ConfigurationError("Invalid configuration")

    def test_all_custom_errors_caught_by_base(self):
        for error_type in (
            DependencyError,
            ConfigurationError,
            OperationError
        ):
            with self.subTest(error_type=error_type):
                with self.assertRaises(JLibError):
                    raise error_type("Test failure")

    def test_error_message_preserved(self):
        error = OperationError("Operation failed")
        self.assertEqual(str(error), "Operation failed")


if __name__ == "__main__":
    unittest.main()
