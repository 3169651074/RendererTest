#ifndef RENDERERTEST_ABSTRACTPDF_HPP
#define RENDERERTEST_ABSTRACTPDF_HPP

#include <basic/Vec3.hpp>

namespace renderer {
    /*
     * 概率密度函数抽象类，子类负责实现不同的分布
     */
    class AbstractPDF : public AbstractObject {
    public:
        ~AbstractPDF() override = default;

        //随机生成一个符合当前概率密度函数所代表的分布的向量（非单位向量）
        virtual Vec3 generate() const = 0;

        //根据传入的向量返回对应的PDF函数值
        virtual double value(const Vec3 & vec) const = 0;
    };
}

#endif //RENDERERTEST_ABSTRACTPDF_HPP
