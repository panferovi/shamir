#include "math/math.h"
#include <iostream>

void test1()
{
    std::cout << "Test 1:\n";
    math::SecretInfo secrets(11, 3);
    secrets.points_ = {{3, 6}, {5, 3}, {6, 9}, {8, 3}};
    auto poly = math::GetSecretPolynomial(secrets);
    for (const auto &elem : *poly)
        std::cout << elem << " ";  // 4 6 8
    std::cout << "\n";
}

void test2()
{
    std::cout << "Test 2:\n";
    auto share = math::ShareSecret(16, 5, 5);
    auto secret = math::GetSecret(share);
    std::cout << *secret << "\n";  // 16
}

int main()
{
    test1();
    test2();
}
