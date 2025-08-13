#ifndef RENDERERTEST_CONSTANTMEDIUM_HPP
#define RENDERERTEST_CONSTANTMEDIUM_HPP

#include <hittable/AbstractHittable.hpp>

namespace renderer {
    /*
     * 密度均匀的随机散射介质，只适用于包裹凸多面体，此类和isotropic共同完成体积雾效果（二者非唯一绑定）
     * constantmedium作为hittable，拥有指向物体（凸多面体）的指针，用于决定体积雾的作用范围
     *     isotropic作为材质，决定光线在constantmedium内部的行为
     * 根据比尔-朗伯定律随机决定光线在撞上第一个粒子前能走多远，此公式需要一个密度因子
     */
    class ConstantMedium : public AbstractHittable {
    private:
        //包围物体
        std::shared_ptr<AbstractHittable> object;
        //将材质替换，可以实现不同的自定义效果
        std::shared_ptr<AbstractMaterial> material;
        //比尔-朗伯定律因子
        double density;
        double factor;

    public:
        //使用物体，材质和密度构造均匀介质
        ConstantMedium(const std::shared_ptr<AbstractHittable> &object, const std::shared_ptr<AbstractMaterial> &material, double density) :
            object(object), material(material), density(density), factor(-1.0 / density)
        {
            this->boundingBox = object->getBoundingBox();
        }

        ~ConstantMedium() override = default;

        bool hit(const Ray &ray, const Range &range, HitRecord &record) const override {
            /*
             * 首先调用边界物体的 hit 函数两次，以找到光线进入和射出该体积的两个交点
             * 如果光线没有进入或者只进入一次（擦边），则认为没有命中
             * 每次碰撞都随机变换方向，多次迭代后就能形成柔和、弥散的视觉效果
             *
             * 1：正常与物体求交，找到一个交点（hit方法自动记录最近交点）
             * 2：前进一小段距离，再次求交，尝试寻找第二个交点
             * 3：光线在介质中行进的距离为两个交点的t值之差乘以光线的方向向量的模
             */
            HitRecord rec1, rec2;
            if (!object->hit(ray, Range(-INFINITY, INFINITY), rec1)) {
                return false;
            }
            if (!object->hit(ray, Range(rec1.t + 0.001, INFINITY), rec2)) {
                return false;
            }

            if (rec1.t < range.getMin()) {
                rec1.t = range.getMin();
            }
            if (rec2.t > range.getMax()) {
                rec2.t = range.getMax();
            }

            if (rec1.t >= rec2.t) {
                return false;
            }
            if (rec1.t < 0) {
                rec1.t = 0;
            }

            //计算出光线在介质内部需要穿行的总距离
            //确保无论光线是否被变换过，距离计算都准确
            const auto rayLength = ray.getDirection().length();
            const auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;

            /*
             * 根据介质的密度决定光在撞上一个第一个粒子前能走多远
             * 使用比尔-朗伯定律
             * hitDistance = −1 / density × ln(randomDouble())
             * 密度越大，平均的hitDistance就越短
             */
            const auto hitDistance = factor * std::log(randomDouble());

            if (hitDistance > distanceInsideBoundary) {
                //光没有在介质内部发生散射
                return false;
            } else {
                //发生散射，计算碰撞点并填充碰撞信息
                record.t = rec1.t + hitDistance / rayLength;
                record.hitPoint = ray.at(record.t);
                record.normalVector = Vec3(1.0, 0.0, 0.0); //任意
                record.hitFrontFace = true; //任意
                record.material = this->material;
                return true;
            }
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * medium = dynamic_cast<const ConstantMedium *>(&obj);
            if (medium == null) return false;
            return object == medium->object && material == medium->material && floatValueEquals(density, medium->density);
        }

        std::string toString() const override {
            std::string ret("Constant Medium: ");
            return ret + "Object = " + object->toString() + ", density = " + std::to_string(density);
        }
    };
}

#endif //RENDERERTEST_CONSTANTMEDIUM_HPP
