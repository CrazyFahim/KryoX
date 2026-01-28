#pragma once
#include "loc/runtime/matrix.hpp"
#include <string>
#include <unordered_map>

namespace loc::rt {

// Minimal operator registry: name -> Matrix
class Registry {
public:
    void set(std::string name, Matrix m);
    const Matrix& get(const std::string& name) const;

private:
    std::unordered_map<std::string, Matrix> ops_;
};

} // namespace loc::rt
