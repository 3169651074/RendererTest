#ifndef RENDERERTEST_TRANSFORM_HPP
#define RENDERERTEST_TRANSFORM_HPP

#include <hittable/AbstractHittable.hpp>

namespace renderer {
    /*
     * 变换类，包含指向物体的指针，变换矩阵及其逆矩阵，以及变换后的包围盒
     */
    class Transform final : public AbstractHittable {
    private:
        std::shared_ptr<AbstractHittable> object;

        //4 x 4变换矩阵
        Matrix transformMatrix;
        Matrix transformInverse;
        Matrix transformInverseTranspose; //逆矩阵的转置，用于变换法线

        //变换后物体的包围盒
        //std::shared_ptr<AbstractBoundingBox> boundingBox;

    public:
        //通过变换参数构造变换矩阵
        //rotate, shift数组分别表示x, y, z轴的平移和旋转角度（角度制）
        explicit Transform(const std::shared_ptr<AbstractHittable> & object,
            const std::array<double, 3> & rotate = {}, const std::array<double, 3> & shift = {}, const std::array<double, 3> & scale = {1.0, 1.0, 1.0}) :
            object(object),
            transformMatrix(4, 4), transformInverse(4, 4), transformInverseTranspose(4, 4)
        {
            //M = T * R * S，平移 * 旋转 * 缩放
            const auto m1 = Matrix::constructShiftMatrix(shift);
            const auto m2 = Matrix::constructRotateMatrix(rotate);
            const auto m3 = Matrix::constructScaleMatrix(scale);
            transformMatrix = m1 * m2 * m3;
            transformInverse = transformMatrix.inverse();
            transformInverseTranspose = transformInverse.transpose();

            //变换包围盒
            this->boundingBox = object->getBoundingBox()->transformBoundingBox(transformMatrix);
        }

        //默认调用Matrix类的析构函数
        ~Transform() override = default;

        bool hit(const Ray &ray, const Range &range, HitRecord &record) const override {
            /*
             * 将世界空间光线变换到物体的局部空间：使用逆矩阵分别对ray的起点和方向向量进行变换
             * 只有左矩阵的列数和右矩阵的行数相同的矩阵才能相乘，则将三维点变为1列4行的列向量
             */
            auto rayOrigin = Matrix::toMatrix(ray.getOrigin().toVector(), 1.0);
            auto rayDirection = Matrix::toMatrix(ray.getDirection(), 0.0);
            rayOrigin = transformInverse * rayOrigin;
            rayDirection = transformInverse * rayDirection;

            //构造变换后的光线
            const Ray transformed(rayOrigin.toPoint(), rayDirection.toPoint().toVector(), ray.getTime());

            //在物体空间中对变换后的光线进行相交测试
            if (!object->hit(transformed, range, record)) {
                return false;
            } else {
                //如果有碰撞，则将将局部空间的命中记录变换回世界空间，t值和uv坐标不需要变换
                //变换碰撞点
                auto point = Matrix::toMatrix(record.hitPoint.toVector(), 1.0);
                point = transformMatrix * point;
                record.hitPoint = point.toPoint();

                //使用逆转置变换矩阵变换法向量
                //向量不受平移影响，将w分量设置为0可完成此目的
                auto normal = Matrix::toMatrix(record.normalVector, 0.0);
                normal = transformInverseTranspose * normal;
                record.normalVector = normal.toPoint().toVector().unitVector();
                record.hitFrontFace = Vec3::dot(ray.getDirection(), record.normalVector) < 0.0;
                return true;
            }
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * trans = dynamic_cast<const Transform *>(&obj);
            if (trans == null) return false;
            return object == trans->object && transformMatrix == trans->transformMatrix;
        }

        std::string toString() const override {
            std::string ret("Transform: Object = ");
            return ret + object->toString() + "Transform matrix = " + transformMatrix.toString();
        }
    };
}

#endif //RENDERERTEST_TRANSFORM_HPP
