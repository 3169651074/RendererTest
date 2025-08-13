#ifndef RENDERERTEST_SOLIDCOLOR_HPP
#define RENDERERTEST_SOLIDCOLOR_HPP

#include <texture/AbstractTexture.hpp>

namespace renderer {
    /*
     * 静态纹理类，包装albedo基础颜色
     */
    class SolidColor final : public AbstractTexture {
    private:
        Color3 albedo;

    public:
        explicit SolidColor(const Color3 & albedo = Color3()) : albedo(albedo) {}
        ~SolidColor() override = default;

        Color3 value(const std::pair<double, double> &uvPair, const Point3 &point) const override {
            return albedo;
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * solidColor = dynamic_cast<const SolidColor *>(&obj);
            if (solidColor == null) return false;
            return albedo == solidColor->albedo;
        }

        std::string toString() const override {
            std::string ret("SolidColor Texture: Albedo = ");
            return ret + albedo.toString();
        }

        const Color3 & getAlbedo() const { return albedo; }
    };
}

#endif //RENDERERTEST_SOLIDCOLOR_HPP