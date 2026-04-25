





# get secrets
def get_secret( name: str, prefix: str | None = None,
    prompt: str | None = None, persist: bool = False,) -> str:

    from os import environ
    from getpass import getpass

    value = environ.get(name, "")

    # fast path: exists and valid
    if value and (prefix is None or value.startswith(prefix)):
        return value

    # prompt user
    prompt_text = prompt or f"{name}: "
    value = getpass(prompt_text)

    # validate
    if prefix and not value.startswith(prefix):
        raise ValueError(f"Expected {name} to start with '{prefix}'")

    # store temporarily
    environ[name] = value

    # optional persistence
    if persist:
        _persist_to_env_file(name, value)

    return value

def _persist_to_env_file(name: str, value: str, path: str = ".env"):
    line = f'{name}="{value}"\n'

    try:
        with open(path, "a+") as f:
            f.seek(0)
            if name not in f.read():
                f.write(line)
    except OSError as e:
        print(f"[warn] Could not persist {name}: {e}")