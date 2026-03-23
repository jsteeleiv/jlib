# clipboard detection logic
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#CLIPBOARD_WRITE_CMDS=
declare -A CLIPCOPY=(
    [pbcopy]="pbcopy"    # macOS
    [wl-copy]="wl-copy"    # Linux (Wayland)
    [xclip]="xclip -selection clipboard"    # Linux (X11)
    [xsel]="xsel --clipboard --input"    # Linux (X11)
    [powershell]="powershell.exe -NoProfile -Command Set-Clipboard"    # Windows / WSL
    [clip]="clip.exe"
    [termux]="termux-clipboard-set" # termux
    [win32yank]="win32yank -i --crlf"    # win32yank
)
#CLIPBOARD_READ_CMDS=
declare -A CLIPPASTE=(
    [pbpaste]="pbpaste"     #macos
    [wl-paste]="wl-paste"    #linux (wayland)
    [xclip]="xclip -selection clipboard -o"    #linux (x11)
    [xsel]="xsel --clipboard --output"
    #windows / WSL / Git Bash
    [powershell]="powershell.exe -NoProfile -Command Get-Clipboard | tr -d '\r'"
    [win32yank]="win32yank -o --lf"
    [termux]="termux-clipboard-get"    #termux (android)
    [nvim-win32yank]="win32yank -o --lf"    # neovim ecosystem (sometimes bundled)
    [gtk]="gtk-launch clipboard-get"     # fallbacks / edge tools
)
#CLIPBOARD_PRIORITY=
CLIPLIST_P=( pbpaste wl-paste xclip xsel powershell win32yank termux )
CLIPLIST_C=( pbcopy wl-copy xclip xsel clip powershell win32yank termux )

clipboard_detect() {
    local tool bin flag
    while [[ "$#" -gt 0 ]]; do
        case "$1" in
            -c|--copy) flag=0 ;;
            -p|--paste) flag=1 ;;
            *)
                printf 'Error: expected <copy|paste>' >&2
                return 1
        esac
    done

    if [[ $flag -gt 0 ]]; then
        for tool in "${CLIPLIST_P[@]}"; do
            bin="${CLIPPASTE[$tool]}"
            command -v "$bin" >/dev/null 2>&1 && {
                echo "$tool"
                return 0
            } done
    else
        for tool in "${CLIPLIST_C[@]}"; do
            bin="${CLIPCOPY[$tool]}"
            command -v "$bin" >/dev/null 2>&1 && {
                echo "$tool"
                return 0
            } done
    fi
}

# clipboard usage
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

getclip() {
    local tool
    tool="$(clipboard_detect)" || {
        echo "[error] no clipboard tool found" >&2
        return 1
    }

    local=cmd()

    case "$tool" in 
        pbpaste)    cmd=(pbpaste) ;;
        wl-paste)   cmd=(wl-paste) ;;
        xclip)      cmd=(xclip -selection clipboard -o) ;;
        xsel)       cmd=(xsel --clipboard --output) ;;
        powershell) cmd=(powershell.exe -NoProfile -Command Get-Clipboard) ;;
        win32yank)  cmd=(win32yank -o --lf) ;;
        termux)     cmd=(termux-clipboard-get) ;;
        *)
            printf 'Unsupported clipboard tool: %s\n' "$tool" >&2
            return 1
            ;;
    esac
    #eval "${CLIPPASTE[$tool]}"
    "${cmd[@]}" | tr -d '\r'
}

setclip() {
    local tool
    tool="$(clipboard_detect)" || {
        echo "No clipboard tool found" >&2
        return 1
    }
    local=cmd()

    case "$tool" in 
        pbcopy)     cmd=(pbcopy) ;;
        wl-copy)    cmd=(wl-copy) ;;
        xclip)      cmd=(xclip -selection clipboard) ;;
        xsel)       cmd=(xsel --clipboard --input) ;;
        powershell) cmd=(powershell.exe -NoProfile -Command Set-Clipboard) ;;
        clip)       cmd=(clip.exe)
        win32yank)  cmd=(win32yank -i --crlf) ;;
        termux)     cmd=(termux-clipboard-set) ;;
        *)
            printf 'Unsupported clipboard tool: %s\n' "$tool" >&2
            return 1
            ;;
    esac
    #eval "${CLIPCOPY[$tool]}"
    "${cmd[@]}" | tr -d '\r'
    
}
