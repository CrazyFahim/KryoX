#pragma once
#include <vector>
#include <cstddef>
#include <ostream>

namespace loc::rt {

class Matrix {
public:
    Matrix() = default;
    Matrix(std::size_t r, std::size_t c, double fill = 0.0);

    static Matrix identity(std::size_t n);

    std::size_t rows() const { return r_; }
    std::size_t cols() const { return c_; }

    double& operator()(std::size_t i, std::size_t j);
    double  operator()(std::size_t i, std::size_t j) const;

    // ops
    friend Matrix operator+(const Matrix& a, const Matrix& b);
    friend Matrix operator*(double s, const Matrix& a);
    friend Matrix operator*(const Matrix& a, double s) { return s * a; }
    friend Matrix matmul(const Matrix& a, const Matrix& b);

    friend std::ostream& operator<<(std::ostream& os, const Matrix& m);

private:
    std::size_t r_{0}, c_{0};
    std::vector<double> data_;
};

} // namespace loc::rt
