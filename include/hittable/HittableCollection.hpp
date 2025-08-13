#ifndef RENDERERTEST_HITTABLECOLLECTION_HPP
#define RENDERERTEST_HITTABLECOLLECTION_HPP

#include <hittable/AbstractHittable.hpp>
#include <box/AbstractBoundingBox.hpp>

namespace renderer {
    /*
     * 所有可碰撞物体组成的集合，也继承自Hittable抽象类，拥有全局包围盒，只支持添加物体
     */
    class HittableCollection final : public AbstractHittable {
    private:
        std::vector<std::shared_ptr<AbstractHittable>> list;

        //AbstractHittable中定义了包围盒成员，子类不要重复定义，否则子类add方法修改子类的包围盒，而继承下来的getBoundingBox方法返回父类包围盒
        //std::shared_ptr<AbstractBoundingBox> boundingBox;

    public:
        HittableCollection() = default;
        ~HittableCollection() override = default;

        bool hit(const Ray & ray, const Range & range, HitRecord & record) const override {
            //遍历列表中所有物体，依次调用其hit方法，找出最近的交点
            bool isHit = false;
            double maxT = range.getMax();
            HitRecord tempRecord;

            for (const auto & obj : list) {
                if (obj->hit(ray, Range(range.getMin(), maxT), tempRecord)) {
                    isHit = true;        //碰撞到了任意物体
                    maxT = tempRecord.t; //记录当前碰撞的t值并作为下一次碰撞检查范围的最大t值，使得之后忽略t值更大的碰撞
                    record = tempRecord;
                }
            }
            return isHit;
        }

        // ====== 对象操作函数 ======

        //添加一个Hittable
        void add(const std::shared_ptr<AbstractHittable> & obj) {
            list.push_back(obj);
            //合并新物体的包围盒，保证总包围盒包括所有物体
            if (boundingBox) {
                boundingBox = boundingBox->merge(obj->getBoundingBox());
            } else {
                boundingBox = obj->getBoundingBox();
            }
        }

        size_t size() const { return list.size(); }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * collection = dynamic_cast<const HittableCollection *>(&obj);
            if (collection == null) return false;
            return list == collection->list;
        }

        std::string toString() const override {
            std::string ret("Hittable List:\n");
            for (size_t i = 0; i < list.size(); i++) {
                ret += "\t[" + std::to_string(i) + "]: " + list[i]->toString() + "\n";
            }
            return ret;
        }

        const std::vector<std::shared_ptr<AbstractHittable>> & getList() const { return list; }
    };
}

#endif //RENDERERTEST_HITTABLECOLLECTION_HPP
