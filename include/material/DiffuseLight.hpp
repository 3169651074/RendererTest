#ifndef RENDERERTEST_DIFFUSELIGHT_HPP
#define RENDERERTEST_DIFFUSELIGHT_HPP

#include <material/AbstractLight.hpp>

namespace renderer {
    /*
     * 漫发光材质，发出均匀光线
     */
    class DiffuseLight : public AbstractLight {
    private:
        Color3 light;

    public:
        explicit DiffuseLight(const Color3 & lightColor) : light(lightColor) {}
        ~DiffuseLight() override = default;

        Color3 emitted(const Ray & ray, const HitRecord & record) const override {
            //仅当光线和法线同向时返回光源颜色
            if (record.hitFrontFace) {
                return light;
            } else {
                return Color3();
            }
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * _light = dynamic_cast<const DiffuseLight *>(&obj);
            if (_light == null) return false;
            return light == _light->light;
        }

        std::string toString() const override {
            std::string ret("Diffuse Light: ");
            return ret + light.toString();
        }
    };
}

#endif //RENDERERTEST_DIFFUSELIGHT_HPP
