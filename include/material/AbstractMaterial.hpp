#ifndef RENDERERTEST_ABSTRACTMATERIAL_HPP
#define RENDERERTEST_ABSTRACTMATERIAL_HPP

#include <basic/Color3.hpp>
#include <hittable/AbstractHittable.hpp>
#include <util/OrthonormalBase.hpp>
#include <util/AbstractPDF.hpp>

namespace renderer {
    //散射记录
    struct ScatterRecord {
        Color3 attenuation;
        std::shared_ptr<AbstractPDF> pdf;
        bool isSkipPDF;
        Ray skipPDFRay;
    };

    /*
     * 材质抽象类，定义物体表面的统一反射规则，有些材质没有纹理
     */
    class AbstractMaterial : public AbstractObject {
    public:
        ~AbstractMaterial() override = default;

        //散光函数，定义材质对光线的变更
        virtual bool scatter(const Ray & in, const HitRecord & record, ScatterRecord & scatterRecord) const = 0;

        //散光概率密度函数，定义对各个入射方向进行采样的概率
        virtual double scatterPDF(const Ray & in, const HitRecord & record, const Ray & out) const { return 1.0; }
    };
}

#endif //RENDERERTEST_ABSTRACTMATERIAL_HPP
