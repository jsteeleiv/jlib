#!/usr/bin/env bash

# jlib.sh ~ my [b|z|fi|ny]sh library for helpful data-structures, functions, etc.
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




# git/github related functionality
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




gh_urlswp() {
    local input_url="$1"
    local user repo rest

    # Map GitHub usernames to SSH config host aliases
    declare -A GH_ACCOUNTS=(
        [jsteeleiv]="gh-jsteeleiv"
        [cerebr4lz]="gh-cerebr4lz"
        [jsteele-ecpi]="gh-jsteele-ecpi"
        [cerebral-software]="gh-cerebral-software"
    )

    # Only handle SSH clone URLs like: git@github.com:username/repo.git
    if [[ "$input_url" =~ ^git@github\.com:(.+)$ ]]; then
        rest="${BASH_REMATCH[1]}"
        user="${rest%%/*}"

        if [[ -n "${GH_ACCOUNTS[$user]}" ]]; then
            printf 'git@%s:%s\n' "${GH_ACCOUNTS[$user]}" "$rest"
            return 0
        else
            printf 'Error: no SSH alias configured for GitHub user "%s"\n' "$user" >&2
            return 1
        fi
    else
        printf 'Error: expected SSH clone URL like git@github.com:user/repo.git\n' >&2
        return 1
    fi
}

gh_clone() {
    local url=""
    local depth=""

    while [[ $# -gt 0 ]]; do
        case "$1" in
            -d|--depth)
                depth="$2"
                shift 2
                ;;
            *)
                url="$1"
                shift
                ;;
        esac
    done

    [[ -z "$url" ]] && {
        echo "USAGE: gh_clone [-d depth] <git-ssh-url>" >&2
        return 1
    }

    local newurl="$(gh_urlswp "$url")" || return 1

    if [[ -n "$depth" ]]; then
        git clone --depth "$depth" "$newurl"
    else
        git clone "$newurl"
    fi
}