#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVICE_NAME="${SERVICE_NAME:-akao}"
RUN_USER="${RUN_USER:-$(whoami)}"
HOST="${HOST:-::}"
HTTP_PORT="${HTTP_PORT:-80}"
HTTPS_PORT="${HTTPS_PORT:-443}"
SSL_KEY="${SSL_KEY:-$ROOT_DIR/.certs/prod-key.pem}"
SSL_CERT="${SSL_CERT:-$ROOT_DIR/.certs/prod-cert.pem}"
PRINT_ONLY=false
NO_START=false

log() { printf '[akao-service] %s\n' "$1"; }
die() { printf '[akao-service] ERROR: %s\n' "$1" >&2; exit 1; }

show_help() {
    cat <<EOF
Usage:
  $0 [options]

Options:
  --service NAME          Systemd service name (default: akao)
  --user USER             Service user (default: current user)
  --host HOST             Bind host for prod.js (default: ::)
  --http-port PORT        HTTP port (default: 80)
  --https-port PORT       HTTPS port (default: 443)
  --ssl-key PATH          TLS key path (default: $ROOT_DIR/.certs/prod-key.pem)
  --ssl-cert PATH         TLS cert path (default: $ROOT_DIR/.certs/prod-cert.pem)
  --no-start              Install and enable service without restarting it
  --print                 Print the unit instead of installing it
  -h, --help              Show this help
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --service) SERVICE_NAME="$2"; shift 2 ;;
        --user) RUN_USER="$2"; shift 2 ;;
        --host) HOST="$2"; shift 2 ;;
        --http-port) HTTP_PORT="$2"; shift 2 ;;
        --https-port) HTTPS_PORT="$2"; shift 2 ;;
        --ssl-key) SSL_KEY="$2"; shift 2 ;;
        --ssl-cert) SSL_CERT="$2"; shift 2 ;;
        --no-start) NO_START=true; shift ;;
        --print) PRINT_ONLY=true; shift ;;
        -h|--help) show_help; exit 0 ;;
        *) die "Unknown option: $1" ;;
    esac
done

node_bin="$(command -v node)"
[[ -n "$node_bin" ]] || die "node not found in PATH"

service_content="$(cat <<EOF
[Unit]
Description=akao production server
After=network.target
StartLimitIntervalSec=0

[Service]
Type=simple
Restart=always
RestartSec=2
User=$RUN_USER
AmbientCapabilities=CAP_NET_BIND_SERVICE
CapabilityBoundingSet=CAP_NET_BIND_SERVICE
WorkingDirectory=$ROOT_DIR
Environment=HOST=$HOST
Environment=HTTP_PORT=$HTTP_PORT
Environment=HTTPS_PORT=$HTTPS_PORT
Environment=SSL_KEY=$SSL_KEY
Environment=SSL_CERT=$SSL_CERT
ExecStart=$node_bin $ROOT_DIR/prod.js

[Install]
WantedBy=multi-user.target
EOF
)"

if [[ "$PRINT_ONLY" == "true" ]]; then
    printf '%s\n' "$service_content"
    exit 0
fi

service_file="/etc/systemd/system/${SERVICE_NAME}.service"
log "Installing $service_file"
printf '%s\n' "$service_content" | sudo tee "$service_file" >/dev/null
sudo systemctl daemon-reload
sudo systemctl enable "$SERVICE_NAME"
if [[ "$NO_START" == "true" ]]; then
    log "Installed and enabled $SERVICE_NAME without starting it"
    exit 0
fi
sudo systemctl restart "$SERVICE_NAME"
sudo systemctl --no-pager --full status "$SERVICE_NAME"
