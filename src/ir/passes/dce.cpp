#include "loc/ir/passes/dce.hpp"

#include <vector>
#include <queue>
#include <unordered_set>

namespace loc::ir::passes {

static void mark_reachable(const Graph& g, int root, std::vector<char>& live) {
    std::queue<int> q;
    q.push(root);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u < 0 || u >= (int)g.nodes.size()) continue;
        if (live[u]) continue;
        live[u] = 1;

        for (int v : g.nodes[u].inputs) {
            q.push(v);
        }
    }
}

void dead_code_elim(Graph& g) {
    if (g.nodes.empty()) return;

    // 1) Mark live nodes starting from "roots"
    std::vector<char> live(g.nodes.size(), 0);
    for (const auto& s : g.program) {
        // Treat both Assign and Print as roots for now
        mark_reachable(g, s.value, live);
    }

    // 2) Build old->new id map and new node list
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

    // 3) Remap inputs inside kept nodes
    for (auto& n : new_nodes) {
        for (int& in : n.inputs) {
            in = remap[in];
        }
    }

    // 4) Remap program statement values
    for (auto& s : g.program) {
        s.value = remap[s.value];
    }

    g.nodes = std::move(new_nodes);
}

} // namespace loc::ir::passes
