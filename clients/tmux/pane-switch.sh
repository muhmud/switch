#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "$SCRIPT_DIR/switch-tmux.sh"

WINDOW_ID="$(tmux display -p "#{window_id}")"
PANE_ID="$(switch --request switch --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP-$WINDOW_ID" || true)"
echo "got $PANE_ID from $SWITCH_APP-$WINDOW_ID" >> /tmp/pane.log
if [[ -n "$PANE_ID" ]]; then
  tmux select-pane -t "$PANE_ID"
fi
