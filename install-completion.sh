#!/bin/bash
# Installation script for akao CLI shell completion

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COMPLETIONS_DIR="$SCRIPT_DIR/completions"

# Function to install bash completion
install_bash_completion() {
    echo "Installing bash completion..."
    
    # Try system-wide installation first
    if [[ -d "/usr/local/share/bash-completion/completions" && -w "/usr/local/share/bash-completion/completions" ]]; then
        cp "$COMPLETIONS_DIR/akao-completion.bash" "/usr/local/share/bash-completion/completions/akao"
        echo "✅ Bash completion installed system-wide"
    elif [[ -d "/usr/share/bash-completion/completions" && -w "/usr/share/bash-completion/completions" ]]; then
        cp "$COMPLETIONS_DIR/akao-completion.bash" "/usr/share/bash-completion/completions/akao"
        echo "✅ Bash completion installed system-wide"
    else
        # Fall back to user installation
        mkdir -p "$HOME/.local/share/bash-completion/completions"
        cp "$COMPLETIONS_DIR/akao-completion.bash" "$HOME/.local/share/bash-completion/completions/akao"
        echo "✅ Bash completion installed for current user"
        echo "📝 Add this to your ~/.bashrc if not already present:"
        echo "    if [[ -d \$HOME/.local/share/bash-completion/completions ]]; then"
        echo "        for completion in \$HOME/.local/share/bash-completion/completions/*; do"
        echo "            [[ -r \$completion ]] && source \$completion"
        echo "        done"
        echo "    fi"
    fi
}

# Function to install zsh completion
install_zsh_completion() {
    echo "Installing zsh completion..."
    
    # Check if Oh My Zsh is installed
    if [[ -d "$HOME/.oh-my-zsh" ]]; then
        mkdir -p "$HOME/.oh-my-zsh/completions"
        cp "$COMPLETIONS_DIR/akao-completion.zsh" "$HOME/.oh-my-zsh/completions/_akao"
        echo "✅ Zsh completion installed for Oh My Zsh"
    else
        # Standard zsh installation
        if [[ -d "/usr/local/share/zsh/site-functions" && -w "/usr/local/share/zsh/site-functions" ]]; then
            cp "$COMPLETIONS_DIR/akao-completion.zsh" "/usr/local/share/zsh/site-functions/_akao"
            echo "✅ Zsh completion installed system-wide"
        else
            # User installation
            mkdir -p "$HOME/.local/share/zsh/site-functions"
            cp "$COMPLETIONS_DIR/akao-completion.zsh" "$HOME/.local/share/zsh/site-functions/_akao"
            echo "✅ Zsh completion installed for current user"
            echo "📝 Add this to your ~/.zshrc if not already present:"
            echo "    if [[ -d \$HOME/.local/share/zsh/site-functions ]]; then"
            echo "        fpath=(\$HOME/.local/share/zsh/site-functions \$fpath)"
            echo "        autoload -U compinit && compinit"
            echo "    fi"
        fi
    fi
}

# Main installation logic
case "${1:-all}" in
    bash)
        install_bash_completion
        ;;
    zsh)
        install_zsh_completion
        ;;
    all)
        install_bash_completion
        echo
        install_zsh_completion
        ;;
    *)
        echo "Usage: $0 [bash|zsh|all]"
        echo "  bash - Install bash completion only"
        echo "  zsh  - Install zsh completion only"
        echo "  all  - Install both (default)"
        exit 1
        ;;
esac

echo
echo "🎉 Installation complete!"
echo "   Restart your shell or run 'source ~/.bashrc' (bash) or 'source ~/.zshrc' (zsh)"
echo "   Then try: akao <TAB><TAB>"