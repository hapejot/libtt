cmake_minimum_required(VERSION 3.23)
project(libtt
    DESCRIPTION "Library for components used by Tiny Talk"
    LANGUAGES C)

###############################################################################
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
#include(CheckStructMember)
include(CheckSymbolExists)
#include(CheckTypeExists)
include(CheckTypeSize)

include(FetchContent)

###############################################################################
# Check headers
set(INCLUDES "")
macro(ck_check_include_file header var)
    check_include_files("${INCLUDES};${header}" ${var})
    if(${var})
        set(INCLUDES ${INCLUDES} ${header})
    endif(${var})
endmacro(ck_check_include_file)

# Some FreeBSD headers assume sys/types.h was already included.
ck_check_include_file("sys/types.h" HAVE_SYS_TYPES_H)

# Alphabetize the rest unless there's a compelling reason
ck_check_include_file("errno.h" HAVE_ERRNO_H)
ck_check_include_file("inttypes.h" HAVE_INTTYPES_H)

###############################################################################
# Check for integer types
check_type_size("short" SIZE_OF_SHORT)
check_type_size("int" SIZE_OF_INT)
check_type_size("long" SIZE_OF_LONG)
check_type_size("long long" SIZE_OF_LONG_LONG)

check_type_size("unsigned short" SIZE_OF_UNSIGNED_SHORT)
check_type_size("unsigned" SIZE_OF_UNSIGNED)
check_type_size("unsigned long" SIZE_OF_UNSIGNED_LONG)
check_type_size("unsigned long long" SIZE_OF_UNSIGNED_LONG_LONG)

check_type_size("__int64" __INT64)
check_type_size("unsigned __int64" UNSIGNED___INT64)

check_type_size(int16_t INT16_T)
check_type_size(int32_t INT32_T)
check_type_size(int64_t INT64_T)
check_type_size(intmax_t INTMAX_T)
check_type_size(uint8_t UINT8_T)
check_type_size(uint16_t UINT16_T)
check_type_size(uint32_t UINT32_T)
check_type_size(uint64_t UINT64_T)
check_type_size(uintmax_t UINTMAX_T)



FetchContent_Declare( Check GIT_REPOSITORY https://github.com/libcheck/check.git )


FetchContent_MakeAvailable(Check)

message( "Check: ${CHECK_LIBRARIES}" )


add_executable(test test/main.c)
target_link_libraries(test check)
if(HAVE_PTHREAD)
target_link_libraries(test pthread)
endif()
