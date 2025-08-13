#ifndef RENDERERTEST_PERLINGENERATOR_HPP
#define RENDERERTEST_PERLINGENERATOR_HPP

#include <basic/Point3.hpp>

namespace renderer {
    /*
     * 柏林噪声工具类，包含不同平滑程度的柏林噪声函数
     */

    //柏林噪声函数的类型
    enum class PerlinNoiseType {
        NO_SMOOTH, TRILINEAR_INTERPOLATION, SMOOTHSTEP_INTERPOLATION,
        RANDOM_VECTOR, RANDOM_TURBULENCE_NET, RANDOM_TURBULENCE_MARBLE
    };

    class PerlinGenerator : public AbstractObject {
    private:
        static constexpr int POINT_COUNT = 256;

        //noise方法返回此数组中的元素，主要随机过程在于决定下标
        double randomNumber[POINT_COUNT];
        Vec3 randomVector[POINT_COUNT]; //用于RANDOM_VECTOR方法

        //置换数组，每个数组里面都储存了从0到255的所有整数，但顺序随机打乱。作为“哈希表”，用于将输入的坐标映射到一个随机但确定的索引上
        int perlinX[POINT_COUNT], perlinY[POINT_COUNT], perlinZ[POINT_COUNT];

        //填充置换数组并打乱（Fisher-Yates洗牌算法）
        static void perlinGenerate(int * arr);

    public:
        PerlinGenerator() {
            for (double & i : randomNumber) {
                i = randomDouble();
            }
            for (auto & i : randomVector) {
                i = Vec3::randomVector(-1.0, 1.0);
            }
            perlinGenerate(perlinX);
            perlinGenerate(perlinY);
            perlinGenerate(perlinZ);
        }
        ~PerlinGenerator() override = default;

        //无平滑的基础噪声
        //无论一个点落在哪个整数立方体区域内，都返回同一个值，导致块状外观
        double perlinNoiseNoSmooth(const Point3 & point) const;

        //线性插值平滑
        //考虑该点所在的整数坐标立方体的所有8个顶点，根据该点在这个立方体内的相对位置
        //将这8个顶点各自的噪声值平滑地混合（插值）在一起得到最终的噪声值
        double perlinNoiseLinearSmooth(const Point3 & point) const;

        //线性差值升级
        double hermitianSmoothImprove(const Point3 & point) const;

        /*
         * 晶格点（整数坐标点）上储存的不再是一个简单的随机数值 (double)，而是一个随机的梯度向量 (Vec3)
         * weightVector是一个距离向量，它从当前计算的立方体顶点 (l, m, n) 指向立方体内部的点 (u, v, w)
         * 将乘法计算修改为计算距离向量和该顶点的梯度向量之间的点积，通过点积运算引入了方向性
         * 此处计算的是在点 (u, v, w) 的位置，受到来自顶点 (l, m, n) 的梯度“推动”的强度
         * 这种方法产生的噪声值不再是孤立的，而是与周围的梯度场相关联。这使得噪声呈现出一种自然的、带有旋转和流动感的纹理，而不是简单的斑点，
         * 从而可以用于模拟云、水、火、地形等自然现象
         *
         * 使用梯度和距离向量的点积可以确保噪声值在每个整数晶格点上都精确为 0，因为在顶点上，距离向量的长度为零，导致点积为零。这进一步增强了噪声的平滑度和连续性
         */
        double vectorLatticePoint(const Point3 & point) const;

        /*
         * 通过叠加多个不同频率和振幅的柏林噪声，来生成一种更复杂、更具细节的噪声，通常被称为湍流 (Turbulence) 或分形噪声 (Fractal Noise)
         * 常被用来模拟云朵、大理石纹理、火焰等不规则但又具有自相似性的自然现象
         *
         * 每一次循环都会叠加一层，但每一层的特性都不同：
         * 频率 (Frequency) 不断增加：在每次循环中，坐标点 temp 的每个分量都会乘以 2。这意味着下一次循环调用噪声函数时，会从一个“放大”两倍的坐标进行采样。
         *  这会使噪声的频率加倍，从而引入更小、更精细的细节。
         * 振幅 (Amplitude) 不断减小：weight（权重）的初始值为 1.0，每次循环后都会减半。这意味着，频率越高的噪声层，其对最终结果的贡献（即“强度”）就越小
         *
         * 通过将这些高频率、低振幅的细节噪声层，与低频率、高振幅的基础噪声层累加在一起，就构建出了一个分形图案：大的、平缓的噪声形态上覆盖着小的、更复杂的细节
         *
         * 最后取绝对值：标准的柏林噪声值范围通常在 [-1, 1] 之间，在最后返回累加结果的绝对值 std::abs()，会把所有负值“翻转”成正值
         * 这会在噪声值为零的地方产生尖锐的“山谷”或“裂缝”，从而形成一种类似大理石纹理或湍流的视觉效果
         */
        double turbulenceNoise(const Point3 & point) const;

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override;
        std::string toString() const override;
    };
}

#endif //RENDERERTEST_PERLINGENERATOR_HPP
