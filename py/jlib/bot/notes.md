# thoughts

What makes something a bot, rather than simply a script, task, service, or application?
A bot generally has an identity, a purpose, an execution mechanism, and some degree of operational control.


## python mechanics to dive into
A few interesting Python mechanisms are already present:

| Feature | Purpose |
|---------|---------|
| `ABC` | Establishes an abstract-class foundation |
| `type(self) is Bot` | Prevents direct instantiation of `Bot` |
| `itertools.count()` | Generates sequential integer IDs |
| `@property` | Exposes read-only attributes |
| `type(self).__name__` | Discovers the actual subclass |
| `__repr__()` | Provides a useful debugging representation |

we're approaching a very natural use case for inheritance—and eventually __init_subclass__(), abstract classes, and metaclasses—without forcing artificial functionality into the root class.

We can use @property to expose these attributes as read-only, while allowing the ecosystem to initialize and update them through controlled mechanisms.

And __slots__ gives us an opportunity to explore Python's memory layout and attribute descriptors.

### Abstract base class (ABC)

### __slots__
`__slots__` is an interesting mechanism we'll explore; here it prevents ordinary instance dictionaries on direct instances of this class, though subclasses can introduce their own.

This is still technically instantiable, so we'd need to address that separately.

But I think we've uncovered a more fundamental distinction:

A bot's class describes what it can become. The ecosystem determines whether an instance is permitted to exist as an operational bot.

### ABCMeta — Python's abstract class machinery
Python uses a special metaclass called ABCMeta.
A metaclass is essentially a class that creates classes.
Ordinary classes create instances:
```py
bot = Worker()
```
Metaclasses participate in creating the classes themselves:
```py
class Worker(Bot):
    pass
```
ABCMeta only prevents instantiation when abstract methods or properties remain unimplemented.

### __new__() — Control instance creation
This is one of Python's special methods.

When you instantiate an object, Python generally calls:
1. __new__() to create the instance.
2. __init__() to initialize it.
We could use __new__() to prevent direct creation of Bot

### ABCMeta.__call__() — Intercept construction at the metaclass level
A metaclass can control what happens when a class is called to construct an instance.
```py
from abc import ABCMeta

class BotMeta(ABCMeta):

    def __call__(cls, *args, **kwargs):
        if cls is Bot:
            raise TypeError(
                "Bot cannot be instantiated directly."
            )

        return super().__call__(*args, **kwargs)


class Bot(metaclass=BotMeta):
    pass
```
This works because Bot() invokes the metaclass's __call__() machinery.

It also opens opportunities for more advanced features, such as automatic class registration and construction policies.


### 1. `@abstractmethod` — Required behavior

An abstract method establishes a contract: every concrete subclass must provide an implementation.

~~~python
from abc import ABC, abstractmethod


class Bot(ABC):
    @abstractmethod
    def identify(self):
        """Return the bot's identity."""
        pass


class Worker(Bot):
    def identify(self):
        return "Worker"
~~~

Now:

~~~python
Bot()       # TypeError
Worker()    # Works
~~~

If `Worker` doesn't implement `identify()`, it remains abstract and cannot be instantiated either.

Notice something interesting: abstractness is inherited until the requirement is satisfied.

Also, an abstract method can contain actual code. It doesn't have to be an empty `pass` statement.

### 2. `@abstractproperty` — Required attributes

This is where things become especially relevant to our design.

Historically, Python provided an `@abstractproperty` decorator, but it's deprecated.

The modern approach combines `@property` with `@abstractmethod`.

~~~python
from abc import ABC, abstractmethod


class Bot(ABC):
    @property
    @abstractmethod
    def id(self) -> int:
        """Every concrete bot must expose an ID."""
        ...
~~~

Notice the decorator order:

~~~python
@property
@abstractmethod
def id(self):
    ...
~~~

`@property` must be on the outside.

Now a subclass must implement `id` before it becomes concrete.

~~~python
class Worker(Bot):
    @property
    def id(self) -> int:
        return 42
~~~

This works, but here's where I would challenge its usefulness for JLIB.

Do we really want every subclass implementing its own identity property?

Probably not!
  

## What belongs in the framework?

My initial boundary would look like this:

| Framework responsibility | Bot-specific responsibility |
|--------------------------|-----------------------------|
| Execution lifecycle | Business logic |
| Cancellation and cleanup | Website selectors |
| Configuration contracts | API endpoints |
| Execution results | Data extraction rules |
| Optional state interfaces | Domain-specific state |
| Dependency integration | Authentication details |
| Dry-run mechanisms | Action-specific dry-run behavior |
| Common error conventions | Domain-specific exceptions |

One subtle distinction: the framework can support dry-run mode, but it cannot automatically guarantee that arbitrary bot actions are harmless.

Bot is the common ancestry. The ecosystem is what brings a particular bot into operational existence.

> Should a bot primarily be an object that does work, or an object that coordinates work?
...

A bot's role should not determine what qualifies it as a bot.

A bot might be a worker, a coordinator, a supervisor, or some combination of those. And interestingly, a bot that coordinates other bots could itself be coordinated by another bot.

| Concept | Responsibility | Example |
|---------|----------------|---------|
| Bot | Owns an automation objective and lifecycle | Website monitoring bot |
| Task | Represents an executable unit of work | Check website for changes |
| Action | Performs a concrete operation | Fetch HTML |
| Engine | Executes and manages work | Task execution, cancellation, results |
| Workflow | Defines relationships between tasks | Fetch → Parse → Compare → Notify |

A task might contain several actions, and a bot might manage several tasks.



# infrastructure


# workers

## Responsibility breakdown

| Module | Primary class | Responsibility |
|--------|---------------|----------------|
| `worker.py` | `Worker` | Persistent identity, lifecycle, capabilities, and work execution contract |
| `job.py` | `Job` | Represents a work package containing one or more tasks |
| `task.py` | `Task` | Represents an individual executable unit of work |
| `engine.py` | `Engine` | Provides the central runtime interface |
| `board.py` | `JobBoard` | Tracks jobs, tasks, assignments, and status |
| `supervisor.py` | `Supervisor` | Controls execution, validates assignments, and manages outcomes |
| `registry.py` | `Registry` | Maintains worker identities, capabilities, and availability |
| `dispatch.py` | `Dispatcher` | Specialized Worker responsible for selecting suitable workers |


# Conway's Game of Life implemented through JLIB's bot ecosystem
Each instance could represent a cell, with birth, survival, and death determined by its neighbors.

Even better, we could explore emergent behavior, state transitions, event propagation, and polymorphism.

One technical distinction: Conway's cells are usually much simpler than our bots, so we'd want to avoid burdening thousands of cells with unnecessary machinery.