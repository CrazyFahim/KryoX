#pragma once

#include "loc/ir/graph.hpp"
#include "loc/runtime/registry.hpp"
#include "loc/runtime/matrix.hpp"

#include <optional>
#include <vector>

namespace loc::rt {

class Executor {
public:
    explicit Executor(const Registry& reg) : reg_(reg) {}

    void run(const loc::ir::Graph& g);

private:
    const Registry& reg_;

    // NEW: memoization cache (one slot per IR node id)
    mutable std::vector<std::optional<Matrix>> cache_;

    Matrix eval(const loc::ir::Graph& g, int id);
};

} // namespace loc::rt
