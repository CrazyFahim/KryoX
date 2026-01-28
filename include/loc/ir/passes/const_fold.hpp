#pragma once
#include "loc/ir/graph.hpp"

namespace loc::ir::passes {

// Rebuilds graph from roots, performing constant folding + canonicalization.
void const_fold(Graph& g);

} // namespace loc::ir::passes
