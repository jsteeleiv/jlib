
"""
Unit tests for jlib.err.fmt.

Run:
    python.exe -m unittest jlib.err.test.test_fmt -v
"""

import unittest

from jlib.err import (
    format_error,
    error_details,
    format_traceback,
)


class TestErrorFormatting(unittest.TestCase):

    def test_simple_error(self):
        result = format_error(ValueError("Invalid input"))

        self.assertEqual(result, "ValueError: Invalid input")

    def test_error_without_message(self):
        result = format_error(ValueError())

        self.assertEqual(result, "ValueError")

    def test_include_module(self):
        result = format_error(
            ValueError("Invalid input"),
            include_module=True
        )

        self.assertEqual(
            result,
            "builtins.ValueError: Invalid input"
        )

    def test_invalid_error_object(self):
        for value in (None, "error", 123):
            with self.subTest(value=value):
                with self.assertRaises(TypeError):
                    format_error(value)

    def test_error_details(self):
        error = ValueError("Bad value")

        details = error_details(error)

        self.assertEqual(details["type"], "ValueError")
        self.assertEqual(details["module"], "builtins")
        self.assertEqual(details["message"], "Bad value")
        self.assertEqual(details["args"], ("Bad value",))

    def test_details_without_traceback(self):
        details = error_details(ValueError("Test"))

        self.assertNotIn("traceback", details)

    def test_details_with_traceback(self):
        try:
            raise ValueError("Traceback test")
        except ValueError as error:
            details = error_details(
                error,
                include_traceback=True
            )

        self.assertIn("traceback", details)
        self.assertIn("ValueError", details["traceback"])
        self.assertIn("Traceback test", details["traceback"])

    def test_format_traceback(self):
        try:
            raise RuntimeError("Something failed")
        except RuntimeError as error:
            result = format_traceback(error)

        self.assertIn("RuntimeError", result)
        self.assertIn("Something failed", result)
        self.assertIn("test_format_traceback", result)

    def test_explicit_exception_chaining(self):
        try:
            try:
                raise ValueError("Original failure")
            except ValueError as original:
                raise RuntimeError("Wrapped failure") from original

        except RuntimeError as error:
            details = error_details(error)
            formatted = format_traceback(error)

        self.assertEqual(
            details["cause"],
            "ValueError: Original failure"
        )

        self.assertIn("Original failure", formatted)
        self.assertIn("Wrapped failure", formatted)

    def test_traceback_without_chain(self):
        try:
            try:
                raise ValueError("Original failure")
            except ValueError as original:
                raise RuntimeError("Wrapped failure") from original

        except RuntimeError as error:
            result = format_traceback(error, chain=False)

        self.assertIn("Wrapped failure", result)
        self.assertNotIn("Original failure", result)

    def test_context_suppression(self):
        try:
            try:
                raise ValueError("Hidden context")
            except ValueError:
                raise RuntimeError("Visible failure") from None

        except RuntimeError as error:
            details = error_details(error)
            formatted = format_traceback(error)

        self.assertTrue(details["suppress_context"])
        self.assertIsNotNone(details["context"])

        self.assertIn("Visible failure", formatted)
        self.assertNotIn("Hidden context", formatted)

    def test_baseexception_support(self):
        result = format_error(KeyboardInterrupt())

        self.assertEqual(result, "KeyboardInterrupt")


if __name__ == "__main__":
    unittest.main()
