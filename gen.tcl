#!/usr/bin/env tclsh 
proc function {name} {puts $name}

foreach arg $argv {
    source $arg
}
