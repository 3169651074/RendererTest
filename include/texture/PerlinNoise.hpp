#ifndef RENDERERTEST_PERLINNOISE_HPP
#define RENDERERTEST_PERLINNOISE_HPP

#include <texture/AbstractTexture.hpp>
#include <util/PerlinGenerator.hpp>

namespace renderer {
    /*
     * 柏林噪声纹理，根据选定的噪声函数随机生成纹理
     */
    class PerlinNoise final : public AbstractTexture {
    private:
        //噪声生成器对象
        PerlinGenerator generator;

        //当前纹理选定的噪声函数
        PerlinNoiseType type;
        double scale;

    public:
        //scale越大，条纹越密集
        explicit PerlinNoise(double scale = 1.0, const PerlinNoiseType type = PerlinNoiseType::SMOOTHSTEP_INTERPOLATION)
            : generator(PerlinGenerator()), scale(scale), type(type) {}
        ~PerlinNoise() override = default;

        //根据选定的类型调用对应的生成函数
        Color3 value(const std::pair<double, double> &uvPair, const Point3 &point) const override {
            const Point3 scaledPoint(scale * point[0], scale * point[1], scale * point[2]);

            switch (type) {
                case PerlinNoiseType::NO_SMOOTH:
                    return Color3(1.0, 1.0, 1.0) * generator.perlinNoiseNoSmooth(scaledPoint);
                case PerlinNoiseType::TRILINEAR_INTERPOLATION:
                    return Color3(1.0, 1.0, 1.0) * generator.perlinNoiseLinearSmooth(scaledPoint);
                case PerlinNoiseType::SMOOTHSTEP_INTERPOLATION:
                default:
                    return Color3(1.0, 1.0, 1.0) * generator.hermitianSmoothImprove(scaledPoint);
                case PerlinNoiseType::RANDOM_VECTOR:
                    return Color3(1.0, 1.0, 1.0) * 0.5 * (1.0 + generator.vectorLatticePoint(scaledPoint));
                case PerlinNoiseType::RANDOM_TURBULENCE_NET:
                    return Color3(1.0, 1.0, 1.0) * generator.turbulenceNoise(point);
                case PerlinNoiseType::RANDOM_TURBULENCE_MARBLE:
                    return Color3(0.5, 0.5, 0.5) * (1.0 + std::sin(scale * point[2] + 10.0 * generator.turbulenceNoise(point)));
            }
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * gen = dynamic_cast<const PerlinNoise *>(&obj);
            return generator == gen->generator && type == gen->type && scale == gen->scale;
        }

        std::string toString() const override {
            std::string ret("Perlin Texture: Type: ");
            const char * typeName; //没有直接获取枚举常量名的方式，手动映射
            switch (type) {
                case PerlinNoiseType::NO_SMOOTH:                 typeName =  "NO_SMOOTH";                break;
                case PerlinNoiseType::TRILINEAR_INTERPOLATION:   typeName =  "TRILINEAR_INTERPOLATION";  break;
                case PerlinNoiseType::SMOOTHSTEP_INTERPOLATION:  typeName =  "SMOOTHSTEP_INTERPOLATION"; break;
                case PerlinNoiseType::RANDOM_VECTOR:             typeName =  "RANDOM_VECTOR";            break;
                case PerlinNoiseType::RANDOM_TURBULENCE_NET:     typeName =  "RANDOM_TURBULENCE_NET";    break;
                case PerlinNoiseType::RANDOM_TURBULENCE_MARBLE:  typeName =  "RANDOM_TURBULENCE_MARBLE"; break;
                default:                                         typeName =  "Unknown";
            }
            ret += typeName;
            ret += ", Scale: " + std::to_string(scale);
            return ret;
        }
    };
}

#endif //RENDERERTEST_PERLINNOISE_HPP
