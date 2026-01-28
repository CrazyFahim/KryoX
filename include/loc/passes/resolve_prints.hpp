#pragma once
#include "loc/frontend/ast.hpp"

namespace loc::passes {

// Rewrites `print Ident(x);` into `print <expr bound to x>;`
// using the assignments seen earlier in the program.
void resolve_prints(loc::ast::Program& program);

} // namespace loc::passes
