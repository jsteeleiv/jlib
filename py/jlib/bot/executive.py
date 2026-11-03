from .worker import Worker


class Executive(Worker):
    """Base class for Workers exercising delegated authority."""

    __slots__ = ()