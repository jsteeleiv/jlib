
"""Web page retrieval and basic HTML inspection."""

from html.parser import HTMLParser
from urllib.parse import urljoin, urlsplit
from urllib.request import Request, urlopen

from ..work.task import Task
from ..worker import Worker


class _PageParser(HTMLParser):
    """Extract a page title and hyperlinks."""

    def __init__(self) -> None:
        super().__init__()
        self.title = ""
        self.links: list[str] = []
        self._in_title = False

    def handle_starttag(self, tag, attrs):
        if tag == "title":
            self._in_title = True

        elif tag == "a":
            attributes = dict(attrs)
            href = attributes.get("href")

            if href:
                self.links.append(href)

    def handle_endtag(self, tag):
        if tag == "title":
            self._in_title = False

    def handle_data(self, data):
        if self._in_title:
            self.title += data


class Scraper(Worker):
    """Retrieve and inspect an authorized HTML web page."""

    __slots__ = ()

    capabilities = frozenset({"scrape"})

    def work(self, task: Task) -> dict:
        """Retrieve a web page and extract basic information."""
        if task.action != "scrape":
            raise ValueError("Scraper requires a scrape task.")

        if not isinstance(task.payload, str):
            raise TypeError("Scraper requires a URL string.")

        url = task.payload
        parsed = urlsplit(url)

        if parsed.scheme not in ("http", "https"):
            raise ValueError("Only HTTP and HTTPS URLs are supported.")

        if not parsed.hostname:
            raise ValueError("A valid hostname is required.")

        request = Request(
            url,
            headers={"User-Agent": "JLIB-Scraper/0.1"},
        )

        with urlopen(request, timeout=10) as response:
            final_url = response.geturl()

            if urlsplit(final_url).scheme not in ("http", "https"):
                raise ValueError("Unsupported redirect destination.")

            content = response.read(1_000_001)

            if len(content) > 1_000_000:
                raise ValueError("Page exceeds the 1 MB limit.")

            charset = response.headers.get_content_charset() or "utf-8"

        parser = _PageParser()
        parser.feed(content.decode(charset, errors="replace"))

        return {
            "url": final_url,
            "title": parser.title.strip(),
            "links": [
                urljoin(final_url, link)
                for link in parser.links
            ],
        }
