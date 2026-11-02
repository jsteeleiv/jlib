


py/
└── jlib/
    ├── __init__.py
    ├── log/
    ├── err/
    │
    └── bots/
        ├── __init__.py
        │
        ├── worker.py       # Base Worker abstraction
        ├── job.py          # Jobs and their constituent tasks
        ├── task.py         # Individual executable work units
        │
        ├── engine.py       # Runtime facade and orchestration
        ├── board.py        # Job board and work tracking
        ├── supervisor.py   # Execution control and enforcement
        ├── registry.py     # Worker registration and discovery
        ├── dispatch.py     # Dispatcher worker and assignment logic
        │
        ├── README.md
        │
        └── test/
            ├── __init__.py
            ├── test_worker.py
            ├── test_job.py
            ├── test_task.py
            ├── test_engine.py
            ├── test_board.py
            ├── test_supervisor.py
            ├── test_registry.py
            └── test_dispatch.py