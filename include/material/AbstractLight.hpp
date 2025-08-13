#ifndef RENDERERTEST_ABSTRACTLIGHT_HPP
#define RENDERERTEST_ABSTRACTLIGHT_HPP

#include <material/AbstractMaterial.hpp>

namespace renderer {
    /*
     * 发光材质抽象类，emitted方法返回发出的光的颜色
     */
    class AbstractLight : public AbstractMaterial {
    public:
        ~AbstractLight() override = default;

        virtual Color3 emitted(const Ray & ray, const HitRecord & record) const = 0;

        //发光材质不散射光线
        bool scatter(const Ray & in, const HitRecord & record, ScatterRecord & scatterRecord) const final {
            return false;
        }
    };
}

#endif //RENDERERTEST_ABSTRACTLIGHT_HPP
