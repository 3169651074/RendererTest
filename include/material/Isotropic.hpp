#ifndef RENDERERTEST_ISOTROPIC_HPP
#define RENDERERTEST_ISOTROPIC_HPP

#include <material/AbstractMaterial.hpp>
#include <util/UniformPDF.hpp>

namespace renderer {
    /*
     * isotropic：各向同性
     * 体积雾材质：光线和此介质碰撞时，会向所有方向均匀地随机散射
     */
    class Isotropic : public AbstractMaterial {
    private:
        Color3 albedo;

    public:
        explicit Isotropic(const Color3 & albedo) : albedo(albedo) {}
        ~Isotropic() override = default;

        //体积雾不关心入射光线和法线，只是将光线衰减（乘以它的 albedo），然后返回一个指向随机方向的新光线
        bool scatter(const Ray &in, const HitRecord &record, ScatterRecord & scatterRecord) const override {
            /*out = Ray(record.hitPoint, Vec3::randomSpaceVector(1.0), in.getTime());
            attenuation = albedo;
            pdfValue = 1.0 / (4.0 * PI);*/
            scatterRecord.attenuation = albedo;
            scatterRecord.pdf = std::make_shared<UniformPDF>();
            scatterRecord.isSkipPDF = false;
            return true;
        }

        double scatterPDF(const Ray &in, const HitRecord &record, const Ray &out) const override {
            return 1.0 / (4.0 * PI);
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * isotropic = dynamic_cast<const Isotropic *>(&obj);
            if (isotropic == null) return false;
            return albedo == isotropic->albedo;
        }

        std::string toString() const override {
            std::string ret("Isotropic: Albedo = ");
            return ret + albedo.toString();
        }
    };
}

#endif //RENDERERTEST_ISOTROPIC_HPP
