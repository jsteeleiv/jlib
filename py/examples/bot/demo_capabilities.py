
"""Demonstrate capability-based Worker selection in JLIB."""

import tempfile

from email.message import Message
from io import BytesIO
from pathlib import Path
from unittest.mock import patch

from jlib.bot.arbiter import Arbiter
from jlib.bot.dispatcher import Dispatcher
from jlib.bot.forge import Forge
from jlib.bot.gauntlet import Gauntlet
from jlib.bot.governer import Governer
from jlib.bot.queue import TaskQueue
from jlib.bot.registry import Registry
from jlib.bot.task import Task

from jlib.bot.workers.scraper import Scraper
from jlib.bot.workers.archivist import Archivist


# ---------------------------------------------------------
# Demonstration Onboarding Context
# ---------------------------------------------------------

class DemoContext:
    """Provide initial knowledge and state for new Workers."""

    def capture_imprint(self, worker):
        return {
            "environment": "JLIB Capability Demo",
            "worker_type": type(worker).__name__,
        }

    def consult(self, worker, query=None):
        return {"knowledge": "Foundational JLIB information"}

    def create_state(self, worker, imprint):
        return {"status": "ready"}

    def create_experience(self, worker, imprint, knowledge):
        return []

    def create_disposition(
        self, worker, imprint, knowledge, experience
    ):
        return {}


# ---------------------------------------------------------
# Simulated HTTP Response
# ---------------------------------------------------------

class DemoResponse:
    """Provide an HTML response without external network access."""

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


# ---------------------------------------------------------
# Ecosystem Demonstration
# ---------------------------------------------------------

def main():
    forge = Forge()
    gauntlet = Gauntlet(DemoContext())
    governer = Governer(forge, gauntlet)

    registry = Registry()
    queue = TaskQueue()
    arbiter = Arbiter()

    governer.start()

    scraper = governer.create(Scraper)
    archivist = governer.create(Archivist)

    registry.register(scraper)
    registry.register(archivist)

    dispatcher = Dispatcher(
        100,
        registry,
        queue,
        arbiter,
    )

    # Temporary files provide real work for the Archivist.
    with tempfile.TemporaryDirectory() as directory:
        root = Path(directory)

        (root / "notes.txt").write_text(
            "JLIB is operational!",
            encoding="utf-8",
        )
        (root / "config.json").write_text(
            '{"version": 1}',
            encoding="utf-8",
        )

        html = b"""
        <html>
            <head><title>JLIB Documentation</title></head>
            <body>
                <a href="/guide">Guide</a>
                <a href="/api">API Reference</a>
            </body>
        </html>
        """

        response = DemoResponse(
            html,
            "https://example.com/",
        )

        # Inventory is queued first, but Scrape has higher priority.
        queue.put(Task(
            id=1,
            action="inventory",
            payload=root,
            priority=2,
        ))

        queue.put(Task(
            id=2,
            action="scrape",
            payload="https://example.com/",
            priority=0,
        ))

        print("\n--- JLIB CAPABILITY DEMO STARTED ---")
        print(f"Available Workers: {len(registry.available())}")
        print(f"Pending Tasks: {len(queue)}\n")

        with patch(
            "jlib.bot.workers.scraper.urlopen",
            return_value=response,
        ):
            while queue:
                task = queue.peek()
                assert task is not None

                print(
                    f"[Dispatcher] Selecting Worker for "
                    f"Task {task.id}: {task.action}"
                )

                result = dispatcher.dispatch()

                if result is None:
                    print("[Dispatcher] No eligible Worker available.")
                    break

                print(f"[Result] {result}\n")

    print("--- JLIB CAPABILITY DEMO COMPLETED ---")
    print(f"Remaining Tasks: {len(queue)}")
    print(f"Busy Workers: {len(registry.busy())}")

    governer.stop()


if __name__ == "__main__":
    main()

