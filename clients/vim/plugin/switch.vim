"Vim plugin for switch
" Last Change:  2023 Sep 24
" Maintainer:   Muhmud Ahmad
" License:      This file is placed in the public domain.

let s:save_cpo = &cpo
set cpo&vim

" Only do this when not done yet for this buffer
if exists("g:loaded_switch")
  finish
endif
let g:loaded_switch = 1

" Script variable(s)
let s:switch_keys_mapped = 0
let s:docs_path = $VIMRUNTIME . '/doc/'

function SwitchApplyDefaultKeyMappings()
  if s:switch_keys_mapped == 0
    vmap <silent> <M-a> :call SwitchSwitch()<CR>
    vmap <silent> <M-S-a> :call SwitchReverseSwitch()<CR>
    imap <silent> <M-a> <C-O>:call SwitchSwitch()<CR>
    imap <silent> <M-S-a> <C-O>:call SwitchReverseSwitch()<CR>
    nmap <silent> <M-a> :call SwitchSwitch()<CR>
    nmap <silent> <M-S-a> :call SwitchReverseSwitch()<CR>

    let s:switch_keys_mapped = 1
  endif
endfunction

function SwitchStart()
  if !exists('s:switch_app')
    let s:switch_app = printf("vim-%d", getpid())
    let s:switch_socket_file = printf("/tmp/switch.%s", s:switch_app)
    call system(["switch", "--server", "--daemonize", "--socket-file", s:switch_socket_file])
    call system(["switch", "--request", "add-app", "--socket-file", s:switch_socket_file, "--app", s:switch_app, "--mod", g:switch_modifier_key])
    augroup switch_buffers
      autocmd!
      autocmd BufEnter * call SwitchSet()
      autocmd BufUnload * call SwitchDelete()
      autocmd VimLeave * call SwitchExit()
    augroup END
  endif
  if g:switch_enable_key_mappings == 1
    call SwitchApplyDefaultKeyMappings()
  endif
endfunction

" By default, keys will be mapped
if !exists("g:switch_enable_key_mappings")
  let g:switch_enable_key_mappings=1
endif

if !exists("g:switch_modifier_key")
  let g:switch_modifier_key="alt"
endif

call SwitchStart()

function SwitchSwitch()
  let l:buffer = system(["switch", "--request", "switch", "--socket-file", s:switch_socket_file, "--app", s:switch_app])
  if l:buffer != "\n"
    execute 'buffer ' . l:buffer
  endif
endfunction

function SwitchReverseSwitch()
  let l:buffer = system(["switch", "--request", "switch", "--socket-file", s:switch_socket_file, "--app", s:switch_app, "--reverse"])
  execute 'buffer ' . l:buffer
endfunction

function SwitchNew()
  enew
  let l:bufnum = bufnr('%')

  call system(["switch", "--request", "add", "--socket-file", s:switch_socket_file, "--app", s:switch_app, "--id", l:bufnum])
endfunction

function SwitchSet()
  let l:bufnum = expand('<abuf>')
  if l:bufnum != -1
    call system(["switch", "--request", "set", "--socket-file", s:switch_socket_file, "--app", s:switch_app, "--id", l:bufnum])
  endif
endfunction

function SwitchDelete()
  call system(["switch", "--request", "delete", "--socket-file", s:switch_socket_file, "--app", s:switch_app, "--id", expand('<abuf>')])
endfunction

function SwitchClose()
  call system(["switch", "--request", "delete", "--socket-file", s:switch_socket_file, "--app", s:switch_app, "--id", bufnr('%')])
  execute 'q'
endfunction

function SwitchExit()
  call system(["switch", "--request", "shutdown", "--socket-file", s:switch_socket_file])
endfunction
