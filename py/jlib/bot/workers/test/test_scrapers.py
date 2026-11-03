
"""Unit tests for JLIB's web Scraper Worker."""

import unittest

from email.message import Message
from io import BytesIO
from unittest.mock import patch

from jlib.bot.task import Task
from jlib.bot.workers.scraper import Scraper


class MockResponse:
    """Minimal HTTP response supporting the Scraper interface."""

    def __init__(self, html: bytes, url: str):
        self._stream = BytesIO(html)
        self._url = url
        self.headers = Message()
        self.headers["Content-Type"] = "text/html; charset=utf-8"

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._stream.close()
        return False

    def read(self, size=-1):
        return self._stream.read(size)

    def geturl(self):
        return self._url


class TestScraper(unittest.TestCase):
    """Tests for HTML retrieval and extraction."""

    def setUp(self):
        """Create a Scraper with a minimal onboarding context."""
        from unittest.mock import Mock

        self.scraper = Scraper(1)

        context = Mock()
        context.capture_imprint.return_value = {}
        context.consult.return_value = {}
        context.create_state.return_value = {}
        context.create_experience.return_value = []
        context.create_disposition.return_value = {}

        self.scraper.initialize(context)

    def test_extract_title_and_links(self):
        """Scraper extracts page titles and resolves links."""
        html = b"""
        <html>
            <head><title>JLIB Test Page</title></head>
            <body>
                <a href="/docs">Documentation</a>
                <a href="https://example.org">External</a>
            </body>
        </html>
        """

        response = MockResponse(
            html,
            "https://example.com/",
        )

        with patch(
            "jlib.bot.workers.scraper.urlopen",
            return_value=response,
        ):
            result = self.scraper.execute(
                Task(1, "scrape", "https://example.com/")
            )

        self.assertEqual(result["title"], "JLIB Test Page")
        self.assertEqual(
            result["links"],
            [
                "https://example.com/docs",
                "https://example.org",
            ],
        )

    def test_empty_page(self):
        """Scraper handles HTML without titles or links."""
        response = MockResponse(
            b"<html><body>Hello</body></html>",
            "https://example.com/",
        )

        with patch(
            "jlib.bot.workers.scraper.urlopen",
            return_value=response,
        ):
            result = self.scraper.execute(
                Task(2, "scrape", "https://example.com/")
            )

        self.assertEqual(result["title"], "")
        self.assertEqual(result["links"], [])

    def test_invalid_scheme(self):
        """Scraper rejects unsupported URL schemes."""
        for url in (
            "file:///etc/passwd",
            "ftp://example.com/file",
        ):
            with self.subTest(url=url):
                with self.assertRaises(ValueError):
                    self.scraper.execute(Task(3, "scrape", url))

    def test_invalid_payload(self):
        """Scraper requires a URL string."""
        with self.assertRaises(TypeError):
            self.scraper.execute(Task(4, "scrape", 123))

    def test_wrong_action(self):
        """Scraper rejects Tasks outside its specialization."""
        with self.assertRaises(ValueError):
            self.scraper.execute(
                Task(5, "inventory", "https://example.com/")
            )

    def test_response_size_limit(self):
        """Scraper rejects responses exceeding its size limit."""
        response = MockResponse(
            b"x" * 1_000_001,
            "https://example.com/",
        )

        with patch(
            "jlib.bot.workers.scraper.urlopen",
            return_value=response,
        ):
            with self.assertRaises(ValueError):
                self.scraper.execute(
                    Task(6, "scrape", "https://example.com/")
                )


if __name__ == "__main__":
    unittest.main()
