from abc import abstractmethod

from typing import Any

from .bot import Bot


class Worker(Bot):
    """
    Base class for bots capable of performing assigned tasks.

    Workers maintain an immutable initial imprint, evolving state,
    accumulated experience, and an adaptable disposition.

    The ecosystem controls initialization and task assignment.
    """

    __slots__ = (
        "_imprint",
        "_state",
        "_disposition",
        "_experience",
        "_context",
        "_initialized",
    )

    def __init__(self, id: int):
        """Construct a Worker without activating it."""
        super().__init__(id)

        self._imprint: Any | None = None
        self._state: Any | None = None
        self._disposition: Any | None = None
        self._experience: Any | None = None
        self._context: Any | None = None
        self._initialized: bool = False

    # ---------------------------------------------------------
    # Initialization
    # ---------------------------------------------------------

    def initialize(self, context: Any) -> None:
        """
        Perform one-time onboarding under ecosystem control.

        The context supplies authorized access to environmental
        information and the Corpus.
        """
        if self._initialized:
            raise RuntimeError("Worker has already been initialized.")

        if context is None:
            raise ValueError("Initialization context cannot be None.")

        # Capture environmental conditions before consulting
        # the Corpus or adapting the Worker's disposition.
        imprint = context.capture_imprint(self)

        # Obtain initial knowledge through the approved interface.
        knowledge = context.consult(self)

        # Build initial operational state from the original imprint.
        state = context.create_state(self, imprint)

        # Establish experience and disposition using the
        # Worker's initial conditions and shared knowledge.
        experience = context.create_experience(
            self, imprint, knowledge
        )

        disposition = context.create_disposition(
            self, imprint, knowledge, experience
        )

        # Commit onboarding only after all steps succeed.
        self._imprint = imprint
        self._state = state
        self._experience = experience
        self._disposition = disposition
        self._context = context
        self._initialized = True

    # ---------------------------------------------------------
    # Ecosystem Communication
    # ---------------------------------------------------------

    def request(self, resource: str, **kwargs: Any) -> Any:
        """Request an authorized ecosystem operation."""
        self._require_initialized()
        return self.context.request(self, resource, **kwargs)

    def consult(self, query: str) -> Any:
        """Retrieve authorized knowledge from the Corpus."""
        self._require_initialized()
        return self.context.consult(self, query)

    def contribute(self, knowledge: Any) -> Any:
        """Submit knowledge for possible inclusion in the Corpus."""
        self._require_initialized()
        return self.context.contribute(self, knowledge)

    # ---------------------------------------------------------
    # Task Execution
    # ---------------------------------------------------------

    def execute(self, task: Any) -> Any:
        """Execute a task through the standard Worker interface."""
        self._require_initialized()
        return self.work(task)

    def work(self, task: Any) -> Any:
        """
        Perform specialized work.

        Concrete subclasses may override this method.
        A generic Worker has no specialized task behavior.
        """
        raise NotImplementedError(
            f"{type(self).__name__} cannot execute this task."
        )

    # ---------------------------------------------------------
    # Properties
    # ---------------------------------------------------------

    @property
    def initialized(self) -> bool:
        """Whether the Worker has completed onboarding."""
        return self._initialized

    @property
    def imprint(self) -> Any:
        """Return the Worker's original environmental imprint."""
        return self._imprint

    @property
    def state(self) -> Any:
        """Return the Worker's current operational state."""
        return self._state

    @property
    def disposition(self) -> Any:
        """Return the Worker's current behavioral disposition."""
        return self._disposition

    @property
    def experience(self) -> Any:
        """Return the Worker's accumulated experience."""
        return self._experience

    # accomodate for the guarded property {None}
    @property
    def context(self) -> Any:
        """Return the initialized ecosystem context."""
        self._require_initialized()
        assert self._context is not None
        return self._context

    # ---------------------------------------------------------
    # Internal Utilities
    # ---------------------------------------------------------

    def _require_initialized(self) -> None:
        """Prevent operations before onboarding is complete."""
        if not self._initialized:
            raise RuntimeError(
                f"{type(self).__name__} has not been initialized."
            )
