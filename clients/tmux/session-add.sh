#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "$SCRIPT_DIR/switch-tmux.sh"

[[ ! -f "$SWITCH_SESSION_LIST_FILE" ]] && touch "$SWITCH_SESSION_LIST_FILE"

switch --server --socket-file "$SWITCH_SOCKET_FILE" & disown
sleep 0.1
switch --request add-app --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP" --mod alt

add_to_list_file "$SWITCH_SESSION_ID" "$SWITCH_SESSION_LIST_FILE"
