#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "$SCRIPT_DIR/switch-tmux.sh"

WINDOW_ID="$(tmux display -p "#{window_id}")"

add_to_list_file "$WINDOW_ID" "$SWITCH_WINDOW_LIST_FILE"
switch --request add --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP" --id "$WINDOW_ID" || true
