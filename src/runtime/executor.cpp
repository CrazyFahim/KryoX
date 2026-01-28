#include "loc/runtime/executor.hpp"
#include <iostream>
#include <stdexcept>

namespace loc::rt {

Matrix Executor::eval_node(const loc::ir::Graph& g, int node_id) {
    auto it = cache_.find(node_id);
    if (it != cache_.end()) return it->second;

    if (node_id < 0 || node_id >= (int)g.nodes.size())
        throw std::runtime_error("Executor: bad node id");

    const auto& n = g.nodes[node_id];
    Matrix out;

    using loc::ir::NodeKind;

    switch (n.kind) {
        case NodeKind::Op: {
            out = reg_.get(n.name); // copy
            break;
        }
        case NodeKind::ScalarMul: {
            Matrix x = eval_node(g, n.inputs.at(0));
            out = n.scalar * x;
            break;
        }
        case NodeKind::Add: {
            Matrix a = eval_node(g, n.inputs.at(0));
            Matrix b = eval_node(g, n.inputs.at(1));
            out = a + b;
            break;
        }
        case NodeKind::Compose: {
            Matrix a = eval_node(g, n.inputs.at(0));
            Matrix b = eval_node(g, n.inputs.at(1));
            out = matmul(a, b);
            break;
        }
        default:
            throw std::runtime_error("Executor: unsupported IR node kind");
    }

    cache_[node_id] = out;
    return out;
}

void Executor::run(const loc::ir::Graph& g) {
    cache_.clear();

    // optional: store assignments as named results (debug)
    std::unordered_map<std::string, Matrix> env;

    for (const auto& s : g.program) {
        if (s.kind == loc::ir::Graph::Stmt::Kind::Assign) {
            env[s.name] = eval_node(g, s.value);
            continue;
        }

        if (s.kind == loc::ir::Graph::Stmt::Kind::Print) {
            Matrix v = eval_node(g, s.value);
            std::cout << "\n[print]\n" << v;
            continue;
        }
    }
}

} // namespace loc::rt
