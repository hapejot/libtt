component mem
function array {Allocate an array} {ctx, type, count} {ptr}
function asprintf {Format a string given a va_list} {t, fmt, ap} {}
# function enable_leak_report
function free {Free a chunk of memory.} {ptr} {rc}
function get_type {Get a typed pointer out of a pointer.} {ptr, type} {type*}
function realloc {Change the size of a array.} {ctx, p, type, count} {type*}
function reference {ctx, ptr} {Create an additional talloc parent to a pointer.} {void*}
#function report_depth_file {} {} {}
#function set_abort_fn
#function set_log_stderr
#function show_parents
#function steal
function strdup {copy string into local memory} {Context String} {String}
function strdup_append {append chars to memory string} {String String} {}
function unlink {remove assocation and eventually free memory} {String} {}
function zero {Allocate a 0-initizialized structure.} {ctx, type} {}
function zero_array {Allocate a zero initialized array} {ctx type count} {type*}
