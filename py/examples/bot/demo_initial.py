
"""Demonstrate the JLIB bot ecosystem's first task assignments."""

from py.jlib.bot.core.arbiter import Arbiter
from py.jlib.bot.execs.dispatcher import Dispatcher
from py.jlib.bot.core.forge import Forge
from py.jlib.bot.core.gauntlet import Gauntlet
from py.jlib.bot.core.governer import Governer
from py.jlib.bot.work.queue import TaskQueue
from py.jlib.bot.core.registry import Registry
from py.jlib.bot.work.task import Task
from jlib.bot.worker import Worker


# ---------------------------------------------------------
# Temporary Onboarding Context
# ---------------------------------------------------------

class DemoContext:
    """Provide foundational onboarding without a real Corpus."""

    def capture_imprint(self, worker):
        return {
            "environment": "JLIB Demo",
            "worker_type": type(worker).__name__,
        }

    def consult(self, worker, query=None):
        return {"knowledge": "Foundational information"}

    def create_state(self, worker, imprint):
        return {"status": "ready"}

    def create_experience(self, worker, imprint, knowledge):
        return []

    def create_disposition(self, worker, imprint, knowledge, experience):
        return {"confidence": 0.5}


# ---------------------------------------------------------
# Specialized Workers
# ---------------------------------------------------------

class Inspector(Worker):
    """Worker specialized in inspection."""

    def work(self, task):
        print(f"[Inspector {self.id}] Inspecting: {task.payload}")
        return f"Inspection complete: {task.payload}"


class Builder(Worker):
    """Worker specialized in construction."""

    def work(self, task):
        print(f"[Builder {self.id}] Building: {task.payload}")
        return f"Construction complete: {task.payload}"


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

    inspector = governer.create(Inspector)
    builder = governer.create(Builder)

    registry.register(inspector)
    registry.register(builder)

    dispatcher = Dispatcher(
        100,
        registry,
        queue,
        arbiter,
    )

    queue.put(Task(1, "inspect", payload="Bridge A", priority=0))
    queue.put(Task(2, "build", payload="Bridge B", priority=1))

    print("\n--- JLIB ECOSYSTEM STARTED ---")

    while queue:
        result = dispatcher.dispatch()
        print(f"[Dispatcher] {result}")

    print("\n--- ECOSYSTEM STOPPED ---")

    governer.stop()


if __name__ == "__main__":
    main()
