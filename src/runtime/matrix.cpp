#include "loc/runtime/matrix.hpp"
#include <stdexcept>
#include <iomanip>

namespace loc::rt {

Matrix::Matrix(std::size_t r, std::size_t c, double fill)
    : r_(r), c_(c), data_(r * c, fill) {}

Matrix Matrix::identity(std::size_t n) {
    Matrix m(n, n, 0.0);
    for (std::size_t i = 0; i < n; ++i) m(i, i) = 1.0;
    return m;
}

double& Matrix::operator()(std::size_t i, std::size_t j) {
    return data_.at(i * c_ + j);
}
double Matrix::operator()(std::size_t i, std::size_t j) const {
    return data_.at(i * c_ + j);
}

Matrix operator+(const Matrix& a, const Matrix& b) {
    if (a.r_ != b.r_ || a.c_ != b.c_)
        throw std::runtime_error("Matrix +: shape mismatch");

    Matrix out(a.r_, a.c_);
    for (std::size_t i = 0; i < a.data_.size(); ++i)
        out.data_[i] = a.data_[i] + b.data_[i];
    return out;
}

Matrix operator*(double s, const Matrix& a) {
    Matrix out(a.r_, a.c_);
    for (std::size_t i = 0; i < a.data_.size(); ++i)
        out.data_[i] = s * a.data_[i];
    return out;
}

Matrix matmul(const Matrix& a, const Matrix& b) {
    if (a.c_ != b.r_)
        throw std::runtime_error("Matrix matmul: shape mismatch");

    Matrix out(a.r_, b.c_, 0.0);
    for (std::size_t i = 0; i < a.r_; ++i) {
        for (std::size_t k = 0; k < a.c_; ++k) {
            const double aik = a(i, k);
            for (std::size_t j = 0; j < b.c_; ++j) {
                out(i, j) += aik * b(k, j);
            }
        }
    }
    return out;
}

std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    os << std::fixed << std::setprecision(3);
    for (std::size_t i = 0; i < m.r_; ++i) {
        os << "[ ";
        for (std::size_t j = 0; j < m.c_; ++j) {
            os << m(i, j);
            if (j + 1 < m.c_) os << ", ";
        }
        os << " ]\n";
    }
    return os;
}

} // namespace loc::rt
