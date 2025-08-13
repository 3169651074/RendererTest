#ifndef RENDERERTEST_TRIANGLE_HPP
#define RENDERERTEST_TRIANGLE_HPP

#include <hittable/AbstractHittable.hpp>

namespace renderer {
    /*
     * 平面三角形类
     * 使用Möller–Trumbore算法进行光线-三角形相交测试，不用获取平面方程
     */
    class Triangle final : public AbstractHittable {
    private:
        //三个顶点
        Point3 apex[3];

        //顶点法向量
        Vec3 normalVector[3];

        //预计算两个边向量
        Vec3 e1, e2;

        std::shared_ptr<AbstractMaterial> material;

    public:
        //使用三个顶点构造三角形，面法向量垂直于三角形平面
        Triangle(const std::shared_ptr<AbstractMaterial> & material, const Point3 & p1, const Point3 & p2, const Point3 & p3) : material(material) {
            apex[0] = p1; apex[1] = p2; apex[2] = p3;
            e1 = Point3::constructVector(p1, p2);
            e2 = Point3::constructVector(p1, p3);
            for (auto & i : normalVector) {
                i = Vec3::cross(e1, e2).unitVector();
            }

            const Point3 minPoint(
                    std::min({p1[0], p2[0], p3[0]}),
                    std::min({p1[1], p2[1], p3[1]}),
                    std::min({p1[2], p2[2], p3[2]})
            );
            const Point3 maxPoint(
                    std::max({p1[0], p2[0], p3[0]}),
                    std::max({p1[1], p2[1], p3[1]}),
                    std::max({p1[2], p2[2], p3[2]})
            );
            this->boundingBox = std::make_shared<AxisAlignedBoundingBox>(minPoint, maxPoint);
        }

        //使用三个顶点和独立的顶点法向量构造三角形
        Triangle(const std::shared_ptr<AbstractMaterial> & material, const Point3 & p1, const Point3 & p2, const Point3 & p3,
                 const Vec3 & normal1, const Vec3 & normal2, const Vec3 & normal3) : material(material)
        {
            apex[0] = p1; apex[1] = p2; apex[2] = p3;
            normalVector[0] = normal1; normalVector[1] = normal2; normalVector[2] = normal3;
            e1 = Point3::constructVector(p1, p2);
            e2 = Point3::constructVector(p1, p3);

//            const auto box1 = std::make_shared<AxisAlignedBoundingBox>(p1, p1 + e1);
//            const auto box2 = std::make_shared<AxisAlignedBoundingBox>(p1, p1 + e2);
//            this->boundingBox = box1->merge(box2);

            //找出三个顶点每个分量的最小值和最大值
            const Point3 minPoint(
                    std::min({p1[0], p2[0], p3[0]}),
                    std::min({p1[1], p2[1], p3[1]}),
                    std::min({p1[2], p2[2], p3[2]})
            );
            const Point3 maxPoint(
                    std::max({p1[0], p2[0], p3[0]}),
                    std::max({p1[1], p2[1], p3[1]}),
                    std::max({p1[2], p2[2], p3[2]})
            );
            this->boundingBox = std::make_shared<AxisAlignedBoundingBox>(minPoint, maxPoint);
        }
        ~Triangle() override = default;

        bool hit(const Ray & ray, const Range & range, HitRecord & record) const override {
            const Vec3 h = ray.getDirection().cross(e2); //h = d x e2
            //系数行列式
            const double detA = e1.dot(h); //detA = e1 * (d x e2)

            //行列式为0，说明方程组无解或有无穷解（光线和三角形平行或有无数个交点）
            if (floatValueNearZero(detA)) {
                return false;
            }

            const Vec3 s = Point3::constructVector(apex[0], ray.getOrigin()); //s = O - v0

            //计算未知数U并检查
            const Range coefficientRange(0.0, 1.0);
            const double u = s.dot(h) / detA; // u = (s · h) / det
            if (!coefficientRange.inRange(u)) {
                return false;
            }

            const Vec3 q = s.cross(e1);  // q = s × e1

            //计算未知数V并检查
            const double v = ray.getDirection().dot(q) / detA; // v = (D · q) / det
            if (!coefficientRange.inRange(v) || u + v > 1.0) {
                return false;
            }

            //满足相交条件，计算碰撞参数
            record.t = e2.dot(q) / detA; // t = (e2 · q) / det
            if (!range.inRange(record.t)) {
                return false;
            }
            record.hitPoint = ray.at(record.t);
            record.material = material;
            record.uvPair = std::pair<double, double>(u, v);

            //交点法向量为三个顶点法向量的插值平滑
            const Vec3 n = ((1.0 - u - v) * normalVector[0] + u * normalVector[1] + v * normalVector[2]).unitVector();
            record.hitFrontFace = Vec3::dot(ray.getDirection(), n) < 0.0;
            record.normalVector = record.hitFrontFace ? n : -n;
            return true;
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * t = dynamic_cast<const Triangle *>(&obj);
            if (t == null) return false;
            return apex[0] == t->apex[0] && apex[1] == t->apex[1] && apex[2] == t->apex[2] && material == t->material;
        }

        std::string toString() const override {
            std::string ret("Triangle: ");
            for (const auto & i : apex) {
                ret += "P1 = " + i.toString() + " ";
            }
            return ret;
        }
    };
}

#endif //RENDERERTEST_TRIANGLE_HPP
