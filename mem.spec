function array {Allocate an array} {ctx, type, count} {ptr}
function asprintf {Format a string given a va_list} {t, fmt, ap} {}
# function enable_leak_report
function free {Free a chunk of memory.} {ptr} {rc}
function get_type {Get a typed pointer out of a talloc pointer.} {ptr, type} {type*}
function realloc {Change the size of a talloc array.} {ctx, p, type, count} {type*}
function reference
function report_depth_file
function set_abort_fn
function set_log_stderr
function show_parents
function steal
function strdup
function strdup_append
function unlink
function zero
function zero_array
