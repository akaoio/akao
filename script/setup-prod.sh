#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DOMAIN="${DOMAIN:-}"
EMAIL="${EMAIL:-}"
SERVICE_NAME="${SERVICE_NAME:-akao}"
RUN_USER="${RUN_USER:-$(whoami)}"
SUDO_KEEPALIVE_PID=""
CHALLENGE_SERVER_PID=""

show_help() {
    cat <<EOF
Usage:
  $0 --domain akao.io --email dev@akao.io [options]

Options:
  --domain DOMAIN         Domain for TLS issuance
  --email EMAIL           Let's Encrypt account email
  --service NAME          Systemd service name (default: akao)
  --user USER             Service user (default: current user)
  -h, --help              Show this help

This script:
  1. Builds akao
  2. Issues a Let's Encrypt certificate into .certs/
  3. Installs and restarts a systemd service for prod.js
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --domain) DOMAIN="$2"; shift 2 ;;
        --email) EMAIL="$2"; shift 2 ;;
        --service) SERVICE_NAME="$2"; shift 2 ;;
        --user) RUN_USER="$2"; shift 2 ;;
        -h|--help) show_help; exit 0 ;;
        *) printf 'Unknown option: %s\n' "$1" >&2; exit 1 ;;
    esac
done

[[ -n "$DOMAIN" ]] || { printf 'Missing --domain\n' >&2; exit 1; }
[[ -n "$EMAIL" ]] || { printf 'Missing --email\n' >&2; exit 1; }

cleanup() {
    if [[ -n "$CHALLENGE_SERVER_PID" ]]; then
        kill "$CHALLENGE_SERVER_PID" 2>/dev/null || true
        wait "$CHALLENGE_SERVER_PID" 2>/dev/null || true
    fi
    if [[ -n "$SUDO_KEEPALIVE_PID" ]]; then
        kill "$SUDO_KEEPALIVE_PID" 2>/dev/null || true
        wait "$SUDO_KEEPALIVE_PID" 2>/dev/null || true
    fi
}

trap cleanup EXIT

cd "$ROOT_DIR"
npm run build
sudo -v
(while true; do sudo -n true; sleep 60; done) &
SUDO_KEEPALIVE_PID="$!"

bash script/install-service.sh --service "$SERVICE_NAME" --user "$RUN_USER" --no-start

mkdir -p "$ROOT_DIR/.well-known/acme-challenge"
sudo env WEBROOT="$ROOT_DIR" HOST="::" PORT="80" "$(command -v node)" "$ROOT_DIR/script/challenge-server.js" >/tmp/akao-acme-challenge.log 2>&1 &
CHALLENGE_SERVER_PID="$!"
sleep 2

bash script/ssl.sh --domain "$DOMAIN" --email "$EMAIL" --webroot-mode --webroot "$ROOT_DIR" --reload-cmd "sudo systemctl restart $SERVICE_NAME"

kill "$CHALLENGE_SERVER_PID" 2>/dev/null || true
wait "$CHALLENGE_SERVER_PID" 2>/dev/null || true
CHALLENGE_SERVER_PID=""

sudo systemctl restart "$SERVICE_NAME"
sudo systemctl --no-pager --full status "$SERVICE_NAME"
