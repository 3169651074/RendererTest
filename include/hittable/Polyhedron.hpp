#ifndef RENDERERTEST_POLYHEDRON_HPP
#define RENDERERTEST_POLYHEDRON_HPP

#include <hittable/Triangle.hpp>
#include <box/AxisAlignedBoundingBox.hpp>

namespace renderer {
    /*
     * 多面体类，由多个三角形组成的不规则物体
     * 传入的三角形数组需要包含完整的三角形信息
     */
    class Polyhedron final : public AbstractHittable {
    private:
        //三角形集合
        std::vector<Triangle> triangles;

        //三个轴的范围
        double bounds[6] {};

    public:
        //使用三角形数组和三个轴向的范围构造多面体及其轴对齐包围盒
        Polyhedron(const std::shared_ptr<AbstractMaterial> & material, const std::vector<Triangle> & triangles,
                   const double bounds[6], const Vec3 & velocity = Vec3()) : triangles(triangles)
        {
            auto box = std::make_shared<AxisAlignedBoundingBox>();
            for (size_t i = 0; i < 6; i += 2) {
                (*box)[i / 2] = Range(bounds[i], bounds[i + 1]);
            }
            this->boundingBox = box;
            memcpy(this->bounds, bounds, 6 * sizeof(double));
        }
        ~Polyhedron() override = default;

        bool hit(const Ray & ray, const Range & range, HitRecord & record) const override {
            double closestT = range.getMax();
            bool isHit = false;
            HitRecord tempRecord;

            //对每个三角形进行求交测试，写法同HittableCollection类
            for (const auto & triangle : triangles) {
                if (triangle.hit(ray, Range(range.getMin(), closestT), tempRecord)) {
                    isHit = true;
                    closestT = tempRecord.t;
                    record = tempRecord;
                }
            }
            return isHit;
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * poly = dynamic_cast<const Polyhedron *>(&obj);
            if (poly == null) return false;
            return triangles == poly->triangles;
        }

        std::string toString() const override {
            std::string ret("Polyhedron: Bounds: ");
            for (size_t i = 0; i < 6; i += 2) {
                ret += (char)('x' + i);
                ret += ": [" + std::to_string(bounds[i]) + ", " + std::to_string(bounds[i + 1]) + "] ";
            }
            return ret;
        }
    };
}

#endif //RENDERERTEST_POLYHEDRON_HPP
