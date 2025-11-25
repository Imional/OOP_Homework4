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
    // data_: a std::vector<T> that stores the matrix elements flattened from 2D to 1D (row-major).
    std::vector<T> data_;
    // rows_, cols_: size_t variables representing the number of rows and columns.
    size_t rows_;
    size_t cols_;

public:
    // (2) Parameterized constructor: creates and initializes an r-by-c matrix.
    Matrix(size_t r, size_t c) : rows_(r), cols_(c), data_(r* c) {}

    // (1) Default constructor: initializes rows and cols to zero.
    Matrix() : rows_(0), cols_(0) {}

    // (3) Const getter methods rows() and cols() for retrieving dimensions.
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    // (4) Overloaded operator() for element access (non-const)
    T& operator()(size_t r, size_t c) {
        if (r >= rows_ || c >= cols_) {
            throw std::out_of_range("Index out of bounds");
        }
        return data_[r * cols_ + c];
    }

    // (4) Overloaded operator() for element access (const)
    const T& operator()(size_t r, size_t c) const {
        if (r >= rows_ || c >= cols_) {
            throw std::out_of_range("Index out of bounds");
        }
        return data_[r * cols_ + c];
    }

    // (5) Overloaded operator+ for addition (A + B)
    Matrix<T> operator+(const Matrix<T>& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw std::invalid_argument("Dimension mismatch for addition");
        }
        Matrix<T> result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] + other.data_[i];
        }
        return result;
    }

    // (6) Overloaded operator- for subtraction (A - B)
    Matrix<T> operator-(const Matrix<T>& other) const {
        if (rows_ != other.rows_ || cols_ != other.cols_) {
            throw std::invalid_argument("Dimension mismatch for subtraction");
        }
        Matrix<T> result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] - other.data_[i];
        }
        return result;
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
                std::cout << std::setw(4) << std::left << (*this)(r, c) << " ";
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

    // Initialize A as you prefer:
    A(0, 0) = 1.0; A(0, 1) = 2.0; A(0, 2) = 3.0;
    A(1, 0) = 4.0; A(1, 1) = 5.0; A(1, 2) = 6.0;

    // Initialize B as you prefer:
    B(0, 0) = 0.5; B(0, 1) = 0.5; B(0, 2) = 0.5;
    B(1, 0) = 2.0; B(1, 1) = 2.0; B(1, 2) = 2.0;

    // Display A and B.
    A.print("Matrix A");
    B.print("Matrix B");

    // Compute C as the sum of A and B.
    Matrix<double> C = A + B;

    // Compute D as the difference between A and B.
    Matrix<double> D = A - B;

    // Display C and D.
    C.print("Matrix C (A + B)");
    D.print("Matrix D (A - B)");

    return 0;
}