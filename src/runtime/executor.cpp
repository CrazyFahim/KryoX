#include "loc/runtime/executor.hpp"
#include <iostream>
#include <stdexcept>

namespace loc::rt {


void Executor::run(const loc::ir::Graph& g) {
    // Resize and clear cache for the new run
    cache_.assign(g.nodes.size(), std::nullopt);

    for (const auto& s : g.program) {
        if (s.kind == loc::ir::Graph::Stmt::Kind::Assign) {
            (void)eval(g, s.value);
        } else if (s.kind == loc::ir::Graph::Stmt::Kind::Print) {
            Matrix v = eval(g, s.value);
            std::cout << "\n[print]\n" << v << "\n";
        } else {
            throw std::runtime_error("Executor: unknown stmt kind");
        }
    }
}

Matrix Executor::eval(const loc::ir::Graph& g, int id) {
    if (id < 0 || id >= (int)g.nodes.size()) {
        throw std::runtime_error("Executor: invalid node id");
    }

    // Check cache
    if (cache_[id].has_value()) {
        // std::cout << "Computing node (cached) " << id << "\n"; // DEBUG
        return *cache_[id];
    }
    
    // std::cout << "Computing node (fresh) " << id << "\n"; // DEBUG

    const auto& n = g.nodes[id];
    using K = loc::ir::NodeKind; 

    Matrix result;

    switch (n.kind) {
    case K::Op:
        result = reg_.get(n.name);
        break;

    case K::ScalarMul: {
        Matrix a = eval(g, n.inputs.at(0));
        result = a * n.scalar;
        break;
    }

    case K::Add: {
        Matrix a = eval(g, n.inputs.at(0));
        Matrix b = eval(g, n.inputs.at(1));
        result = a + b;
        break;
    }

    case K::Compose: {
        Matrix a = eval(g, n.inputs.at(0));
        Matrix b = eval(g, n.inputs.at(1));
        result = a.matmul(b);
        break;
    }
    
    default:
        throw std::runtime_error("Executor: unreachable");
    }

    // Store in cache
    cache_[id] = result;
    return result;
}

} // namespace loc::rt

