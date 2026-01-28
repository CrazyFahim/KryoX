#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace loc::ir {

enum class NodeKind {
    Op,        // named operator, e.g. "D", "F"
    Add,       // x + y
    Compose,   // x @ y
    ScalarMul  // a * x
};

struct Node {
    int id = -1;
    NodeKind kind{NodeKind::Op};

    // For Op nodes
    std::string name;

    // For ScalarMul
    double scalar{1.0};

    // Inputs (edges): indices into Graph::nodes
    std::vector<int> inputs;
};

struct Graph {
    std::vector<Node> nodes;

    // Program-level statements
    struct Stmt {
        enum class Kind { Assign, Print } kind;
        std::string name; // for Assign: variable name; for Print: optional label
        int value = -1;   // node id in nodes[]
    };

    std::vector<Stmt> program;

    int add_node(Node n) {
        n.id = static_cast<int>(nodes.size());
        nodes.push_back(std::move(n));
        return nodes.back().id;
    }

    void dump() const {
        std::cout << "=== IR Graph ===\n";
        for (const auto& n : nodes) {
            std::cout << "%" << n.id << " = ";
            switch (n.kind) {
                case NodeKind::Op:
                    std::cout << "Op(" << n.name << ")";
                    break;
                case NodeKind::Add:
                    std::cout << "Add";
                    break;
                case NodeKind::Compose:
                    std::cout << "Compose(@)";
                    break;
                case NodeKind::ScalarMul:
                    std::cout << "ScalarMul(" << n.scalar << ")";
                    break;
            }
            if (!n.inputs.empty()) {
                std::cout << " [";
                for (size_t i = 0; i < n.inputs.size(); ++i) {
                    std::cout << "%" << n.inputs[i];
                    if (i + 1 < n.inputs.size()) std::cout << ", ";
                }
                std::cout << "]";
            }
            std::cout << "\n";
        }

        std::cout << "\n=== IR Program ===\n";
        for (const auto& s : program) {
            if (s.kind == Stmt::Kind::Assign) {
                std::cout << s.name << " = %" << s.value << "\n";
            } else {
                std::cout << "print %" << s.value << "\n";
            }
        }
    }
};

} // namespace loc::ir
