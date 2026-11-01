
"""
Unit tests for jlib.err.deps.

Run:
    python.exe -m unittest jlib.err.test.test_deps -v
"""

import sys
import types
import unittest

from unittest.mock import patch

from jlib.err import (
    DependencyError,
    require,
    available,
    check,
)


class TestDependencies(unittest.TestCase):

    def test_require_standard_library(self):
        module = require("json")

        self.assertIs(module, sys.modules["json"])

    def test_require_missing_dependency(self):
        with self.assertRaises(DependencyError) as context:
            require("jlib_nonexistent_package_12345")

        self.assertIn(
            "python -m pip install",
            str(context.exception)
        )

    def test_custom_install_package(self):
        with self.assertRaises(DependencyError) as context:
            require(
                "jlib_missing_image_module_12345",
                package="Pillow"
            )

        self.assertIn("Pillow", str(context.exception))

    def test_invalid_module_name(self):
        for name in ("", "bad-name", "bad module", None):
            with self.subTest(name=name):
                with self.assertRaises(ValueError):
                    require(name)

    def test_invalid_package_name(self):
        with self.assertRaises(ValueError):
            require("json", package="")

    def test_available_standard_library(self):
        self.assertTrue(available("json"))

    def test_available_missing_module(self):
        self.assertFalse(
            available("jlib_nonexistent_package_12345")
        )

    def test_check_multiple_modules(self):
        results = check([
            "json",
            "jlib_nonexistent_package_12345"
        ])

        self.assertEqual(results, {
            "json": True,
            "jlib_nonexistent_package_12345": False
        })

    def test_internal_module_not_found_preserved(self):
        """
        A package that fails because of an unrelated missing
        dependency must not be misidentified as absent.
        """
        missing = ModuleNotFoundError(
            "No module named 'internal_dependency'",
            name="internal_dependency"
        )

        with patch(
            "importlib.import_module",
            side_effect=missing
        ):
            with self.assertRaises(ModuleNotFoundError) as context:
                require("some_installed_package")

        self.assertIs(context.exception, missing)

    def test_internal_import_error_preserved(self):
        """
        A generic internal ImportError must propagate unchanged.
        """
        original = ImportError("Broken internal import")

        with patch(
            "importlib.import_module",
            side_effect=original
        ):
            with self.assertRaises(ImportError) as context:
                require("some_installed_package")

        self.assertIs(context.exception, original)

    def test_missing_submodule(self):
        """
        A missing requested submodule should produce DependencyError.
        """
        missing = ModuleNotFoundError(
            "No module named 'example.submodule'",
            name="example.submodule"
        )

        with patch(
            "importlib.import_module",
            side_effect=missing
        ):
            with self.assertRaises(DependencyError):
                require("example.submodule")

    def test_error_chaining(self):
        with self.assertRaises(DependencyError) as context:
            require("jlib_nonexistent_package_12345")

        self.assertIsInstance(
            context.exception.__cause__,
            ModuleNotFoundError
        )


if __name__ == "__main__":
    unittest.main()
