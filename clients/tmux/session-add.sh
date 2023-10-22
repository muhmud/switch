#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "$SCRIPT_DIR/switch-tmux.sh"

[[ ! -f "$SWITCH_SESSION_LIST_FILE" ]] && touch "$SWITCH_SESSION_LIST_FILE"

switch --server --daemonize --socket-file "$SWITCH_SOCKET_FILE"
switch --request add-app --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP" --mod "$SWITCH_MOD_KEY"

add_to_list_file "$SWITCH_SESSION_ID" "$SWITCH_SESSION_LIST_FILE"
