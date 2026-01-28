#include "loc/passes/resolve_prints.hpp"

#include <unordered_map>
#include <string>
#include <stdexcept>

namespace loc::passes {
using namespace loc::ast;

// ----- Deep clone helper (because AST uses unique_ptr) -----

static NodePtr clone_node(const Node& n) {
    if (auto id = dynamic_cast<const IdentExpr*>(&n)) {
        return std::make_unique<IdentExpr>(id->name);
    }
    if (auto sm = dynamic_cast<const ScalarMulExpr*>(&n)) {
        return std::make_unique<ScalarMulExpr>(sm->scalar, clone_node(*sm->expr));
    }
    if (auto add = dynamic_cast<const AddExpr*>(&n)) {
        return std::make_unique<AddExpr>(clone_node(*add->lhs), clone_node(*add->rhs));
    }
    if (auto comp = dynamic_cast<const ComposeExpr*>(&n)) {
        return std::make_unique<ComposeExpr>(clone_node(*comp->lhs), clone_node(*comp->rhs));
    }

    // Statements (usually not cloned here)
    if (auto od = dynamic_cast<const OperatorDecl*>(&n)) {
        return std::make_unique<OperatorDecl>(od->name);
    }
    if (auto asn = dynamic_cast<const AssignStmt*>(&n)) {
        return std::make_unique<AssignStmt>(asn->name, clone_node(*asn->expr));
    }
    if (auto pr = dynamic_cast<const PrintStmt*>(&n)) {
        return std::make_unique<PrintStmt>(clone_node(*pr->expr));
    }
    if (auto prog = dynamic_cast<const Program*>(&n)) {
        auto out = std::make_unique<Program>();
        for (const auto& st : prog->statements) out->statements.emplace_back(clone_node(*st));
        return out;
    }

    throw std::runtime_error("clone_node: unsupported AST node type");
}

// ----- Main pass -----

void resolve_prints(Program& program) {
    // Map variable name -> expression (as a raw pointer owned by AssignStmt)
    std::unordered_map<std::string, const Node*> env;

    for (auto& st : program.statements) {
        if (!st) continue;

        if (auto asn = dynamic_cast<AssignStmt*>(st.get())) {
            // Record the latest binding
            env[asn->name] = asn->expr.get();
            continue;
        }

        if (auto pr = dynamic_cast<PrintStmt*>(st.get())) {
            // Only rewrite `print Ident(x);`
            if (auto id = dynamic_cast<IdentExpr*>(pr->expr.get())) {
                auto it = env.find(id->name);
                if (it != env.end() && it->second) {
                    pr->expr = clone_node(*it->second); // replace Ident(x) with bound expr clone
                }
            }
        }
    }
}

} // namespace loc::passes
