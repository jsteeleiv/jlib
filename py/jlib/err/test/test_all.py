
"""
Integration tests for jlib.err.

Run:
    python.exe -m unittest jlib.err.test.test_integration -v
"""

import subprocess
import sys
import unittest

from unittest.mock import patch

from jlib.err import (
    JLibError,
    DependencyError,
    require,
    available,
    error_details,
    format_error,
    format_traceback,
)


class TestErrorIntegration(unittest.TestCase):

    def test_dependency_error_hierarchy(self):
        with self.assertRaises(JLibError):
            require("jlib_nonexistent_dependency_98765")

    def test_dependency_error_is_importerror(self):
        with self.assertRaises(ImportError):
            require("jlib_nonexistent_dependency_98765")

    def test_dependency_error_preserves_cause(self):
        try:
            require("jlib_nonexistent_dependency_98765")
        except DependencyError as error:
            self.assertIsInstance(
                error.__cause__,
                ModuleNotFoundError
            )

            details = error_details(error)

            self.assertIn(
                "ModuleNotFoundError",
                details["cause"]
            )
        else:
            self.fail("Expected DependencyError")

    def test_formatted_dependency_error(self):
        try:
            require("jlib_nonexistent_dependency_98765")
        except DependencyError as error:
            result = format_error(error)

        self.assertIn("DependencyError", result)
        self.assertIn("python -m pip install", result)

    def test_traceback_preserves_chain(self):
        try:
            require("jlib_nonexistent_dependency_98765")
        except DependencyError as error:
            result = format_traceback(error)

        self.assertIn("ModuleNotFoundError", result)
        self.assertIn("DependencyError", result)

    def test_available_does_not_require_dependency(self):
        self.assertFalse(
            available("jlib_nonexistent_dependency_98765")
        )

    def test_formatting_does_not_print(self):
        with patch("builtins.print") as mock_print:
            format_error(ValueError("Test"))
            error_details(ValueError("Test"))
            format_traceback(ValueError("Test"))

        mock_print.assert_not_called()

    def test_import_safety(self):
        """
        Import jlib.err in a fresh Python interpreter.

        Verifies no console output and no eager import
        of jlib.log or optional third-party dependencies.
        """
        code = """
import sys
import jlib.err

assert "jlib.log" not in sys.modules
assert "pandas" not in sys.modules
assert "matplotlib" not in sys.modules
"""

        result = subprocess.run(
            [sys.executable, "-B", "-c", code],
            capture_output=True,
            text=True,
            timeout=10
        )

        self.assertEqual(
            result.returncode,
            0,
            msg=result.stderr
        )

        self.assertEqual(result.stdout, "")
        self.assertEqual(result.stderr, "")


if __name__ == "__main__":
    unittest.main()
