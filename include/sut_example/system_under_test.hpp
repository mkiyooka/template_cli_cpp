#pragma once

#include <vector>

namespace sut_example {

// シンプルな数学関数（double型比較のサンプル用）
double DoubleValue(double input);

// vector型比較のサンプル用関数
std::vector<int> CreateSequence(int start, int count);

// 簡単なクラスのサンプル（剰余計算）
class ModuloCalculator {
public:
    explicit ModuloCalculator(int divisor);

    int GetRemainder(int value) const;

private:
    int divisor_;
};

} // namespace sut_example