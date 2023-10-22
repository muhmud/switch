# switch

Switch enables most-recently used window switching for terminal applications such as `vim`/`nvim` and `tmux`.
Though primarily written for X11, it also appears to work in recent versions of Gnome and Wayland where
XWayland is available.

Switch works by running a background daemon process that globally monitors key press and release events for
modifier keys. Each application used with switch must register a modifier key that it will use for switching
between buffers or windows. As these are created, switch will add them to its window stack. The user can then
choose to cycle through the windows in MRU order by using a keyboard shortcut based on the previously chosen
modifier key. As long as the modifier key is held down, switch will return windows in order without actually
selecting one. When the modifier key is released, switch will assume that the window has been selected and
adjust its window stack accordingly, i.e. by moving the selected window to the top of the stack.

As well as the background switch process, a plugin is also required for the application being used, and this
repo contains such client plugins for `vim`/`nvim` and `tmux`.

# Install

First, build and deploy this project:

```bash
$ make
$ sudo make install
```

The clients directory in this repo contains the aforementioned application plugins, which you can install
according to your personal setup.

## vim

For `vim`/`nvim`, after installing the plugin, you can set your own shortcuts for cycling between windows
by setting `g:switch_enable_key_mappings` to `0`, and adding keys in the following manner:

```
nmap <silent> <M-a> :call SwitchSwitch()<CR>
nmap <silent> <M-S-a> :call SwitchReverseSwitch()<CR>
```

In the case above, `alt` is used as the modifier key, which the vim plugin uses by default. To use a
different modifier key, set the value of `g:switch_modifier_key`. For a list of the available modifiers,
see the switch man page.

## tmux

After installing the provided scripts to `~/.switch/tmux`, add the following to your tmux config:

```
# Switch
set-hook -g window-linked "run-shell ~/.switch/tmux/window-add.sh"
set-hook -g window-unlinked "run-shell ~/.switch/tmux/window-delete.sh"
set-hook -g pane-focus-in "run-shell ~/.switch/tmux/set.sh"
set-hook -g session-created "run-shell 'env SWITCH_MOD_KEY=alt ~/.switch/tmux/session-add.sh'"
set-hook -g session-closed "run-shell ~/.switch/tmux/session-delete.sh"

bind-key -n M-a run-shell "~/.switch/tmux/switch.sh"
bind-key -n M-S-a run-shell "~/.switch/tmux/reverse-switch.sh"
```

In the future, I might turn this into a proper tmux plugin. In this case also, the `alt` key is used
as the modifier. To change this, update the shortcuts, and also the `set-hook` line for
`session-created`.

# Documentation

For further details, please see the switch man page.
