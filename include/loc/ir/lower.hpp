#pragma once
#include "loc/frontend/ast.hpp"
#include "loc/ir/graph.hpp"

namespace loc::ir {

// Lowers AST Program into IR Graph
Graph lower_program(const loc::ast::Program& prog);

} // namespace loc::ir
