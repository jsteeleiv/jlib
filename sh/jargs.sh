#!/usr/bin/env bash

# Usage:
# - create arrays for desired flags and options
#   ARG_FLAGS=(g global h help)
#   ARG_VALS=(n name e email d depth j justify s search)
# - run main parsing function to parse args
#   args_parse "$@" || exit 1
#
#   args_debug() will pretty print the captured values, sorted by arg type.  For testing.


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
    
    # while [ $# -gt 0 ]; do
    while (( $# )); do
        arg="$1" shift
        case "$arg" in

            # handle -- terminator (end of options, start of positionals)
            --) 
                #while [ $# -gt 0 ]; do
                while (( $# )); do ARGS_POS+=("$1") shift done ;;
            
            # handle key value pair options (--key=value)
            --*=*) 
                # ${arg ... } parameter expansion
                key="${arg%%=*}" val="${arg#*=}" key="${key#--}" 

                if _args_in_list "$key" "${ARG_VALS[@]}"; then
                    ARGS_OPT["$key"]="$val"
                elif _args_in_list "$key" "${ARG_FLAGS[@]}"; then
                    #ARGS_FLG["$key"]=1
                    printf "Error: flag '--%s' does not take a value\n" "$key" >&2
                    return 1
                else
                    printf "Error: unknown option '%s'" "$key" >&2
                    return 1
                fi ;;
            
            # handle bare long options (--key)
            --*) 
                key="${arg#--}"

                if _args_in_list "$key" "${ARG_VALS[@]}"; then
                    ARGS_FLG["$key"]=1
                elif _args_in_list "$key" "${ARG_FLAGS[@]}"; then
                    if [ $# -eq 0 ]; then
                        printf "Error: option '%s' requires a value" "$key" >&2
                        return 1
                    fi
                    ARGS_OPT["$key"]="$1"
                    shift
                else
                    printf "Error: unknown option '%s'" "$key" >&2
                    return 1
                fi ;;

            # handle short option clusters (-abc)
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
                            printf "Error: option '%s' requires a value" "$ch" >&2
                            return 1
                        fi
                    else
                        echo "Error: unknown option '%s'" "$ch" >&2
                        return 1
                    fi
                done ;;

            # handle single short options (-h)
            -?)
                key="${arg#-}"

                if _args_in_list "$key" "${ARG_FLAGS[@]}"; then
                    ARGS_FLG["$key"]=1
                elif _args_in_list "$key" "${ARG_VALS[@]}"; then
                    if [ $# -eq 0 ]; then
                        printf "Error: option '%s' requires a value" "$key" >&2
                        return 1
                    fi
                    ARGS_OPT["$key"]="$1"
                    shift
                else
                    echo "Error: unknown option '%s'" "$key" >&2
                    return 1
                fi ;;

            # catch everything else (positional arguments)
            *) 
                ARGS_POS+=("$arg") ;;
        esac
    done

}

# ~ Query Layer ~

args_has_flag() {
    local key="$1"
    #[ "${ARGS_FLG[$key]:-0}" = "1" ]
    [[ -v "ARGS_FLG[$key]" ]]
}

args_has_opt() {
    local key="$1"
    # [ -n "${ARGS_OPT[$key]+x}" ]
    [[ -v "ARGS_OPT[$key]" ]]
}

args_get_opt() {
    local key="$1"
    [[ -v "ARGS_OPT[$key]" ]] || return 1
    printf '%s\n' "${ARGS_OPT[$key]}"
}

args_get_opt_or_empty() {
    local key="$1"
    printf '%s\n' "${ARGS_OPT[$key]-}"
}

args_get_opt_or_default() {
    local key="$1"
    local default="$2"
    printf '%s\n' "${ARGS_OPT[$key]-$default}"
}

args_pos_count() {
    printf '%d\n' "${#ARGS_POS[@]}"
}

args_pos_get() {
    local idx="$1"
    [[ -v "ARGS_POS[$idx]" ]] || return 1
    # printf '%s\n' "${ARGS_POS[$idx]:-}"
    printf '%s\n' "${ARGS_POS[$idx]}"
}

# ~ Validation Layer ~

args_need_pos() {
    local need="$1"
    # if [ "${#ARGS_POS[@]}" -lt "$need" ]; then
    if (( ${#ARGS_POS[@]} < need )); then
        printf 'Error: expected at least %d positional argument(s), got %d\n' \
            "$need" "${#ARGS_POS[@]}" >&2
        return 1
    fi
    return 0
}

args_need_pos_exact() {
    local need="$1"

    if (( ${#ARGS_POS[@]} != need )); then
        printf 'Error: expected exactly %d positional argument(s), got %d\n' \
            "$need" "${#ARGS_POS[@]}" >&2
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