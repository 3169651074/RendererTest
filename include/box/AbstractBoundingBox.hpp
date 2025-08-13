#ifndef RENDERERTEST_ABSTRACTBOUNDINGBOX_HPP
#define RENDERERTEST_ABSTRACTBOUNDINGBOX_HPP

#include <basic/Ray.hpp>
#include <util/Range.hpp>
#include <util/Matrix.hpp>

namespace renderer {
    /*
     * 包围盒抽象类，允许定义不同类型的包围盒
     * 原有的直接调用将转化为使用智能指针调用虚函数，需要在运行时通过虚函数表获取函数地址，有性能开销
     */
    class AbstractBoundingBox : public AbstractObject {
    public:
        ~AbstractBoundingBox() override = default;

        //返回空的包围盒
        virtual std::shared_ptr<AbstractBoundingBox> emptyBox() const = 0;

        //包围盒的hit方法：判断光线是否和包围盒相交
        virtual bool hit(const Ray & ray, const Range & range) const = 0;

        //将两个包围盒合并为一个更大的包围盒，返回合并后的包围盒（合并构造）
        virtual std::shared_ptr<AbstractBoundingBox> merge(const std::shared_ptr<AbstractBoundingBox> & box) const = 0;

        //返回这个包围盒在几何上的中心点，作为包围盒的几何属性代表，用于包围盒比较函数
        virtual Point3 centerPoint() const = 0;

        //获取包围盒最长的轴下标
        virtual int longestAxis() const = 0;

        //使用矩阵变换当前包围盒
        virtual std::shared_ptr<AbstractBoundingBox> transformBoundingBox(const Matrix & matrix) = 0;
    };
}

#endif //RENDERERTEST_ABSTRACTBOUNDINGBOX_HPP
