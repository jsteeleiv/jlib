




# ╭──────────────────────────────────────────────────────────╮
# >    NVIDIA API CODE                                       │
# ╰──────────────────────────────────────────────────────────╯

# check for key or get from user, then  add to env (temporarily!)
def nvidia_get_api():
    from os import environ
    from getpass import getpass

    if os.environ.get("NVIDIA_API_KEY", "").startswith("nvapi-"):
        print("Valid NVIDIA_API_KEY already in environment. Delete to reset")
    else:
        nvapi_key = getpass.getpass("NVAPI Key (starts with nvapi-): ")
        assert nvapi_key.startswith(
            "nvapi-"
        ), f"{nvapi_key[:5]}... is not a valid key"
        os.environ["NVIDIA_API_KEY"] = nvapi_key
