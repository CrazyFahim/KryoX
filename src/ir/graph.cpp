#include "loc/ir/lower.hpp"

#include <unordered_map>
#include <stdexcept>
#include <string>

namespace loc::ir {

// Create a structural key for an AST expression.
// Minimal but effective for our current node types.
static std::string key_of(const loc::ast::Node& e) {
    if (auto id = dynamic_cast<const loc::ast::IdentExpr*>(&e)) {
        return "id:" + id->name;
    }
    if (auto sm = dynamic_cast<const loc::ast::ScalarMulExpr*>(&e)) {
        return "sm:" + std::to_string(sm->scalar) + "(" + key_of(*sm->expr) + ")";
    }
    if (auto add = dynamic_cast<const loc::ast::AddExpr*>(&e)) {
        return "add(" + key_of(*add->lhs) + "," + key_of(*add->rhs) + ")";
    }
    if (auto comp = dynamic_cast<const loc::ast::ComposeExpr*>(&e)) {
        return "comp(" + key_of(*comp->lhs) + "," + key_of(*comp->rhs) + ")";
    }
    throw std::runtime_error("key_of: unsupported AST expr node");
}

static int lower_expr(const loc::ast::Node& e,
                      Graph& g,
                      std::unordered_map<std::string,int>& op_cache,
                      std::unordered_map<std::string,int>& expr_cache) {
    // CSE: reuse if we've already lowered an identical AST expression
    std::string k = key_of(e);
    if (auto it = expr_cache.find(k); it != expr_cache.end()) {
        return it->second;
    }

    int nid = -1;

    if (auto id = dynamic_cast<const loc::ast::IdentExpr*>(&e)) {
        auto it = op_cache.find(id->name);
        if (it != op_cache.end()) {
            nid = it->second;
        } else {
            Node n;
            n.kind = NodeKind::Op;
            n.name = id->name;
            nid = g.add_node(std::move(n));
            op_cache[id->name] = nid;
        }
        expr_cache[k] = nid;
        return nid;
    }

    if (auto sm = dynamic_cast<const loc::ast::ScalarMulExpr*>(&e)) {
        int x = lower_expr(*sm->expr, g, op_cache, expr_cache);
        Node n;
        n.kind = NodeKind::ScalarMul;
        n.scalar = sm->scalar;
        n.inputs = {x};
        nid = g.add_node(std::move(n));
        expr_cache[k] = nid;
        return nid;
    }

    if (auto add = dynamic_cast<const loc::ast::AddExpr*>(&e)) {
        int a = lower_expr(*add->lhs, g, op_cache, expr_cache);
        int b = lower_expr(*add->rhs, g, op_cache, expr_cache);
        Node n;
        n.kind = NodeKind::Add;
        n.inputs = {a, b};
        nid = g.add_node(std::move(n));
        expr_cache[k] = nid;
        return nid;
    }

    if (auto comp = dynamic_cast<const loc::ast::ComposeExpr*>(&e)) {
        int a = lower_expr(*comp->lhs, g, op_cache, expr_cache);
        int b = lower_expr(*comp->rhs, g, op_cache, expr_cache);
        Node n;
        n.kind = NodeKind::Compose;
        n.inputs = {a, b};
        nid = g.add_node(std::move(n));
        expr_cache[k] = nid;
        return nid;
    }

    throw std::runtime_error("lower_expr: unsupported AST expr node");
}

Graph lower_program(const loc::ast::Program& prog) {
    Graph g;
    std::unordered_map<std::string,int> op_cache;
    std::unordered_map<std::string,int> expr_cache;

    for (const auto& st_ptr : prog.statements) {
        const loc::ast::Node& st = *st_ptr;

        if (auto od = dynamic_cast<const loc::ast::OperatorDecl*>(&st)) {
            if (op_cache.find(od->name) == op_cache.end()) {
                Node n;
                n.kind = NodeKind::Op;
                n.name = od->name;
                int nid = g.add_node(std::move(n));
                op_cache[od->name] = nid;
                expr_cache["id:" + od->name] = nid;
            }
            continue;
        }

        if (auto asn = dynamic_cast<const loc::ast::AssignStmt*>(&st)) {
            int v = lower_expr(*asn->expr, g, op_cache, expr_cache);
            Graph::Stmt s;
            s.kind = Graph::Stmt::Kind::Assign;
            s.name = asn->name;
            s.value = v;
            g.program.push_back(std::move(s));
            
            // Allow variable reuse in subsequent statements
            op_cache[asn->name] = v;
            expr_cache["id:" + asn->name] = v;
            continue;
        }

        if (auto pr = dynamic_cast<const loc::ast::PrintStmt*>(&st)) {
            int v = lower_expr(*pr->expr, g, op_cache, expr_cache);
            Graph::Stmt s;
            s.kind = Graph::Stmt::Kind::Print;
            s.value = v;
            g.program.push_back(std::move(s));
            continue;
        }

        throw std::runtime_error("lower_program: unsupported AST stmt node");
    }

    return g;
}

} // namespace loc::ir
