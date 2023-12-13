#ifndef MATH_H
#define MATH_H

#include "utils/macros.h"
#include <vector>
#include <functional>
#include <random>

namespace math {

using NumT = long long;

// polynomial in field p
class Polynomial {
    NumT p_;
    std::vector<NumT> coefs_ {0};

    Polynomial &AddLikeAction(const Polynomial &other, std::function<NumT(NumT, NumT)> op)
    {
        ASSERT_PRINT(p_ == other.p_, "polynomials from different fields!");
        size_t other_size = other.coefs_.size();
        if (other_size > coefs_.size())
            coefs_.resize(other_size, 0);
        for (size_t power = 0; power < std::min(other_size, coefs_.size()); power++)
            coefs_[power] = op(coefs_[power], other.coefs_[power]) % p_;
        return *this;
    }

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

    Polynomial &operator*=(const Polynomial &other)
    {
        ASSERT_PRINT(p_ == other.p_, "polynomials from different fields!");
        size_t my_size = coefs_.size();
        size_t other_size = other.coefs_.size();
        std::vector<NumT> new_coefs(my_size + other_size - 1);
        for (size_t i_my = 0; i_my < my_size; i_my++)
            for (size_t i_other = 0; i_other < other_size; i_other++)
                new_coefs[i_my + i_other] = (new_coefs[i_my + i_other] + coefs_[i_my] * other.coefs_[i_other]) % p_;
        coefs_ = std::move(new_coefs);
        return *this;
    }

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

    void FillRandom()
    {
        for (auto &elem : coefs_)
            elem = std::rand() % p_;
        auto &last = *coefs_.rbegin();
        if (last == 0)
            last = 1;
    }

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

struct SectetPiece {
    NumT key_, value_;
};

/// p_ - prime number for field
/// k_ - minimum number of participants for decryption
struct SecretInfo {
    NumT p_, k_;
    std::vector<SectetPiece> points_;
    SecretInfo(NumT p, NumT k) : p_(p), k_(k) {}
};

SecretInfo ShareSecret(NumT secret, NumT k, NumT n);
Polynomial GetSecret(SecretInfo secrets);

}  // namespace math

#endif  // MATH_H