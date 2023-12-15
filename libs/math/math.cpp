#include "math/math.h"
#include "utils/macros.h"
#include <random>
#include <chrono>

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

Polynomial &Polynomial::AddLikeAction(const Polynomial &other, std::function<NumT(NumT, NumT)> op)
{
    ASSERT_PRINT(p_ == other.p_, "polynomials from different fields!");
    size_t other_size = other.coefs_.size();
    if (other_size > coefs_.size())
        coefs_.resize(other_size, 0);
    for (size_t power = 0; power < std::min(other_size, coefs_.size()); power++)
        coefs_[power] = op(coefs_[power], other.coefs_[power]) % p_;
    return *this;
}

Polynomial &Polynomial::operator*=(const Polynomial &other)
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

void Polynomial::FillRandom()
{
    static std::mt19937_64 rand_gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    for (auto &elem : coefs_)
        elem = rand_gen() % p_;
    auto &last = *coefs_.rbegin();
    if (last == 0)
        last = 1;
}

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

/// https://en.wikipedia.org/wiki/Shamir's_secret_sharing
SecretInfo ShareSecret(NumT secret, NumT k, NumT n)
{
    ASSERT_PRINT(k > 0, "incorrect parameter k!");
    ASSERT_PRINT(n > 0, "incorrect parameter n!");

    NumT p = GetNearestPrimeNumber(secret + 1);
    SecretInfo result(p, k);
    Polynomial poly(p, k - 1);
    poly.FillRandom();
    poly[0] = secret;

    for (NumT point = 1; point <= n; point++)
        result.points_.push_back({point, poly.GetValue(point)});
    return result;
}

/// https://en.wikipedia.org/wiki/Shamir's_secret_sharing
std::optional<Polynomial> GetSecretPolynomial(SecretInfo secrets)
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

    // for (const auto &piece : secrets.)

    return result;
}

std::optional<NumT> GetSecret(SecretInfo secrets)
{
    auto poly = GetSecretPolynomial(std::move(secrets));
    if (poly)
        return (*poly)[0];
    return std::nullopt;
}

}  // namespace math
