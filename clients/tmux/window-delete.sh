#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "$SCRIPT_DIR/switch-tmux.sh"

align_list_file "$SWITCH_WINDOW_LIST_FILE" "$(get_window_list)" | \
  while IFS= read -r id; do
    SWITCH_WINDOW_PANE_LIST_FILE="$SWITCH_WINDOW_LIST_FILE.$id.panes"

    switch --request delete --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP" --id "$id" || true
    switch --request delete-app --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP-$id" || true
    rm -f "$SWITCH_WINDOW_PANE_LIST_FILE"
  done
