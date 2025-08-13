#ifndef RENDERERTEST_ABSTRACTHITTABLE_HPP
#define RENDERERTEST_ABSTRACTHITTABLE_HPP

#include <basic/Ray.hpp>
#include <util/Range.hpp>
#include <box/AbstractBoundingBox.hpp>

namespace renderer {
    //前置声明，告知编译器该类稍后定义
    class AbstractMaterial;

    //碰撞记录，POD类型
    struct HitRecord {
        Point3 hitPoint;
        Vec3 normalVector;
        double t;                                   //光线撞击物体时对应的t值
        bool hitFrontFace;                          //光线是否撞击到物体的外表面
        std::shared_ptr<AbstractMaterial> material; //碰撞位置材质信息
        std::pair<double, double> uvPair;           //纹理映射信息
    };

    class AbstractHittable : public AbstractObject {
    protected:
        //所有能被光线撞击的物体都有包围盒
        std::shared_ptr<AbstractBoundingBox> boundingBox;

    public:
        ~AbstractHittable() override = default;

        //所有可碰撞物体都有hit方法，用于判断光线是否和物体相交。如果相交，需要记录交点信息到record中
        virtual bool hit(const Ray & ray, const Range & range, HitRecord & record) const = 0;

        //获取可碰撞物体在指定起点和方向的PDF函数值
        virtual double pdfValue(const Point3 & origin, const Vec3 & direction) const {
            return 1.0;
        }

        //在物体范围内随机生成一个点
        //生成从指定点指向物体上一点的向量
        virtual Vec3 randomVector(const Point3 & origin) const {
            return Vec3();
        }

        //设置和获取包围盒
        void setBoundingBox(const std::shared_ptr<AbstractBoundingBox> & _boundingBox) {
            AbstractHittable::boundingBox = _boundingBox;
        }

        const std::shared_ptr<AbstractBoundingBox> & getBoundingBox() const {
            return boundingBox;
        }
    };
}

#endif //RENDERERTEST_ABSTRACTHITTABLE_HPP
