#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "$SCRIPT_DIR/switch-tmux.sh"

align_list_file "$SWITCH_SESSION_LIST_FILE" "$(get_session_list)" | \
  while IFS= read -r id; do
    SWITCH_APP="tmux-$id"
    SWITCH_SOCKET_FILE="/tmp/switch.$SWITCH_APP"
    SWITCH_WINDOW_LIST_FILE="/tmp/switch.$SWITCH_APP.windows"

    switch --request shutdown --socket-file "$SWITCH_SOCKET_FILE"
    rm -f "$SWITCH_WINDOW_LIST_FILE" "$SWITCH_WINDOW_LIST_FILE".*
  done
