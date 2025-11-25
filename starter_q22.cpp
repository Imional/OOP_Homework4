#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <concepts>

// TODO: Define a concept named 'Numeric' to ensure T is an arithmetic type (supports +, -, *, etc.)
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

// TODO: Implement a templated class `Matrix`.
template <Numeric T>
class Matrix {
private:
    std::vector<T> data_;
    size_t rows_;
    size_t cols_;

public:
    // (From Q2.1) Constructors and Getters
    Matrix(size_t r, size_t c) : rows_(r), cols_(c), data_(r* c) {}
    Matrix() : rows_(0), cols_(0) {}
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    // (From Q2.1) Element access
    T& operator()(size_t r, size_t c) {
        if (r >= rows_ || c >= cols_) throw std::out_of_range("Index out of bounds");
        return data_[r * cols_ + c];
    }
    const T& operator()(size_t r, size_t c) const {
        if (r >= rows_ || c >= cols_) throw std::out_of_range("Index out of bounds");
        return data_[r * cols_ + c];
    }

    // (From Q2.1) Addition
    Matrix<T> operator+(const Matrix<T>& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_) throw std::invalid_argument("Dimension mismatch (+)");
        Matrix<T> result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) result.data_[i] = data_[i] + other.data_[i];
        return result;
    }

    // (From Q2.1) Subtraction
    Matrix<T> operator-(const Matrix<T>& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_) throw std::invalid_argument("Dimension mismatch (-)");
        Matrix<T> result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) result.data_[i] = data_[i] - other.data_[i];
        return result;
    }

    // (7) Overloaded operator^ for element-wise multiplication (A ^ B)
    Matrix<T> operator^(const Matrix<T>& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw std::invalid_argument("Dimension mismatch for element-wise multiplication");
        }
        Matrix<T> result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] * other.data_[i];
        }
        return result;
    }

    // (8) Overloaded operator* for standard matrix multiplication (A * B)
    Matrix<T> operator*(const Matrix<T>& other) const {
        if (cols_ != other.rows_) {
            throw std::invalid_argument("Dimension mismatch for matrix multiplication");
        }
        Matrix<T> result(rows_, other.cols_);
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < other.cols_; ++j) {
                T sum = 0;
                for (size_t k = 0; k < cols_; ++k) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    // 2.3 EXTRA CREDIT: Matrix Inverse for 3x3 Matrix
    Matrix<T> inverse() const {
        if (rows_ != 3 || cols_ != 3) {
            throw std::invalid_argument("Inverse restricted to 3x3 matrices");
        }
        const Matrix<T>& m = *this;
        T a = m(0, 0), b = m(0, 1), c = m(0, 2);
        T d = m(1, 0), e = m(1, 1), f = m(1, 2);
        T g = m(2, 0), h = m(2, 1), i = m(2, 2);

        // Determinant
        T det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
        if (std::abs(det) < 1e-9) throw std::runtime_error("Matrix is singular");

        T invDet = static_cast<T>(1.0) / det;
        Matrix<T> res(3, 3);

        // Adjugate matrix (Transpose of Cofactors) * (1/det)
        res(0, 0) = (e * i - f * h) * invDet;  res(0, 1) = -(b * i - c * h) * invDet; res(0, 2) = (b * f - c * e) * invDet;
        res(1, 0) = -(d * i - f * g) * invDet; res(1, 1) = (a * i - c * g) * invDet;  res(1, 2) = -(a * f - c * d) * invDet;
        res(2, 0) = (d * h - e * g) * invDet;  res(2, 1) = -(a * h - b * g) * invDet; res(2, 2) = (a * e - b * d) * invDet;
        return res;
    }

    // Utility function to print the matrix to the standard output.
    void print(const std::string& name = "") const {
        if (!name.empty()) {
            std::cout << name << " (" << rows_ << "x" << cols_ << "):" << std::endl;
        }
        std::cout << std::fixed << std::setprecision(1);
        for (size_t r = 0; r < rows_; ++r) {
            std::cout << "[ ";
            for (size_t c = 0; c < cols_; ++c) {
                std::cout << std::setw(6) << std::left << (*this)(r, c) << " ";
            }
            std::cout << "]" << std::endl;
        }
        std::cout << std::endl;
    }
};

int main() {
    // Create Matrix<double> A and B.
    Matrix<double> A(2, 3);
    Matrix<double> B(2, 3);

    // Initialize A (2x3)
    A(0, 0) = 1.0; A(0, 1) = 2.0; A(0, 2) = 3.0;
    A(1, 0) = 4.0; A(1, 1) = 5.0; A(1, 2) = 6.0;

    // Initialize B (2x3)
    B(0, 0) = 0.5; B(0, 1) = 0.5; B(0, 2) = 0.5;
    B(1, 0) = 2.0; B(1, 1) = 2.0; B(1, 2) = 2.0;

    // Display A and B.
    A.print("Matrix A");
    B.print("Matrix B");

    // Compute E as the element-wise multiplication of A and B.
    Matrix<double> E = A ^ B;

    // Create and initialize F (3x2)
    Matrix<double> F(3, 2);
    F(0, 0) = 1.0; F(0, 1) = 2.0;
    F(1, 0) = 1.0; F(1, 1) = 2.0;
    F(2, 0) = 1.0; F(2, 1) = 2.0;

    // Compute G as the standard matrix multiplication of A and F.
    Matrix<double> G = A * F;

    // Display E, F, G.
    E.print("Matrix E (A ^ B)");
    F.print("Matrix F");
    G.print("Matrix G (A * F)");

    // --- EXTRA CREDIT TEST ---
    std::cout << "--- Extra Credit Check ---" << std::endl;
    Matrix<double> X(3, 3);
    X(0, 0) = 3; X(0, 1) = 0; X(0, 2) = 2;
    X(1, 0) = 2; X(1, 1) = 0; X(1, 2) = -2;
    X(2, 0) = 0; X(2, 1) = 1; X(2, 2) = 1;

    try {
        Matrix<double> X_inv = X.inverse();
        X_inv.print("X Inverse");
        (X * X_inv).print("Identity Check (X * X_inv)");
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}