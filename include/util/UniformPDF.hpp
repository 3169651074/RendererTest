#ifndef RENDERERTEST_UNIFORMPDF_HPP
#define RENDERERTEST_UNIFORMPDF_HPP

#include <util/AbstractPDF.hpp>

namespace renderer {
    /*
     * 均匀分布，在单位球面上
     */
    class UniformPDF : public AbstractPDF {
    public:
        ~UniformPDF() override = default;

        Vec3 generate() const override {
            //在单位球面上随机生成向量
            return Vec3::randomSpaceVector(1.0);
        }

        double value(const Vec3 &vec) const override {
            //单位球的面积为4π
            return 1.0 / (4.0 * PI);
        }

        bool equals(const AbstractObject &obj) const override {
            return true;
        }

        std::string toString() const override {
            return {"UniformPDF"};
        }
    };
}

#endif //RENDERERTEST_UNIFORMPDF_HPP
