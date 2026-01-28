#pragma once
#include "loc/ir/graph.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace loc::ir {

struct Pass {
    std::string name;
    std::function<void(Graph&)> run;
};

class PassManager {
public:
    bool dump_after_each = false;

    void add(std::string name, std::function<void(Graph&)> fn) {
        passes_.push_back(Pass{std::move(name), std::move(fn)});
    }

    void run(Graph& g) const {
        for (const auto& p : passes_) {
            // std::cout << "\n[pass] " << p.name << "\n";
            p.run(g);
            if (dump_after_each) {
                g.dump();
            }
        }
    }

private:
    std::vector<Pass> passes_;
};

} // namespace loc::ir
