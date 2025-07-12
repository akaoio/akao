#compdef akao

# Zsh completion for akao CLI

_akao() {
    local context state line
    typeset -A opt_args

    _arguments -C \
        '1: :_akao_commands' \
        '*::arg:->args' \
        '(--verbose -v)'{--verbose,-v}'[Print detailed logs]' \
        '(--quiet -q)'{--quiet,-q}'[Suppress all output]' \
        '(--debug -d)'{--debug,-d}'[Enable debug mode]' \
        '--json[Output in JSON format]' \
        '--yaml[Output in YAML format]' \
        '(--help -h)'{--help,-h}'[Show help message]'

    case $state in
        args)
            case $words[1] in
                list|install|uninstall|enable|disable|search|run)
                    _arguments \
                        '--node[Target nodes]:node:_akao_nodes' \
                        '--workflow[Target workflows]:workflow:_akao_workflows' \
                        '*::param:_files'
                    ;;
                version|status|validate|test)
                    # These commands don't take additional arguments
                    ;;
                *)
                    _message 'no more arguments'
                    ;;
            esac
            ;;
    esac
}

_akao_commands() {
    local commands=(
        'list:List nodes or workflows'
        'run:Execute a node or workflow'
        'install:Install external nodes or workflows'
        'uninstall:Uninstall nodes or workflows'
        'enable:Enable a node or workflow'
        'disable:Disable a node or workflow'
        'search:Search for nodes or workflows'
        'version:Show version information'
        'status:Show system status'
        'validate:Validate system configuration'
        'test:Run system tests'
    )
    _describe 'akao commands' commands
}

_akao_nodes() {
    local nodes=(
        'file:Filesystem scanner node'
        'logic:Logic evaluation node'
        'yaml:YAML processing node'
        'reporter:Reporting and output node'
    )
    _describe 'builtin nodes' nodes
}

_akao_workflows() {
    local workflows=(
        'validation-workflow:System validation workflow'
        'build-workflow:Build system workflow'
        'test-workflow:Testing workflow'
    )
    _describe 'available workflows' workflows
}

_akao "$@"