#!/usr/bin/env tclsh 
set fd_out {}

proc function {name desc in_param out_params} {
    global fd_out
    puts $fd_out "void*(*$name)();"}


proc component {name} {
    global fd_out
    if {$fd_out != ""} { close $fd_out }
    set fd_out [open "$name.c" w]
    puts $fd_out "struct $name \{"
}

foreach arg $argv {
    source $arg
}

puts $fd_out "\};"
close $fd_out
