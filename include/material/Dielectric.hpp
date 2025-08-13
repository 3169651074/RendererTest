#ifndef RENDERERTEST_DIELECTRIC_HPP
#define RENDERERTEST_DIELECTRIC_HPP

#include <material/AbstractMaterial.hpp>

namespace renderer {
    /*
     * 电介质材质：折射光线或全反射，通过折射率参数控制
     * 设定由空气（折射率1.0）入射
     */
    class Dielectric final : public AbstractMaterial {
    private:
        Color3 albedo;
        double refractiveIndex;

        //使用Schlick近似计算反射率
        static double reflectance(double cosine, double refractiveIndex) {
            double r0 = (1.0 - refractiveIndex) / (1.0 + refractiveIndex);
            r0 = r0 * r0;
            return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5.0);
        }

        //计算折射光线，要求i和n都是单位向量，需要根据光线的入射方向决定相对折射率
        Vec3 refract(const Vec3 & i, const Vec3 & n, bool isFrontFace) const {
            const double cosTheta = Vec3::dot(-i, n);
            const double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
            const double rate = isFrontFace ? 1.0 / refractiveIndex : refractiveIndex * 1.0; //根据入射方向确定折射率

            //确定是否发生全反射
            if (sinTheta * rate > 1.0 || reflectance(cosTheta, refractiveIndex) > randomDouble()) {
                //全反射
                return i - 2 * Vec3::dot(i, n) * n;
            } else {
                //折射
                const Vec3 perpendicular = rate * (i + cosTheta * n); //和法向量垂直的折射分量
                const Vec3 parallel = -std::sqrt(std::abs(1.0 - perpendicular.lengthSquare())) * n;
                return perpendicular + parallel;
            }
        }

    public:
        explicit Dielectric(double refractiveIndex = 1.0) :
                //材质不吸收光
                albedo(Color3(1.0, 1.0, 1.0)), refractiveIndex(refractiveIndex) {}
        ~Dielectric() override = default;

        bool scatter(const Ray &in, const HitRecord &record, ScatterRecord & scatterRecord) const override {
            //单位化输入向量
            const Vec3 i = in.getDirection().unitVector();
            //计算折射向量
            const Vec3 r = refract(i, record.normalVector, record.hitFrontFace);
            //构造折射光线
//            out = Ray(record.hitPoint, r.unitVector(), in.getTime());
//            attenuation = albedo;
            scatterRecord.attenuation = albedo;
            scatterRecord.skipPDFRay = Ray(record.hitPoint, r.unitVector(), in.getTime());
            scatterRecord.isSkipPDF = true;
            scatterRecord.pdf = null;
            return true;
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * dielectric = dynamic_cast<const Dielectric *>(&obj);
            if (dielectric == null) return false;
            return albedo == dielectric->albedo && refractiveIndex == dielectric->refractiveIndex;
        }

        std::string toString() const override {
            char buffer[TOSTRING_BUFFER_SIZE] = { 0 };
            snprintf(buffer, TOSTRING_BUFFER_SIZE, "Dielectric: Albedo = %s, Refractive Index = %.4lf", albedo.toString().c_str(), refractiveIndex);
            return {buffer};
        }
    };
}

#endif //RENDERERTEST_DIELECTRIC_HPP
