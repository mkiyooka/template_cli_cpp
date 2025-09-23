#include "sut_example/system_under_test.hpp"

namespace sut_example {

// シンプルな数学関数の実装（double型比較のサンプル用）
double DoubleValue(double input) {
    return input * 2.0;
}

// vector型比較のサンプル用関数
std::vector<int> CreateSequence(int start, int count) {
    std::vector<int> result;
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
        result.push_back(start + i);
    }
    return result;
}

// ModuloCalculator クラスの実装
ModuloCalculator::ModuloCalculator(int divisor) : divisor_(divisor) {}

int ModuloCalculator::GetRemainder(int value) const {
    return value % divisor_;
}

} // namespace sut_example