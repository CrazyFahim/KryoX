cmake_minimum_required(VERSION 3.16)
project(linear_op_compiler LANGUAGES C CXX)

# -----------------------------
# Language settings
# -----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------
# Find Flex & Bison
# -----------------------------
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# -----------------------------
# Include directories
# -----------------------------
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}   # for generated parser headers
)

# -----------------------------
# Flex / Bison targets
# -----------------------------
BISON_TARGET(
    Parser
    src/frontend/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp
)

FLEX_TARGET(
    Lexer
    src/frontend/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# -----------------------------
# Compiler executable
# -----------------------------
add_executable(loc
    src/main.cpp
    # frontend
    src/frontend/ast.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}

    # IR
    src/ir/graph.cpp

    # passes
    src/passes/simplify.cpp
    src/passes/resolve_prints.cpp
    
    # runtime
    src/runtime/executor.cpp

    # dce
    src/ir/dce.cpp
    
    # IR passes
    src/ir/const_fold.cpp

    # Matrix stuffs:
    src/runtime/matrix.cpp
    src/runtime/registry.cpp
)

# -----------------------------
# Warnings (recommended)
# -----------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(loc PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()
