#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "$SCRIPT_DIR/switch-tmux.sh"

WINDOW_ID="$(switch --request switch --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP" || true)"
if [[ -n "$WINDOW_ID" ]]; then
  tmux select-window -t "$WINDOW_ID"
fi
