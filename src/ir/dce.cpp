#include "loc/ir/passes/dce.hpp"

#include <vector>
#include <queue>

namespace loc::ir::passes {

static void mark_reachable(const Graph& g, int root, std::vector<char>& live) {
    std::queue<int> q;
    q.push(root);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u < 0 || u >= (int)g.nodes.size()) continue;
        if (live[u]) continue;
        live[u] = 1;

        for (int v : g.nodes[u].inputs) q.push(v);
    }
}

void dead_code_elim(Graph& g) {
    if (g.nodes.empty()) return;

    // 1) Mark live nodes from PRINT roots only
    std::vector<char> live(g.nodes.size(), 0);
    for (const auto& s : g.program) {
        if (s.kind == Graph::Stmt::Kind::Print) {
            mark_reachable(g, s.value, live);
        }
    }

    // 2) Build remap + new nodes
    std::vector<int> remap(g.nodes.size(), -1);
    std::vector<Node> new_nodes;
    new_nodes.reserve(g.nodes.size());

    for (int i = 0; i < (int)g.nodes.size(); ++i) {
        if (!live[i]) continue;
        remap[i] = (int)new_nodes.size();
        Node n = g.nodes[i];
        n.id = remap[i];
        new_nodes.push_back(std::move(n));
    }

    // 3) Remap inputs
    for (auto& n : new_nodes) {
        for (int& in : n.inputs) {
            in = (in >= 0 && in < (int)remap.size()) ? remap[in] : -1;
        }
    }

    // 4) Remap program + drop dead assigns
    std::vector<Graph::Stmt> new_prog;
    new_prog.reserve(g.program.size());

    for (auto s : g.program) {
        int nv = -1;
        if (s.value >= 0 && s.value < (int)remap.size()) nv = remap[s.value];

        if (s.kind == Graph::Stmt::Kind::Assign) {
            if (nv == -1) continue;     // drop dead assignment
            s.value = nv;
            new_prog.push_back(std::move(s));
        } else { // Print
            s.value = nv;               // should never be -1 if rooted correctly
            new_prog.push_back(std::move(s));
        }
    }

    g.nodes = std::move(new_nodes);
    g.program = std::move(new_prog);
}

} // namespace loc::ir::passes
