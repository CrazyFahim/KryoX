#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <optional>

namespace loc::ast {

// -----------------------------
// Base AST node
// -----------------------------
struct Node {
    virtual ~Node() = default;
    virtual void dump(int indent = 0) const = 0;
};

using NodePtr = std::unique_ptr<Node>;

// -----------------------------
// Utility for indentation
// -----------------------------
inline void indent(int n) {
    for (int i = 0; i < n; ++i) std::cout << "  ";
}

// -----------------------------
// Matrix literal (for operator init)
// -----------------------------
struct MatrixLiteral {
    std::vector<std::vector<double>> rows; // must be rectangular
};

// -----------------------------
// Expressions (operator algebra)
// -----------------------------
struct Expr : Node {};

// Identifier: D, F, I, L
struct IdentExpr : Expr {
    std::string name;

    explicit IdentExpr(std::string n) : name(std::move(n)) {}

    void dump(int indent_lvl = 0) const override {
        indent(indent_lvl);
        std::cout << "Ident(" << name << ")\n";
    }
};

// Scalar * Expr   (e.g., 2 * D)
struct ScalarMulExpr : Expr {
    double scalar;
    NodePtr expr;

    ScalarMulExpr(double s, NodePtr e)
        : scalar(s), expr(std::move(e)) {}

    void dump(int indent_lvl = 0) const override {
        indent(indent_lvl);
        std::cout << "ScalarMul(" << scalar << ")\n";
        expr->dump(indent_lvl + 1);
    }
};

// Expr + Expr
struct AddExpr : Expr {
    NodePtr lhs;
    NodePtr rhs;

    AddExpr(NodePtr l, NodePtr r)
        : lhs(std::move(l)), rhs(std::move(r)) {}

    void dump(int indent_lvl = 0) const override {
        indent(indent_lvl);
        std::cout << "Add\n";
        lhs->dump(indent_lvl + 1);
        rhs->dump(indent_lvl + 1);
    }
};

// Expr @ Expr   (composition)
struct ComposeExpr : Expr {
    NodePtr lhs;
    NodePtr rhs;

    ComposeExpr(NodePtr l, NodePtr r)
        : lhs(std::move(l)), rhs(std::move(r)) {}

    void dump(int indent_lvl = 0) const override {
        indent(indent_lvl);
        std::cout << "Compose (@)\n";
        lhs->dump(indent_lvl + 1);
        rhs->dump(indent_lvl + 1);
    }
};

// -----------------------------
// Statements
// -----------------------------
struct Stmt : Node {};

// operator D;
// operator D = [[0,1],[-1,0]];
struct OperatorDecl : Stmt {
    std::string name;
    std::optional<MatrixLiteral> init; // NEW

    explicit OperatorDecl(std::string n)
        : name(std::move(n)) {}

    OperatorDecl(std::string n, MatrixLiteral m)
        : name(std::move(n)), init(std::move(m)) {}

    void dump(int indent_lvl = 0) const override {
        indent(indent_lvl);
        std::cout << "OperatorDecl(" << name << ")";

        if (init) {
            std::cout << " = [";
            for (size_t i = 0; i < init->rows.size(); ++i) {
                std::cout << "[";
                for (size_t j = 0; j < init->rows[i].size(); ++j) {
                    std::cout << init->rows[i][j];
                    if (j + 1 < init->rows[i].size()) std::cout << ", ";
                }
                std::cout << "]";
                if (i + 1 < init->rows.size()) std::cout << ", ";
            }
            std::cout << "]";
        }

        std::cout << "\n";
    }
};

// L = expr;
struct AssignStmt : Stmt {
    std::string name;
    NodePtr expr;

    AssignStmt(std::string n, NodePtr e)
        : name(std::move(n)), expr(std::move(e)) {}

    void dump(int indent_lvl = 0) const override {
        indent(indent_lvl);
        std::cout << "Assign(" << name << ")\n";
        expr->dump(indent_lvl + 1);
    }
};

// print expr;
struct PrintStmt : Stmt {
    NodePtr expr;

    explicit PrintStmt(NodePtr e)
        : expr(std::move(e)) {}

    void dump(int indent_lvl = 0) const override {
        indent(indent_lvl);
        std::cout << "Print\n";
        expr->dump(indent_lvl + 1);
    }
};

// -----------------------------
// Program root
// -----------------------------
struct Program : Node {
    std::vector<NodePtr> statements;

    void dump(int indent_lvl = 0) const override {
        indent(indent_lvl);
        std::cout << "Program\n";
        for (const auto& s : statements) {
            s->dump(indent_lvl + 1);
        }
    }
};

} // namespace loc::ast
