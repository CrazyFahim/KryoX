#include "loc/ir/passes/const_fold.hpp"

#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>

namespace loc::ir::passes {

// static bool is_zero(double x) { return std::abs(x) < 1e-12; }
static bool is_one(double x)  { return std::abs(x - 1.0) < 1e-12; }

// stable scalar string to use in keys
static std::string scalar_key(double s) {
    std::ostringstream oss;
    oss << std::setprecision(17) << s;
    return oss.str();
}

// Build a structural key for an IR node (after folding inputs).
static std::string make_key(const loc::ir::Node& n) {
    std::ostringstream oss;
    oss << (int)n.kind << "|";
    if (n.kind == loc::ir::NodeKind::Op) {
        oss << "name=" << n.name;
    } else if (n.kind == loc::ir::NodeKind::ScalarMul) {
        oss << "s=" << scalar_key(n.scalar) << "|";
        oss << "in=" << (n.inputs.empty() ? -1 : n.inputs[0]);
    } else {
        for (size_t i = 0; i < n.inputs.size(); ++i) {
            oss << n.inputs[i];
            if (i + 1 < n.inputs.size()) oss << ",";
        }
    }
    return oss.str();
}

// Intern node into out-graph using a key-cache (CSE).
static int intern_node(loc::ir::Graph& out,
                       std::unordered_map<std::string,int>& intern,
                       loc::ir::Node node) {
    std::string k = make_key(node);
    auto it = intern.find(k);
    if (it != intern.end()) return it->second;
    int id = out.add_node(std::move(node));
    intern[k] = id;
    return id;
}

static int fold_node(int id,
                     const loc::ir::Graph& in,
                     loc::ir::Graph& out,
                     std::unordered_map<int,int>& memo,
                     std::unordered_map<std::string,int>& intern) {
    if (id < 0) return -1;
    if (auto it = memo.find(id); it != memo.end()) return it->second;

    const auto& n = in.nodes[id];

    // ---- Fold recursively depending on kind ----
    if (n.kind == loc::ir::NodeKind::Op) {
        loc::ir::Node nn;
        nn.kind = loc::ir::NodeKind::Op;
        nn.name = n.name;
        int out_id = intern_node(out, intern, std::move(nn));
        memo[id] = out_id;
        return out_id;
    }

    if (n.kind == loc::ir::NodeKind::ScalarMul) {
        int x = fold_node(n.inputs[0], in, out, memo, intern);
        double a = n.scalar;

        // Rule: 1*x -> x
        if (is_one(a)) {
            memo[id] = x;
            return x;
        }

        // Rule: a*(b*x) -> (a*b)*x
        const auto& xn = out.nodes[x];
        if (xn.kind == loc::ir::NodeKind::ScalarMul) {
            double b = xn.scalar;
            int inner = xn.inputs[0];

            loc::ir::Node nn;
            nn.kind = loc::ir::NodeKind::ScalarMul;
            nn.scalar = a * b;
            nn.inputs = { inner };

            int out_id = intern_node(out, intern, std::move(nn));
            memo[id] = out_id;
            return out_id;
        }

        loc::ir::Node nn;
        nn.kind = loc::ir::NodeKind::ScalarMul;
        nn.scalar = a;
        nn.inputs = { x };

        int out_id = intern_node(out, intern, std::move(nn));
        memo[id] = out_id;
        return out_id;
    }

    if (n.kind == loc::ir::NodeKind::Add) {
        int a = fold_node(n.inputs[0], in, out, memo, intern);
        int b = fold_node(n.inputs[1], in, out, memo, intern);

        // Rule: x + x -> 2*x
        if (a == b) {
            loc::ir::Node nn;
            nn.kind = loc::ir::NodeKind::ScalarMul;
            nn.scalar = 2.0;
            nn.inputs = { a };
            int out_id = intern_node(out, intern, std::move(nn));
            memo[id] = out_id;
            return out_id;
        }

        // Rule: (sa*x) + (sb*x) -> (sa+sb)*x   (after folding/canonicalization)
        auto is_smul = [&](int v, double& s, int& inner) -> bool {
            const auto& vn = out.nodes[v];
            if (vn.kind != loc::ir::NodeKind::ScalarMul) return false;
            s = vn.scalar;
            inner = vn.inputs[0];
            return true;
        };

        double sa, sb;
        int xa, xb;
        if (is_smul(a, sa, xa) && is_smul(b, sb, xb) && xa == xb) {
            loc::ir::Node nn;
            nn.kind = loc::ir::NodeKind::ScalarMul;
            nn.scalar = sa + sb;
            nn.inputs = { xa };
            int out_id = intern_node(out, intern, std::move(nn));
            memo[id] = out_id;
            return out_id;
        }

        // Canonical order for better CSE: keep inputs sorted by id
        if (b < a) std::swap(a, b);

        loc::ir::Node nn;
        nn.kind = loc::ir::NodeKind::Add;
        nn.inputs = { a, b };
        int out_id = intern_node(out, intern, std::move(nn));
        memo[id] = out_id;
        return out_id;
    }

    if (n.kind == loc::ir::NodeKind::Compose) {
        int L = fold_node(n.inputs[0], in, out, memo, intern);
        int R = fold_node(n.inputs[1], in, out, memo, intern);

        // Pull scalars out of composition:
        // (a*L) @ R -> a*(L@R)
        // L @ (a*R) -> a*(L@R)
        auto peel_scalar = [&](int v, double& s, int& inner) -> bool {
            const auto& vn = out.nodes[v];
            if (vn.kind != loc::ir::NodeKind::ScalarMul) return false;
            s = vn.scalar;
            inner = vn.inputs[0];
            return true;
        };

        double a, b;
        int Lin, Rin;
        bool Ls = peel_scalar(L, a, Lin);
        bool Rs = peel_scalar(R, b, Rin);

        if (Ls || Rs) {
            double s = 1.0;
            if (Ls) { s *= a; L = Lin; }
            if (Rs) { s *= b; R = Rin; }

            loc::ir::Node comp;
            comp.kind = loc::ir::NodeKind::Compose;
            comp.inputs = { L, R };
            int comp_id = intern_node(out, intern, std::move(comp));

            if (is_one(s)) {
                memo[id] = comp_id;
                return comp_id;
            }

            loc::ir::Node sm;
            sm.kind = loc::ir::NodeKind::ScalarMul;
            sm.scalar = s;
            sm.inputs = { comp_id };
            int out_id = intern_node(out, intern, std::move(sm));

            memo[id] = out_id;
            return out_id;
        }

        loc::ir::Node nn;
        nn.kind = loc::ir::NodeKind::Compose;
        nn.inputs = { L, R };
        int out_id = intern_node(out, intern, std::move(nn));
        memo[id] = out_id;
        return out_id;
    }

    throw std::runtime_error("const_fold: unknown node kind");
}

void const_fold(loc::ir::Graph& g) {
    loc::ir::Graph out;

    std::unordered_map<int,int> memo;
    std::unordered_map<std::string,int> intern;

    // Rebuild program: fold each statement value, but keep statement list.
    // Note: DCE will remove dead assigns afterwards.
    out.program.reserve(g.program.size());

    for (auto s : g.program) {
        int nv = fold_node(s.value, g, out, memo, intern);
        s.value = nv;
        out.program.push_back(std::move(s));
    }

    g = std::move(out);
}

} // namespace loc::ir::passes
