#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DOMAIN="${DOMAIN:-}"
EMAIL="${EMAIL:-}"
KEY_FILE="${KEY_FILE:-$ROOT_DIR/.certs/prod-key.pem}"
CERT_FILE="${CERT_FILE:-$ROOT_DIR/.certs/prod-cert.pem}"
ACME_DIR="${ACME_DIR:-$ROOT_DIR/.acme.sh}"
WEBROOT="${WEBROOT:-$ROOT_DIR}"
MODE="${MODE:-standalone}"
RELOAD_CMD="${RELOAD_CMD:-}"

log() { printf '[akao-ssl] %s\n' "$1"; }
die() { printf '[akao-ssl] ERROR: %s\n' "$1" >&2; exit 1; }

show_help() {
    cat <<EOF
Usage:
  $0 --domain akao.io --email dev@akao.io [options]

Options:
  --domain DOMAIN         Primary domain for the certificate
  --email EMAIL           Let's Encrypt account email
  --key-file PATH         Output private key path (default: $ROOT_DIR/.certs/prod-key.pem)
  --cert-file PATH        Output fullchain cert path (default: $ROOT_DIR/.certs/prod-cert.pem)
  --acme-dir PATH         acme.sh home directory (default: $ROOT_DIR/.acme.sh)
  --webroot PATH          Webroot for HTTP validation (default: $ROOT_DIR)
  --standalone            Use standalone validation on port 80 (default)
  --webroot-mode          Use webroot validation against the running akao server
  --reload-cmd COMMAND    Command run after certificate install/renew
  -h, --help              Show this help

Examples:
  $0 --domain akao.io --email dev@akao.io --standalone
  $0 --domain akao.io --email dev@akao.io --webroot-mode --reload-cmd "systemctl restart akao"
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --domain) DOMAIN="$2"; shift 2 ;;
        --email) EMAIL="$2"; shift 2 ;;
        --key-file) KEY_FILE="$2"; shift 2 ;;
        --cert-file) CERT_FILE="$2"; shift 2 ;;
        --acme-dir) ACME_DIR="$2"; shift 2 ;;
        --webroot) WEBROOT="$2"; shift 2 ;;
        --reload-cmd) RELOAD_CMD="$2"; shift 2 ;;
        --standalone) MODE="standalone"; shift ;;
        --webroot-mode) MODE="webroot"; shift ;;
        -h|--help) show_help; exit 0 ;;
        *) die "Unknown option: $1" ;;
    esac
done

[[ -n "$DOMAIN" ]] || die "Missing --domain"
[[ -n "$EMAIL" ]] || die "Missing --email"
[[ "$KEY_FILE" = /* ]] || die "--key-file must be an absolute path"
[[ "$CERT_FILE" = /* ]] || die "--cert-file must be an absolute path"
[[ "$ACME_DIR" = /* ]] || die "--acme-dir must be an absolute path"
[[ "$WEBROOT" = /* ]] || die "--webroot must be an absolute path"

install_acme() {
    if [[ -x "$ACME_DIR/acme.sh" ]]; then
        log "Using existing acme.sh at $ACME_DIR"
        return
    fi

    log "Installing acme.sh into $ACME_DIR"
    local tmp_dir
    tmp_dir="$(mktemp -d)"
    trap 'rm -rf "$tmp_dir"' RETURN
    git clone --depth 1 https://github.com/acmesh-official/acme.sh.git "$tmp_dir/acme.sh" >/dev/null
    (
        cd "$tmp_dir/acme.sh"
        ./acme.sh --install --home "$ACME_DIR" --accountemail "$EMAIL"
    )
}

issue_cert() {
    if [[ "$MODE" == "standalone" ]]; then
        [[ $EUID -eq 0 ]] || die "Standalone mode on port 80 requires sudo/root. Use setup:prod or run this script with sudo."
        log "Issuing certificate for $DOMAIN with standalone HTTP challenge on port 80"
        "$ACME_DIR/acme.sh" --home "$ACME_DIR" --server letsencrypt --issue -d "$DOMAIN" --keylength ec-256 --standalone
        return
    fi

    log "Issuing certificate for $DOMAIN with webroot $WEBROOT"
    "$ACME_DIR/acme.sh" --home "$ACME_DIR" --server letsencrypt --issue -d "$DOMAIN" --keylength ec-256 -w "$WEBROOT"
}

install_cert() {
    mkdir -p "$(dirname "$KEY_FILE")" "$(dirname "$CERT_FILE")"

    local args=(
        --home "$ACME_DIR"
        --install-cert
        -d "$DOMAIN"
        --ecc
        --key-file "$KEY_FILE"
        --fullchain-file "$CERT_FILE"
    )
    if [[ -n "$RELOAD_CMD" ]]; then
        args+=(--reloadcmd "$RELOAD_CMD")
    fi

    log "Installing certificate to $CERT_FILE and key to $KEY_FILE"
    "$ACME_DIR/acme.sh" "${args[@]}"
}

install_acme
issue_cert
install_cert
log "Certificate setup complete"
