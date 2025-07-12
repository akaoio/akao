#!/bin/bash
# Bash completion for akao CLI

_akao_completion() {
    local cur prev words cword
    _init_completion || return

    # Define all available commands and flags
    local commands="list run install uninstall enable disable search version status validate test"
    local target_flags="--node --workflow"
    local global_flags="--verbose --quiet --debug --json --yaml --help"
    local builtin_nodes="file logic yaml reporter"
    local sample_workflows="validation-workflow build-workflow test-workflow"

    # Handle different completion contexts
    case $prev in
        akao)
            COMPREPLY=($(compgen -W "$commands" -- "$cur"))
            return 0
            ;;
        list|install|uninstall|enable|disable|search)
            COMPREPLY=($(compgen -W "$target_flags" -- "$cur"))
            return 0
            ;;
        run)
            COMPREPLY=($(compgen -W "$target_flags" -- "$cur"))
            return 0
            ;;
        --node)
            case ${words[1]} in
                list|enable|disable|search)
                    # For list, enable, disable, search - suggest builtin nodes
                    COMPREPLY=($(compgen -W "$builtin_nodes" -- "$cur"))
                    ;;
                run)
                    # For run - suggest builtin nodes
                    COMPREPLY=($(compgen -W "$builtin_nodes" -- "$cur"))
                    ;;
                install|uninstall)
                    # For install/uninstall - complete filenames and directories
                    _filedir
                    ;;
            esac
            return 0
            ;;
        --workflow)
            case ${words[1]} in
                list|enable|disable|search)
                    # For list, enable, disable, search - suggest sample workflows
                    COMPREPLY=($(compgen -W "$sample_workflows" -- "$cur"))
                    ;;
                run)
                    # For run - suggest sample workflows
                    COMPREPLY=($(compgen -W "$sample_workflows" -- "$cur"))
                    ;;
                install|uninstall)
                    # For install/uninstall - complete filenames
                    _filedir
                    ;;
            esac
            return 0
            ;;
    esac

    # Handle parameter separator
    if [[ $cur == "--" ]]; then
        COMPREPLY=("--")
        return 0
    fi

    # If we haven't matched a specific context, suggest global flags
    if [[ $cur == -* ]]; then
        COMPREPLY=($(compgen -W "$global_flags" -- "$cur"))
        return 0
    fi

    # Default completion
    COMPREPLY=($(compgen -W "$commands $global_flags" -- "$cur"))
}

# Register the completion function
complete -F _akao_completion akao