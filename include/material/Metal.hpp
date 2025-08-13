#ifndef RENDERERTEST_METAL_HPP
#define RENDERERTEST_METAL_HPP

#include <material/AbstractMaterial.hpp>

namespace renderer {
    /*
     * 金属材质类，使用基础颜色albedo和反射扰动fuzz参数定义
     */
    class Metal final : public AbstractMaterial {
    private:
        Color3 albedo;
        double fuzz;

    public:
        explicit Metal(const Color3 & albedo = Color3(1.0, 1.0, 1.0), double fuzz = 0.0) :
            albedo(albedo), fuzz(fuzz)
        {
            this->fuzz = Range(0.0, 1.0).clamp(fuzz);
        }
        ~Metal() override = default;

        //金属材质不吸收光线，完全反射光线
        bool scatter(const Ray & in, const HitRecord & record, ScatterRecord & scatterRecord) const override {
            //计算反射光线方向向量（单位向量）
            const Vec3 v = in.getDirection();
            const Vec3 n = record.normalVector;
            Vec3 reflectDirection = (v - 2 * Vec3::dot(v, n) * n).unitVector();

            //应用反射扰动：在距离物体表面1单位处随机选取单位向量和反射向量相加，形成随机扰动
            if (fuzz > 0.0) {
                reflectDirection += fuzz * Vec3::randomSpaceVector(1.0);
            }
//
//            //构建反射光线，光线的时间属性不随传播而改变
//            out = Ray(record.hitPoint, reflectDirection, in.getTime());
//            attenuation = albedo;
//
//            //防止由于计算精度导致out方向向内。使用点积检查反射光线是否和物体外法线的同侧，仅当二者同侧时有效
//            return Vec3::dot(out.getDirection(), record.normalVector) > 0.0;

            scatterRecord.attenuation = albedo;
            scatterRecord.skipPDFRay = Ray(record.hitPoint, reflectDirection, in.getTime());
            scatterRecord.isSkipPDF = true;
            scatterRecord.pdf = null;
            return Vec3::dot(reflectDirection, record.normalVector) > 0.0;
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * metal = dynamic_cast<const Metal *>(&obj);
            if (metal == null) return false;
            return albedo == metal->albedo && fuzz == metal->fuzz;
        }

        std::string toString() const override {
            char buffer[TOSTRING_BUFFER_SIZE] = { 0 };
            snprintf(buffer, TOSTRING_BUFFER_SIZE, "Metal: Albedo = %s, Fuzz = %.4lf", albedo.toString().c_str(), fuzz);
            return {buffer};
        }
    };
}

#endif //RENDERERTEST_METAL_HPP
