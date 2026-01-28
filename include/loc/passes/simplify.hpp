#pragma once

#include "loc/frontend/ast.hpp"

namespace loc::passes {

// Simplify expressions inside statements in-place.
void simplify_program(loc::ast::Program& program);

} // namespace loc::passes
