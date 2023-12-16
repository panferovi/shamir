#ifndef MATH_H
#define MATH_H

#include <vector>
#include <functional>
#include <optional>

namespace math {

using NumT = long long;

// polynomial in field p
class Polynomial {
    NumT p_;
    std::vector<NumT> coefs_ {0};
    Polynomial &AddLikeAction(const Polynomial &other, std::function<NumT(NumT, NumT)> op);

public:
    Polynomial(NumT p, size_t max_power) : p_(p), coefs_(max_power + 1) {}

    Polynomial &operator+=(const Polynomial &other)
    {
        return AddLikeAction(other, std::plus<NumT>());
    }

    Polynomial &operator-=(const Polynomial &other)
    {
        return AddLikeAction(other, std::minus<NumT>());
    }

    Polynomial &operator*=(const Polynomial &other);

    Polynomial &operator*=(NumT coef)
    {
        for (auto &elem : coefs_)
            elem = elem * coef % p_;
        return *this;
    }

    NumT GetValue(NumT point) const
    {
        NumT power_point = 1, result = 0;
        for (const auto &elem : coefs_) {
            result = (result + elem * power_point) % p_;
            power_point = power_point * point % p_;
        }
        return result;
    }

    void SetSize(size_t size)
    {
        coefs_.resize(size);
    }

    void FillRandom();

    void Canonicalize()
    {
        for (auto &elem : coefs_)
            elem = (elem % p_ + p_) % p_;
    }

    NumT &operator[](size_t index)
    {
        return coefs_[index];
    }

    auto begin()
    {
        return coefs_.begin();
    }

    auto end()
    {
        return coefs_.end();
    }
};

Polynomial operator+(const Polynomial &left, const Polynomial &right);
Polynomial operator-(const Polynomial &left, const Polynomial &right);
Polynomial operator*(const Polynomial &left, const Polynomial &right);

struct SecretPiece {
    NumT key_, value_;
};

/// p_ - prime number for field
/// k_ - minimum number of participants for decryption
struct SecretInfo {
    NumT p_, k_;
    std::vector<SecretPiece> points_;
    SecretInfo(NumT p, NumT k) : p_(p), k_(k) {}
};

SecretInfo ShareSecret(NumT secret, NumT k, NumT n);
std::optional<Polynomial> GetSecretPolynomial(SecretInfo secrets);
std::optional<NumT> GetSecret(SecretInfo secrets);

}  // namespace math

#endif  // MATH_H
