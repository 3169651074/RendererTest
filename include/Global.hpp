#ifndef RENDERERTEST_GLOBAL_HPP
#define RENDERERTEST_GLOBAL_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <cstring>
#include <random>
#include <algorithm>
#include <limits>
#include <array>

#include <mylibrary/lib_sdl.hpp>

#ifdef INFINITY
#undef INFINITY
#endif

namespace renderer {
    // ====== 数值常量 ======
    constexpr double FLOAT_VALUE_ZERO_EPSILON = 1e-5;
    constexpr double INFINITY = std::numeric_limits<double>::infinity();
    constexpr double PI = M_PI;

    // ====== 工具函数 ======
    inline double degreeToRadian(double degree) {
        return degree * PI / 180.0;
    }

    inline double radianToDegree(double radian) {
        return radian * 180.0 / PI;
    }

    //生成一个[0, 1)之间的浮点随机数
    inline double randomDouble() {
        static std::random_device rd; //需要初始化随机设备，让每次运行都能生成不同的随机数
        static std::uniform_real_distribution<> distribution(0.0, 1.0);
        static std::mt19937 generator(rd());
        return distribution(generator);
    }

    //生成一个[min, max)之间的浮点随机数
    inline double randomDouble(double min, double max) {
        return min + (max - min) * randomDouble();
    }

    //生成一个[min, max]之间的整数随机数
    inline int randomInt(int min, int max) {
        //直接使用 randomDouble() 来生成整数，以共享同一个静态生成器
        return static_cast<int>(randomDouble(min, max + 1));
    }

    //判断浮点数是否接近于0
    inline bool floatValueNearZero(double val) {
        return std::abs(val) < FLOAT_VALUE_ZERO_EPSILON;
    }

    //判断两个浮点数是否相等
    inline bool floatValueEquals(double v1, double v2) {
        return std::abs(v1 - v2) < FLOAT_VALUE_ZERO_EPSILON;
    }
}

#endif //RENDERERTEST_GLOBAL_HPP
