#include "util/PerlinGenerator.hpp"

namespace renderer {
    void PerlinGenerator::perlinGenerate(int *arr) {
        for (int i = 0; i < POINT_COUNT; i++) {
            arr[i] = i;
        }

        for (int i = POINT_COUNT - 1; i > 0; i--) {
            const int swap = randomInt(0, i);
            std::swap(arr[i], arr[swap]);
        }
    }

    double PerlinGenerator::perlinNoiseNoSmooth(const Point3 &point) const {
        //放大坐标，并使用按位与限定范围在0到255之间，快速取模运算（x % 256）
        const int i = static_cast<int>(4 * point[0]) & 255;
        const int j = static_cast<int>(4 * point[1]) & 255;
        const int k = static_cast<int>(4 * point[2]) & 255;

        //合成下标
        return randomNumber[perlinX[i] ^ perlinY[j] ^ perlinZ[k]];
    }

    double PerlinGenerator::perlinNoiseLinearSmooth(const Point3 &point) const {
        //点坐标整数部分，代表了该点所在的立方体的“左下角”或起始顶点坐标
        const auto i = static_cast<int>(std::floor(point[0]));
        const auto j = static_cast<int>(std::floor(point[1]));
        const auto k = static_cast<int>(std::floor(point[2]));

        //点坐标小数部分，代表了该点在立方体内部，沿着 x, y, z 轴的相对位置，为之后进行平滑混合的权重
        const auto u = point[0] - std::floor(point[0]);
        const auto v = point[1] - std::floor(point[1]);
        const auto w = point[2] - std::floor(point[2]);

        //获取立方体 8 个顶点的噪声值，三重循环遍历立方体每一个顶点
        double c[2][2][2];
        for (int l = 0; l < 2; l++) {
            for (int m = 0; m < 2; m++) {
                for (int n = 0; n < 2; n++) {
                    /*
                     * i + l, j + m, k + n 计算出了立方体 8 个顶点的整数坐标
                     * & 255 依然是用来确保索引不会超出数组范围
                     * c[l][m][n] 储存这 8 个顶点的噪声值
                     */
                    c[l][m][n] = randomNumber[perlinX[(i + l) & 255] ^ perlinY[(j + m) & 255] ^ perlinZ[(k + n) & 255]];
                }
            }
        }

        //三线性插值 (Trilinear Interpolation)
        //通过加权平均将 8 个顶点的噪声值 c[l][m][n] 混合起来
        double accum = 0.0;
        for (int l = 0; l < 2; l++) {
            for (int m = 0; m < 2; m++) {
                for (int n = 0; n < 2; n++) {
                    //每一次循环，accum 累加的是： x方向权重 * y方向权重 * z方向权重 * 对应顶点的噪声值。
                    accum += (l * u + (1 - l) * (1 - u)) * (m * v + (1 - m) * (1 - v)) * (n * w + (1 - n) * (1 - w)) * c[l][m][n];
                }
            }
        }
        return accum;
    }

    double PerlinGenerator::hermitianSmoothImprove(const Point3 &point) const {
        const auto i = static_cast<int>(std::floor(point[0]));
        const auto j = static_cast<int>(std::floor(point[1]));
        const auto k = static_cast<int>(std::floor(point[2]));

        auto u = point[0] - std::floor(point[0]);
        auto v = point[1] - std::floor(point[1]);
        auto w = point[2] - std::floor(point[2]);

        /*
         * 将线性插值升级为平滑步进插值 (Smoothstep Interpolation)，此运算称为fade函数
         * 线性插值的变化率是恒定的，这会导致一个问题：在每个整数网格的边界处，噪声变化的速率会突然改变，
         * 人眼依然可以察觉到这种不自然的、类似网格的瑕疵。在数学上，这被称为 C¹ 不连续。
         *
         * 新的 "S" 形曲线 y = x*x*(3-2x) 在起点 x = 0 和终点 x = 1处的导数都为0
         *
         * 实现缓入缓出的效果
         * 缓入 (Ease-in)：当一个点刚刚进入一个新的整数立方体时（u, v, w 接近 0），噪声的变化非常缓慢。
         * 缓出 (Ease-out)：当它即将离开这个立方体时（u, v, w 接近 1），噪声的变化也再次变得非常缓慢。
         *
         * 通过在边界处将变化率降为零，它消除了线性插值留下的网格瑕疵，使得不同整数立方体之间的过渡变得无法察觉，噪声看起来更加自然、平滑和有机
         */
        u = u * u * (3 - 2 * u);
        v = v * v * (3 - 2 * v);
        w = w * w * (3 - 2 * w);

        double c[2][2][2];
        for (int l = 0; l < 2; l++) {
            for (int m = 0; m < 2; m++) {
                for (int n = 0; n < 2; n++) {
                    c[l][m][n] = randomNumber[perlinX[(i + l) & 255] ^ perlinY[(j + m) & 255] ^ perlinZ[(k + n) & 255]];
                }
            }
        }

        double accum = 0.0;
        for (int l = 0; l < 2; l++) {
            for (int m = 0; m < 2; m++) {
                for (int n = 0; n < 2; n++) {
                    accum += (l * u + (1 - l) * (1 - u)) * (m * v + (1 - m) * (1 - v)) * (n * w + (1 - n) * (1 - w)) * c[l][m][n];
                }
            }
        }
        return accum;
    }

    double PerlinGenerator::vectorLatticePoint(const Point3 &point) const {
        const auto i = static_cast<int>(std::floor(point[0]));
        const auto j = static_cast<int>(std::floor(point[1]));
        const auto k = static_cast<int>(std::floor(point[2]));

        auto u = point[0] - std::floor(point[0]); u = u * u * (3 - 2 * u);
        auto v = point[1] - std::floor(point[1]); v = v * v * (3 - 2 * v);
        auto w = point[2] - std::floor(point[2]); w = w * w * (3 - 2 * w);

        Vec3 c[2][2][2];
        for (int l = 0; l < 2; l++) {
            for (int m = 0; m < 2; m++) {
                for (int n = 0; n < 2; n++) {
                    c[l][m][n] = randomVector[perlinX[(i + l) & 255] ^ perlinY[(j + m) & 255] ^ perlinZ[(k + n) & 255]];
                }
            }
        }

        double accum = 0.0;
        for (int l = 0; l < 2; l++) {
            for (int m = 0; m < 2; m++) {
                for (int n = 0; n < 2; n++) {
                    const Vec3 weightVector(u - l, v - m, w - n);
                    accum += (l * u + (1 - l) * (1 - u)) * (m * v + (1 - m) * (1 - v)) * (n * w + (1 - n) * (1 - w)) * weightVector.dot(c[l][m][n]);
                }
            }
        }
        return accum;
    }

    double PerlinGenerator::turbulenceNoise(const Point3 &point) const {
        constexpr int depth = 7;

        double accum = 0.0;
        Point3 temp(point);
        double weight = 1.0;

        for (int i = 0; i < depth; i++) {
            accum += weight * vectorLatticePoint(temp);
            weight /= 2.0;
            for (int j = 0; j < 3; j++) {
                temp[j] *= 2.0;
            }
        }
        return std::abs(accum);
    }

    bool PerlinGenerator::equals(const AbstractObject &obj) const {
        if (this == &obj) return true;
        const auto * gen = dynamic_cast<const PerlinGenerator *>(&obj);
        if (gen == null) return false;

        //比较所有成员数组是否相等
        for (size_t i = 0; i < POINT_COUNT; i++) {
            if (!floatValueEquals(randomNumber[i], gen->randomNumber[i]) ||
                randomVector[i] != gen->randomVector[i] ||
                perlinX[i] != gen->perlinX[i] ||
                perlinY[i] != gen->perlinY[i] ||
                perlinZ[i] != gen->perlinZ[i]
            ) return false;
        }
        return true;
    }

    std::string PerlinGenerator::toString() const {
        return "Perlin Noise Generator: " + std::to_string(reinterpret_cast<uintptr_t>(this));
    }
}
