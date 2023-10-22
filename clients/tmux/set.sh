#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
source "$SCRIPT_DIR/switch-tmux.sh"

WINDOW_ID="$(tmux display -p "#{window_id}")"
PANE_ID="$(tmux display -p "#{pane_id}")"

[[ ! -f "$SWITCH_SESSION_LIST_FILE" ]] && touch "$SWITCH_SESSION_LIST_FILE"
[[ ! -f "$SWITCH_WINDOW_LIST_FILE" ]] && touch "$SWITCH_WINDOW_LIST_FILE"
SWITCH_WINDOW_PANE_LIST_FILE="$SWITCH_WINDOW_LIST_FILE.$WINDOW_ID.panes"

if [[ "$(list_file_contains "$SWITCH_SESSION_ID" "$SWITCH_SESSION_LIST_FILE")" == "0" ]]; then
  switch --server --daemonize --socket-file "$SWITCH_SOCKET_FILE"
  switch --request add-app --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP" --mod "$SWITCH_MOD_KEY"

  touch "$SWITCH_SESSION_LIST_FILE"
  add_to_list_file "$SWITCH_SESSION_ID" "$SWITCH_SESSION_LIST_FILE"
fi

switch --request add --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP" --id "$WINDOW_ID" || true
add_to_list_file "$WINDOW_ID" "$SWITCH_WINDOW_LIST_FILE"

if [[ ! -f "$SWITCH_WINDOW_PANE_LIST_FILE" ]]; then
  switch --request add-app --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP-$WINDOW_ID" --mod "$SWITCH_PANE_MOD_KEY" 2> /tmp/pane.log

  touch "$SWITCH_WINDOW_PANE_LIST_FILE"
fi

# Ensure the pane list is up-to-date
align_list_file "$SWITCH_WINDOW_PANE_LIST_FILE" "$(get_pane_list)" | \
  while IFS= read -r id; do
    switch --request delete --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP-$WINDOW_ID" --id "$id" || true
  done

switch --request set --socket-file "$SWITCH_SOCKET_FILE" --app "$SWITCH_APP-$WINDOW_ID" --id "$PANE_ID" || true
add_to_list_file "$PANE_ID" "$SWITCH_WINDOW_PANE_LIST_FILE"
