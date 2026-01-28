#include "loc/runtime/registry.hpp"
#include <stdexcept>

namespace loc::rt {

void Registry::set(std::string name, Matrix m) {
    ops_[std::move(name)] = std::move(m);
}

const Matrix& Registry::get(const std::string& name) const {
    auto it = ops_.find(name);
    if (it == ops_.end()) {
        throw std::runtime_error("Registry: unknown operator '" + name + "'");
    }
    return it->second;
}

} // namespace loc::rt
