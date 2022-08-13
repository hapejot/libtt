#!/usr/bin/env tclsh 
set fd_out {}
set defs {}
set cmp "<comp>"

proc name_list {params} {
    set result {}
    foreach x $params {
        lappend result [lindex $x 1]
    }
    return [join $result {, }]
}

proc function {name desc in_params out_param} {
    global fd_out defs cmp

    set parnames [name_list $in_params]
    set par [join $in_params {, }]
    set result_type [lindex $out_param 0]
    if {$result_type != ""}  {set result_type "($result_type)"}
    lappend defs "#define bc_${cmp}_${name}($parnames) ${result_type}(g_${cmp}.${name}($parnames))"
    set rtype [lindex $out_param 0]
    if {$rtype == ""} {set rtype "void"}
    puts $fd_out "${rtype}(*$name)($par);"
}
    


proc component {name} {
    global fd_out cmp
    set cmp $name
    if {$fd_out != ""} { close $fd_out }
    set fd_out [open "$name.h" w]
    puts $fd_out "extern struct $name \{"
}

foreach arg $argv {
    source $arg
}

puts $fd_out "\}        g_$cmp;"
foreach x $defs {
    puts $fd_out $x
}
close $fd_out
