#include "math/math.h"
#include <ctime>

namespace math {
namespace {
// ToDo: not nearest number?
NumT GetNearestPrimeNumber(NumT num)
{
    bool is_prime = false;
    for (; !is_prime; num++) {
        is_prime = true;
        for (NumT i = 2; i * i <= num && is_prime; i++)
            if (num % i == 0)
                is_prime = false;
    }
    return num - 1;
}

// ToDo: extended Euclid algorithm?
NumT GetInverse(NumT num, NumT p)
{
    NumT result = 1;
    bool sign = num < 0;
    num = std::abs(num);
    for (NumT i = 0; i < p - 2; i++)
        result = result * num % p;
    return sign ? p - result : result;
}

Polynomial XMinusNumber(NumT num, NumT p)
{
    Polynomial result(p, 1);
    result[0] = (-num % p + p) % p;
    result[1] = 1;
    return result;
}

}  // namespace

Polynomial operator+(const Polynomial &left, const Polynomial &right)
{
    Polynomial result = left;
    result += right;
    return result;
}

Polynomial operator-(const Polynomial &left, const Polynomial &right)
{
    Polynomial result = left;
    result -= right;
    return result;
}

Polynomial operator*(const Polynomial &left, const Polynomial &right)
{
    Polynomial result = left;
    result *= right;
    return result;
}

SecretInfo ShareSecret(NumT secret, NumT k, NumT n)
{
    ASSERT_PRINT(k > 0, "incorrect parameter k!");
    ASSERT_PRINT(n > 0, "incorrect parameter n!");
    std::srand(std::time(nullptr));

    NumT p = GetNearestPrimeNumber(secret + 1);
    SecretInfo result(p, k);
    Polynomial poly(p, k - 1);
    poly.FillRandom();
    poly[0] = secret;

    for (NumT point = 1; point <= n; point++)
        result.points_.push_back({point, poly.GetValue(point)});
    return result;
}

Polynomial GetSecret(SecretInfo secrets)
{
    ASSERT_PRINT(secrets.points_.size() >= secrets.k_, "incorrect secrets!");
    Polynomial result(secrets.p_, secrets.k_ - 1);

    for (size_t i = 0; i < secrets.k_; i++) {
        Polynomial l_i(secrets.p_, 1);
        l_i[0] = 1;
        NumT coef = 1;
        for (size_t j = 0; j < secrets.k_; j++) {
            if (i == j)
                continue;

            l_i *= XMinusNumber(secrets.points_[j].key_, secrets.p_);
            coef *= secrets.points_[i].key_ - secrets.points_[j].key_;
        }
        l_i *= GetInverse(coef, secrets.p_);
        l_i *= secrets.points_[i].value_;
        result += l_i;
    }

    return result;
}

}  // namespace math
