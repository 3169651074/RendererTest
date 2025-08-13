#ifndef RENDERERTEST_PARALLELOGRAM_HPP
#define RENDERERTEST_PARALLELOGRAM_HPP

#include <hittable/HittableCollection.hpp>

namespace renderer {
    /*
     * 四边形类
     */
    class Parallelogram final : public AbstractHittable {
    private:
        //形状属性
        Point3 q;
        Vec3 u, v;
        //四边形的面积
        double area;

        std::shared_ptr<AbstractMaterial> material;

        //判断光线和四边形相交的属性
        Vec3 normalVector; //四边形所在平面的法向量
        double planeD;     //平面一般方程Ax + By + Cz = D，由常量D和法向量(A, B, C)确定

    public:
        Parallelogram(const std::shared_ptr<AbstractMaterial> & material, const Point3 & q, const Vec3 & u, const Vec3 & v):
                material(material), q(q), u(u), v(v)
        {
            //将四个顶点都包进包围盒中
            const auto boundBox1 = AxisAlignedBoundingBox(q, q + u + v);
            const auto boundBox2 = AxisAlignedBoundingBox(q + u, q + v);
            this->boundingBox = std::make_shared<AxisAlignedBoundingBox>(boundBox1, boundBox2);

            //计算四边形所在平面的属性
            this->normalVector = Vec3::cross(u, v);
            this->area = normalVector.length(); //|u||v|sin(theta)
            this->normalVector.unitize();

            double sum = 0.0;
            for (int i = 0; i < 3; i++) {
                sum += normalVector[i] * q[i]; //D = Ax + By + Cz
            }
            this->planeD = sum;
        }
        ~Parallelogram() override = default;

        bool hit(const Ray & ray, const Range & range, HitRecord & hitInfo) const override {
            //光线参数t = (D - n · P) / (n · d)
            //若(n · d) = 0，则光线和四边形所在平面平行
            const double NDotD = Vec3::dot(normalVector, ray.getDirection());
            if (floatValueNearZero(NDotD)) {
                return false;
            }

            //计算光线和四边形所在无限平面的交点参数t
            double NDotP = 0.0;
            for (int i = 0; i < 3; i++) {
                NDotP += normalVector[i] * ray.getOrigin()[i];
            }
            const double t = (planeD - NDotP) / NDotD;
            if (!range.inRange(t)) {
                return false;
            }

            //计算用四边形边向量表示的交点的系数，判断两个系数是否在[0, 1]范围内
            const Point3 intersection = ray.at(t);
            const Vec3 p = Point3::constructVector(q, intersection);
            const Vec3 normal = Vec3::cross(u, v);
            const double denominator = normal.lengthSquare(); // |u x v|^2

            if (floatValueNearZero(denominator)) {
                return false; // u 和 v 平行，无法构成平行四边形
            }

            const double alpha = Vec3::dot(Vec3::cross(p, v), normal) / denominator;
            const double beta = Vec3::dot(Vec3::cross(u, p), normal) / denominator;

            const Range coefficientRange(0.0, 1.0);
            if (!coefficientRange.inRange(alpha) || !coefficientRange.inRange(beta)) {
                return false;
            }

            //记录碰撞信息
            hitInfo.t = t;
            hitInfo.hitPoint = intersection;
            hitInfo.material = material;
            hitInfo.uvPair = std::pair<double, double>(alpha, beta);
            hitInfo.hitFrontFace = Vec3::dot(ray.getDirection(), normalVector) < 0.0;
            hitInfo.normalVector = hitInfo.hitFrontFace ? normalVector : -normalVector;
            return true;
        }

        double pdfValue(const Point3 &origin, const Vec3 &direction) const override {
            HitRecord record;
            //检查方向有效性，确保从origin沿direction方向能够直接指向光源
            if (!this->hit(Ray(origin, direction), Range(0.001, INFINITY), record)) {
                return 0.0;
            }

            //从origin到q（光源上随机点）的向量为 record.t * direction
            const double distanceSquare = (record.t * direction).lengthSquare();
            //向量点积公式：cos(theta) = a dot b / |a| |b|，其中|b| = 1
            const double cosine = std::abs(Vec3::dot(direction, record.normalVector) / direction.length());
            return distanceSquare / (cosine * area);
        }

        Vec3 randomVector(const Point3 &origin) const override {
            const Point3 to = q + (randomDouble() * u) + (randomDouble() * v);
            return Point3::constructVector(origin, to);
        }

        // ====== 静态操作函数 ======

        //构造由6个长方形构成的长方体，以a和b为对角点
        static std::shared_ptr<HittableCollection> constructBox(const std::shared_ptr<AbstractMaterial> & mat, const Point3 & a, const Point3 & b) {
            //找出a和b的大小关系
            Point3 min, max;
            for (int i = 0; i < 3; i++) {
                min[i] = std::min(a[i], b[i]);
                max[i] = std::max(a[i], b[i]);
            }

            const Vec3 dx(max[0] - min[0], 0.0, 0.0);
            const Vec3 dy(0.0, max[1] - min[1], 0.0);
            const Vec3 dz(0.0, 0.0, max[2] - min[2]);

            const auto front = std::make_shared<Parallelogram>(mat, Point3(min[0], min[1], max[2]), dx, dy);
            const auto right = std::make_shared<Parallelogram>(mat, Point3(max[0], min[1], max[2]), -dz, dy);
            const auto back = std::make_shared<Parallelogram>(mat, Point3(max[0], min[1], min[2]), -dx, dy);
            const auto left = std::make_shared<Parallelogram>(mat, Point3(min[0], min[1], min[2]), dz, dy);
            const auto top = std::make_shared<Parallelogram>(mat, Point3(min[0], max[1], max[2]), dx, -dz);
            const auto bottom = std::make_shared<Parallelogram>(mat, Point3(min[0], min[1], min[2]), dx, dz);

            //使用智能指针包裹列表对象，使得列表对象在函数外部可以使用
            std::shared_ptr<HittableCollection> list = std::make_shared<HittableCollection>();
            list->add(front);
            list->add(right);
            list->add(back);
            list->add(left);
            list->add(top);
            list->add(bottom);
            return list;
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * parallelogram = dynamic_cast<const Parallelogram *>(&obj);
            if (parallelogram == null) return false;
            return q == parallelogram->q && u == parallelogram->u && v == parallelogram->v && material == parallelogram->material;
        }

        std::string toString() const override {
            char buffer[TOSTRING_BUFFER_SIZE];
            snprintf(buffer, TOSTRING_BUFFER_SIZE, "Parallelogram: %p, Q: %s, (U, V): (%s, %s)", this, q.toString().c_str(), u.toString().c_str(), v.toString().c_str());
            return {buffer};
        }
    };
}

#endif //RENDERERTEST_PARALLELOGRAM_HPP
