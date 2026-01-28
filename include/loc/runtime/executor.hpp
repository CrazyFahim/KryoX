#pragma once
#include "loc/ir/graph.hpp"
#include "loc/runtime/registry.hpp"
#include "loc/runtime/matrix.hpp"
#include <unordered_map>

namespace loc::rt {

class Executor {
public:
    explicit Executor(const Registry& reg) : reg_(reg) {}

    // Evaluate a node (matrix result)
    Matrix eval_node(const loc::ir::Graph& g, int node_id);

    // Run program and print outputs
    void run(const loc::ir::Graph& g);

private:
    const Registry& reg_;
    std::unordered_map<int, Matrix> cache_;
};

} // namespace loc::rt
