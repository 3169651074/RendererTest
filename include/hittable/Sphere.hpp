#ifndef RENDERERTEST_SPHERE_HPP
#define RENDERERTEST_SPHERE_HPP

#include <hittable/AbstractHittable.hpp>
#include <material/AbstractMaterial.hpp>
#include <box/AxisAlignedBoundingBox.hpp>

namespace renderer {
    /*
     * 球体物体类，有运动属性以实现运动模糊效果
     */
    class Sphere final : public AbstractHittable {
    private:
        //球心使用空间光线表示而不是静态的点
        Ray center;
        double radius;

        std::shared_ptr<AbstractMaterial> material;

        //将位于球体表面的点转换为二维坐标（u, v）
        static std::pair<double, double> mapUVPair(const Point3 & surfacePoint) {
            const double theta = std::acos(-surfacePoint[1]);
            const double phi = std::atan2(-surfacePoint[2], surfacePoint[0]) + PI;

            return {phi / (2.0 * PI), theta / PI};
        }

    public:
        //构造静止球体
        Sphere(const std::shared_ptr<AbstractMaterial> & material, const Point3 & center, double radius) :
                material(material), center(Ray(center, Vec3())), radius(radius > 0.0 ? radius : 0.0)
        {
            //构造包围盒
            const Vec3 edge = Vec3(radius, radius, radius);
            boundingBox = std::make_shared<AxisAlignedBoundingBox>(center - edge, center + edge);
        }

        //构造运动球体
        Sphere(const std::shared_ptr<AbstractMaterial> & material, const Point3 & from, const Point3 & to, double radius) :
                material(material), center(Ray(from, Point3::constructVector(from, to))), radius(radius > 0.0 ? radius : 0.0)
        {
            //运动物体的包围盒需要包括其整个运动路径的每一个位置，此处构造的球体为直线运动，使用起点和终点的包围盒合并即可
            const Vec3 edge = Vec3(radius, radius, radius);
            const auto bStart = AxisAlignedBoundingBox(from - edge, from + edge);
            const auto bEnd = AxisAlignedBoundingBox(to - edge, to + edge);

            boundingBox = std::make_shared<AxisAlignedBoundingBox>(bStart, bEnd);
        }

        ~Sphere() override = default;

        bool hit(const Ray & ray, const Range & range, HitRecord & record) const override {
            //获取球体在当前时间的中心位置
            const Point3 currentCenter = center.at(ray.getTime());

            //解一元二次方程，判断光线和球体的交点个数
            const Vec3 cq = Point3::constructVector(ray.getOrigin(), currentCenter);
            const Vec3 dir = ray.getDirection();
            const double a = Vec3::dot(dir, dir);
            const double b = -2.0 * Vec3::dot(cq, dir);
            const double c = Vec3::dot(cq, cq) - radius * radius;
            double delta = b * b - 4.0 * a * c;

            if (delta < 0.0) return false;
            delta = sqrt(delta);

            //root1对应较小的t值，为距离摄像机较近的交点
            const double root1 = (-b - delta) / (a * 2.0);
            const double root2 = (-b + delta) / (a * 2.0);

            double root;
            if (range.inRange(root1)) { //先判断root1
                root = root1;
            } else if (range.inRange(root2)) {
                root = root2;
            } else {
                return false; //两个根均不在允许范围内
            }

            //设置碰撞信息
            record.t = root;
            record.hitPoint = ray.at(root);
            record.material = material;

            //outwardNormal为球面向外的单位法向量，通过此向量和光线方向向量的点积符号判断光线撞击了球的内表面还是外表面
            //若点积小于0，则两向量夹角大于90度，两向量不同方向
            const Vec3 outwardNormal = Point3::constructVector(currentCenter, record.hitPoint).unitVector();
            record.hitFrontFace = Vec3::dot(ray.getDirection(), outwardNormal) < 0.0;
            record.normalVector = record.hitFrontFace ? outwardNormal : -outwardNormal;

            //将碰撞点从世界坐标系变换到以球心为原点的局部坐标系：直接在世界坐标系中构造向量
            const Vec3 localVector = Point3::constructVector(currentCenter, record.hitPoint).unitVector();
            record.uvPair = mapUVPair(Point3(localVector));
            return true;
        }

        double pdfValue(const Point3 &origin, const Vec3 &direction) const override {
            //此计算方法只对静止球体有效
            HitRecord record;
            if (!this->hit(Ray(origin, direction), Range(0.001, INFINITY), record)) {
                return 0.0;
            }

            const double distanceSquare = Point3::distanceSquare(origin, center.at(0.0));
            const double cosThetaMax = std::sqrt(1.0 - radius * radius / distanceSquare);
            const double solidAngle = 2.0 * PI * (1.0 - cosThetaMax);
            return 1.0 / solidAngle;
        }

        Vec3 randomVector(const Point3 &origin) const override {
            const Vec3 direction = Point3::constructVector(origin, center.at(0.0));
            const double distanceSquare = direction.lengthSquare();

            const double r1 = randomDouble();
            const double r2 = randomDouble();

            const double phi = 2.0 * PI * r1;
            const double z = 1.0 + r2 * (std::sqrt(1.0 - radius * radius / distanceSquare) - 1);
            const double x = std::cos(phi) * std::sqrt(1.0 - z * z);
            const double y = std::sin(phi) * std::sqrt(1.0 - z * z);

            OrthonormalBase base(direction, 2);
            return base.transform(Vec3(x, y, z));
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * sphere = dynamic_cast<const Sphere *>(&obj);
            if (sphere == null) return false;
            return material == sphere->material && center == sphere->center && radius == sphere->radius;
        }

        std::string toString() const override {
            std::string ret;
            if (center.getDirection() == Vec3()) {
                ret += "Static Sphere, Center = " + center.getOrigin().toString();
            } else {
                ret += "Moving Sphere, Center " + center.toString();
            }
            ret += ", Radius = " + std::to_string(radius);
            return ret;
        }
    };
}

#endif //RENDERERTEST_SPHERE_HPP
