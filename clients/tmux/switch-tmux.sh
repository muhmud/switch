
SWITCH_SESSION_ID="$(tmux display -p "#{session_name}")"
SWITCH_APP="tmux-$SWITCH_SESSION_ID"

export SWITCH_SESSION_LIST_FILE="/tmp/switch.tmux.sessions"
export SWITCH_WINDOW_LIST_FILE="/tmp/switch.$SWITCH_APP.windows"
export SWITCH_SOCKET_FILE="/tmp/switch.$SWITCH_APP"
export SWITCH_MOD_KEY=${SWITCH_MOD_KEY:-alt}

function list_file_contains() {
  local -r id=$1
  local -r list_file=$2

  grep -c "^$id\$" "$list_file"
}

function add_to_list_file() {
  local -r id=$1
  local -r list_file=$2

  if [[ ! -f "$list_file" ]] || [[ "$(list_file_contains "$id" "$list_file")" == "0" ]]; then
    echo "$id" >> "$list_file"
  fi
}

function delete_from_list_file() {
  local -r id=$1
  local -r list_file=$2

  if [[ -f "$list_file" ]] || [[ "$(list_file_contains "$id" "$list_file")" != "0" ]]; then
    sed -i "/^${id}\$/d" "$list_file"
  fi
}

function align_list_file() {
  local -r list_file=$1
  local -r new_list=$2

  if [[ -f "$list_file" ]]; then
    local ids=()
    while IFS= read -r id; do
      if [[ "$(grep -c "^$id\$" <<< "$new_list")" == "0" ]]; then
        ids+=("$id")
      fi
    done < "$list_file"

    for id in "${ids[@]}"; do
      echo "$id"
      delete_from_list_file "$id" "$list_file"
    done
  fi
}

function get_session_list() {
  tmux list-sessions -F "#{session_name}"
}

function get_window_list() {
  tmux list-windows -F "#{window_id}"
}

