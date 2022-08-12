component mem
# function array          {Allocate an array}                                 {{void* ctx} {int type} {int count}}                {void* ptr}
# function asprintf       {Format a string given a va_list}                   {{char* str} {char* fmt} {va_list ap}}              {}
# function enable_leak_report
function free           {Free a chunk of memory.}                           {{void* ptr}}                                       {int rc}
function get_type       {Get a typed pointer out of a pointer.}             {{void* ptr} {int type}}                            {void* ptr}
function realloc        {Change the size of a array.}                       {{void* ctx} {void* p} {int type} {int count}}      {void* ptr}
function link           {Create an additional talloc parent to a pointer.}  {{void* src} {void* target}}                        {void* result}
#function report_depth_file {} {} {}
#function set_abort_fn
#function set_log_stderr
#function show_parents
#function steal
# function strdup         {copy string into local memory}                     {{void* Context} {char* String}}                    {char* String}
# function strdup_append  {append chars to memory string}                     {{char* target} {char* src}}                        {}
function unlink         {remove assocation and eventually free memory}      {{void* ptr}}                                       {}
# function zero           {Allocate a 0-initizialized structure.}             {{void* ctx} {int type}}                            {void*}
# function zero_array     {Allocate a zero initialized array}                 {{void* ctx} {int type} {int count}}                {void*}
