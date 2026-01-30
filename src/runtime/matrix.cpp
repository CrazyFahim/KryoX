#include "loc/runtime/matrix.hpp"
#include <stdexcept>
#include <iomanip>

namespace loc::rt {

Matrix::Matrix(std::size_t r, std::size_t c, double fill)
    : r_(r), c_(c), data_(r * c, fill) {}

Matrix Matrix::identity(std::size_t n) {
    Matrix I(n, n, 0.0);
    for (std::size_t i = 0; i < n; ++i) I(i, i) = 1.0;
    return I;
}

double& Matrix::operator()(std::size_t i, std::size_t j) {
    if (i >= r_ || j >= c_) throw std::out_of_range("Matrix index out of range");
    return data_[i * c_ + j];
}

double Matrix::operator()(std::size_t i, std::size_t j) const {
    if (i >= r_ || j >= c_) throw std::out_of_range("Matrix index out of range");
    return data_[i * c_ + j];
}

Matrix operator+(const Matrix& a, const Matrix& b) {
    if (a.rows() != b.rows() || a.cols() != b.cols())
        throw std::runtime_error("Matrix add: shape mismatch");

    Matrix out(a.rows(), a.cols(), 0.0);
    for (std::size_t i = 0; i < a.rows(); ++i)
        for (std::size_t j = 0; j < a.cols(); ++j)
            out(i, j) = a(i, j) + b(i, j);
    return out;
}

Matrix operator*(double s, const Matrix& a) {
    Matrix out(a.rows(), a.cols(), 0.0);
    for (std::size_t i = 0; i < a.rows(); ++i)
        for (std::size_t j = 0; j < a.cols(); ++j)
            out(i, j) = s * a(i, j);
    return out;
}

Matrix Matrix::matmul(const Matrix& b) const {
    if (c_ != b.rows())
        throw std::runtime_error("Matrix matmul: shape mismatch");

    Matrix out(r_, b.cols(), 0.0);

    for (std::size_t i = 0; i < r_; ++i) {
        for (std::size_t k = 0; k < c_; ++k) {
            double aik = (*this)(i, k);
            for (std::size_t j = 0; j < b.cols(); ++j) {
                out(i, j) += aik * b(k, j);
            }
        }
    }
    return out;
}

std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    os << std::fixed << std::setprecision(3);
    for (std::size_t i = 0; i < m.rows(); ++i) {
        os << "[ ";
        for (std::size_t j = 0; j < m.cols(); ++j) {
            os << m(i, j);
            if (j + 1 < m.cols()) os << ", ";
        }
        os << " ]\n";
    }
    return os;
}

} // namespace loc::rt
