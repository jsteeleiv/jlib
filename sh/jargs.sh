#!/usr/bin/env bash

# 'include' guard
[ -n "${ARGS_SH_INC:-}" ] && return 0
ARGS_SH_INC=1

declare -ag ARGS_POS=()
declare -Ag ARGS_FLG=()
declare -Ag ARGS_OPT=()

args_reset() {
    ARGS_POS=()
    ARGS_FLG=()
    ARGS_OPT=()
}

_args_in_list() {
    local i="$1"
    shift

    local item
    for item in "$@"; do
        [ "$i" = "$item" ] && return 0
    done
    return 1
}

args_parse() {
    args_reset
    local arg key val rest i ch
    
    while [ $# -gt 0 ]; do
        arg="$1"
        shift
        case "$arg" in
            --)
                while [ $# -gt 0 ]; do
                    ARGS_POS+=("$1")
                    shift
                done ;;
            
            --*=*)
                # ${arg ... } parameter expansion | `%%`` → remove longest match from end
                key="${arg%%=*}"
                # `#`` → remove the shortest match from the start
                key="${key#--}"
                value="${arg#*=}"

                if _args_in_list "$key" "${ARG_VALS[@]}"; then
                    ARGS_OPT["$key"]="$value"
                elif _args_in_list "$key" "${ARG_FLAGS[@]}"; then
                    ARGS_FLG["$key"]=1
                else
                    echo "Error: unknown option '--$key'" >&2
                    return 1
                fi ;;
            
            --*)
                key="${arg#--}"

                if _args_in_list "$key" "${ARG_VALS[@]}"; then
                    ARGS_FLG["$key"]=1
                elif _args_in_list "$key" "${ARG_FLAGS[@]}"; then
                    if [ $# -eq 0 ]; then
                        echo "Error: option '--$key' requires a value" >&2
                        return 1
                    fi
                    ARGS_OPT["$key"]="$1"
                    shift
                else
                    echo "Error: unknown option '--$key'" >&2
                    return 1
                fi ;;

            -[!-]?*)
                rest="${arg#-}"
                for (( i=0; i<${#rest}; i++ )); do
                    ch="${rest:i:1}"

                    if _args_in_list "$ch" "${ARG_FLAGS[@]}"; then
                        ARGS_FLG["$ch"]=1
                    elif _args_in_list "$ch" "${ARG_VALS[@]}"; then
                        if [ $i -lt $(( ${#rest} - 1 )) ]; then
                            ARGS_OPT["$ch"]="${rest:i+1}"
                            break
                        elif [ $# -gt 0 ]; then
                            ARGS_OPT["$ch"]="$1"
                            shift
                            break
                        else
                            echo "Error: option '-$ch' requires a value" >&2
                            return 1
                        fi
                    else
                        echo "Error: unknown option '-$ch'" >&2
                        return 1
                    fi
                done ;;

            -?)
                key="${arg#-}"

                if _args_in_list "$key" "${ARG_FLAGS[@]}"; then
                    ARGS_FLG["$key"]=1
                elif _args_in_list "$key" "${ARG_VALS[@]}"; then
                    if [ $# -eq 0 ]; then
                        echo "Error: option '-$key' requires a value" >&2
                        return 1
                    fi
                    ARGS_OPT["$key"]="$1"
                    shift
                else
                    echo "Error: unknown option '-$key'" >&2
                    return 1
                fi ;;

            *)
                ARGS_POS+=("$arg")
                ;;
        esac
    done

}

args_has_flag() {
    local key="$1"
    [ "${ARGS_FLG[$key]:-0}" = "1" ]
}

args_has_opt() {
    local key="$1"
    [ -n "${ARGS_OPT[$key]+x}" ]
}

args_get_opt() {
    local key="$1"
    printf '%s\n' "${ARGS_OPT[$key]:-}"
}

args_pos_count() {
    printf '%s\n' "${#ARGS_POS[@]}"
}

args_pos_get() {
    local idx="$1"
    printf '%s\n' "${ARGS_POS[$idx]:-}"
}

args_need_pos() {
    local need="$1"
    if [ "${#ARGS_POS[@]}" -lt "$need" ]; then
        echo "Error: expected at least $need positional argument(s), got ${#ARGS_POS[@]}" >&2
        return 1
    fi
}

args_debug() {
    local k i

    echo "FLAGS:"
    for k in "${!ARGS_FLG[@]}"; do
        echo "  $k=1"
    done | sort

    echo "OPTIONS:"
    for k in "${!ARGS_OPT[@]}"; do
        echo "  $k=${ARGS_OPT[$k]}"
    done | sort

    echo "POSITIONAL:"
    for (( i=0; i<${#ARGS_POS[@]}; i++ )); do
        echo "  [$i]=${ARGS_POS[$i]}"
    done
}