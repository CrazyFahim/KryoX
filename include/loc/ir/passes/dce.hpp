#pragma once
#include "loc/ir/graph.hpp"

namespace loc::ir::passes {

// Removes unused nodes and remaps ids; also updates program statements.
void dead_code_elim(Graph& g);

} // namespace loc::ir::passes
