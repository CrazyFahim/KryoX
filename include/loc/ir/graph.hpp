#pragma once
#include <string>
#include <vector>
#include <iostream>

namespace loc::ir {

enum class NodeKind {
    Op,
    ScalarMul,
    Add,
    Compose
};

struct Node {
    int id = -1;
    NodeKind kind = NodeKind::Op;

    // Op fields
    std::string name;

    // ScalarMul fields
    double scalar = 0.0;

    // DAG inputs
    std::vector<int> inputs;
};

struct Graph {
    struct Stmt {
        enum class Kind { Assign, Print };
        Kind kind = Kind::Print;
        std::string name; // for Assign
        int value = -1;   // node id
    };

    std::vector<Node> nodes;
    std::vector<Stmt> program;

    int add_node(Node n) {
        n.id = (int)nodes.size();
        nodes.push_back(std::move(n));
        return nodes.back().id;
    }

    void dump() const {
        std::cout << "=== IR Graph ===\n";
        for (const auto& n : nodes) {
            std::cout << "%" << n.id << " = ";
            switch (n.kind) {
                case NodeKind::Op:        std::cout << "Op(" << n.name << ")"; break;
                case NodeKind::ScalarMul: std::cout << "ScalarMul(" << n.scalar << ")"; break;
                case NodeKind::Add:       std::cout << "Add"; break;
                case NodeKind::Compose:   std::cout << "Compose(@)"; break;
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
