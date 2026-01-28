#include "loc/passes/simplify.hpp"

#include <cmath>
#include <memory>
#include <string>

namespace loc::passes {
using namespace loc::ast;

// ---------- Helpers ----------

// static bool is_zero(double x) {
//     return std::abs(x) < 1e-12;
// }

static bool is_one(double x) {
    return std::abs(x - 1.0) < 1e-12;
}

// Extract a "term" of the form coeff * Ident(name).
// Accepts: Ident(name)  -> coeff=1
//          ScalarMul(coeff, Ident(name)) -> coeff=coeff
static bool extract_ident_term(const Node* n, double& coeff, std::string& name) {
    if (auto id = dynamic_cast<const IdentExpr*>(n)) {
        coeff = 1.0;
        name = id->name;
        return true;
    }
    if (auto sm = dynamic_cast<const ScalarMulExpr*>(n)) {
        if (auto id = dynamic_cast<const IdentExpr*>(sm->expr.get())) {
            coeff = sm->scalar;
            name = id->name;
            return true;
        }
    }
    return false;
}

static NodePtr make_ident_or_scalarmul(double coeff, const std::string& name) {
    if (is_one(coeff)) {
        return std::make_unique<IdentExpr>(name);
    }
    return std::make_unique<ScalarMulExpr>(coeff, std::make_unique<IdentExpr>(name));
}

// Forward declaration
static NodePtr simplify_expr(NodePtr e);

// ---------- Core simplifier ----------

static NodePtr simplify_add(std::unique_ptr<AddExpr> add) {
    add->lhs = simplify_expr(std::move(add->lhs));
    add->rhs = simplify_expr(std::move(add->rhs));

    // Combine like terms if both sides are (coeff * Ident(name))
    double a = 0.0, b = 0.0;
    std::string na, nb;

    if (extract_ident_term(add->lhs.get(), a, na) && extract_ident_term(add->rhs.get(), b, nb) && na == nb) {
        double sum = a + b;
        // Keep 0*D as ScalarMul(0, D) for now (no ZeroExpr yet)
        return make_ident_or_scalarmul(sum, na);
    }

    // No combine possible
    return add;
}

static NodePtr simplify_scalarmul(std::unique_ptr<ScalarMulExpr> sm) {
    sm->expr = simplify_expr(std::move(sm->expr));

    // Flatten nested scalar mul: a*(b*x) -> (a*b)*x
    if (auto inner = dynamic_cast<ScalarMulExpr*>(sm->expr.get())) {
        double a = sm->scalar;
        double b = inner->scalar;

        // Take ownership of inner->expr
        NodePtr inner_child = std::move(inner->expr);

        return std::make_unique<ScalarMulExpr>(a * b, simplify_expr(std::move(inner_child)));
    }

    // Nothing else to do
    return sm;
}

static NodePtr simplify_compose(std::unique_ptr<ComposeExpr> c) {
    c->lhs = simplify_expr(std::move(c->lhs));
    c->rhs = simplify_expr(std::move(c->rhs));

    // Pull scalar out of composition:
    // (a*L) @ R -> a*(L @ R)
    if (auto lsm = dynamic_cast<ScalarMulExpr*>(c->lhs.get())) {
        double a = lsm->scalar;
        NodePtr L = std::move(lsm->expr);  // steal inner expr
        NodePtr R = std::move(c->rhs);

        return simplify_expr(
            std::make_unique<ScalarMulExpr>(
                a,
                std::make_unique<ComposeExpr>(simplify_expr(std::move(L)), simplify_expr(std::move(R)))
            )
        );
    }

    // L @ (a*R) -> a*(L @ R)
    if (auto rsm = dynamic_cast<ScalarMulExpr*>(c->rhs.get())) {
        double a = rsm->scalar;
        NodePtr R = std::move(rsm->expr);  // steal inner expr
        NodePtr L = std::move(c->lhs);

        return simplify_expr(
            std::make_unique<ScalarMulExpr>(
                a,
                std::make_unique<ComposeExpr>(simplify_expr(std::move(L)), simplify_expr(std::move(R)))
            )
        );
    }

    return c;
}

static NodePtr simplify_expr(NodePtr e) {
    if (!e) return e;

    if (dynamic_cast<AddExpr*>(e.get())) {
        NodePtr base = std::move(e);
        auto derived = std::unique_ptr<AddExpr>(static_cast<AddExpr*>(base.release()));
        return simplify_add(std::move(derived));
    }

    if (dynamic_cast<ScalarMulExpr*>(e.get())) {
        NodePtr base = std::move(e);
        auto derived = std::unique_ptr<ScalarMulExpr>(static_cast<ScalarMulExpr*>(base.release()));
        return simplify_scalarmul(std::move(derived));
    }

    if (dynamic_cast<ComposeExpr*>(e.get())) {
        NodePtr base = std::move(e);
        auto derived = std::unique_ptr<ComposeExpr>(static_cast<ComposeExpr*>(base.release()));
        return simplify_compose(std::move(derived));
    }

    return e;
}

// static NodePtr simplify_expr(NodePtr e) {
//     if (!e) return e;

//     if (auto add = dynamic_cast<AddExpr*>(e.get())) {
//         // Transfer ownership into a unique_ptr of the derived type
//         NodePtr base = std::move(e);
//         auto derived = std::unique_ptr<AddExpr>(static_cast<AddExpr*>(base.release()));
//         return simplify_add(std::move(derived));
//     }

//     if (auto sm = dynamic_cast<ScalarMulExpr*>(e.get())) {
//         NodePtr base = std::move(e);
//         auto derived = std::unique_ptr<ScalarMulExpr>(static_cast<ScalarMulExpr*>(base.release()));
//         return simplify_scalarmul(std::move(derived));
//     }

//     if (auto comp = dynamic_cast<ComposeExpr*>(e.get())) {
//         NodePtr base = std::move(e);
//         auto derived = std::unique_ptr<ComposeExpr>(static_cast<ComposeExpr*>(base.release()));
//         return simplify_compose(std::move(derived));
//     }

//     // IdentExpr or anything else: nothing to simplify
//     return e;
// }

// ---------- Statement-level simplifier ----------

void simplify_program(Program& program) {
    for (auto& st : program.statements) {
        if (!st) continue;

        if (auto asn = dynamic_cast<AssignStmt*>(st.get())) {
            asn->expr = simplify_expr(std::move(asn->expr));
        } else if (auto pr = dynamic_cast<PrintStmt*>(st.get())) {
            pr->expr = simplify_expr(std::move(pr->expr));
        }
        // OperatorDecl has no expr; ignore
    }
}

} // namespace loc::passes
