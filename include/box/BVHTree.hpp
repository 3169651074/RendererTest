#ifndef RENDERERTEST_BVHTREE_HPP
#define RENDERERTEST_BVHTREE_HPP

#include <hittable/HittableCollection.hpp>
#include <box/AbstractBoundingBox.hpp>

namespace renderer {
    /*
     * BVH树节点类
     */
    class BVHNode final : public AbstractHittable {
    public:
        /*
         * BVH为二叉树，节点成员为指向左右子节点的指针和当前节点的包围盒
         * 当前节点代表一个层次，当前节点的包围盒包括子节点所有包围盒
         */
        std::shared_ptr<AbstractHittable> left;
        std::shared_ptr<AbstractHittable> right;

        ~BVHNode() override = default;

        bool hit(const Ray &ray, const Range &range, HitRecord &record) const override {
            //判断光线有没有和当前节点的包围盒碰撞
            if (!boundingBox->hit(ray, range)) {
                return false;
            }

            //递归遍历左右子树的包围盒
            const bool hitLeft = left->hit(ray, range, record);
            const bool hitRight = right->hit(ray, Range(range.getMin(), hitLeft ? record.t : range.getMax()), record);
            return hitLeft || hitRight;
        }

        bool equals(const AbstractObject &obj) const override { throw std::runtime_error("Not supported"); }
        std::string toString() const override { throw std::runtime_error("Not supported"); }
    };

    /*
     * BVH二叉树
     * 继承Hittable抽象类，使得Hittable可能是包围盒节点，也可能是具体的物体
     * 树本身作为一个整体可被击中
     * 树的包围盒，就是根节点的包围盒
     */
    class BVHTree final : public AbstractHittable {
    private:
        //树的根节点
        std::shared_ptr<BVHNode> root;

        //比较函数，比较两个 hittable 对象的包围盒在特定轴上的位置，使得空间上邻近的物体在数组内也相邻
        static bool compare(const std::shared_ptr<AbstractHittable> & obj1, const std::shared_ptr<AbstractHittable> & obj2, size_t axis) {
            const auto point1 = obj1->getBoundingBox()->centerPoint();
            const auto point2 = obj2->getBoundingBox()->centerPoint();
            return point1[axis] < point2[axis];
        }

        //分为x，y，z三个轴向的比较
        static bool compare0(const std::shared_ptr<AbstractHittable> & obj1, const std::shared_ptr<AbstractHittable> & obj2) { return compare(obj1, obj2, 0); }
        static bool compare1(const std::shared_ptr<AbstractHittable> & obj1, const std::shared_ptr<AbstractHittable> & obj2) { return compare(obj1, obj2, 1); }
        static bool compare2(const std::shared_ptr<AbstractHittable> & obj1, const std::shared_ptr<AbstractHittable> & obj2) { return compare(obj1, obj2, 2); }

        //树结构构造函数
        std::shared_ptr<BVHNode> buildNode(std::vector<std::shared_ptr<AbstractHittable>> & array, size_t startIndex, size_t endIndex) {
            const size_t nodeCount = endIndex - startIndex;

            //当该层的物体个数不足以构成新的子树时，构造叶子节点
            auto node = std::make_shared<BVHNode>();

            if (nodeCount == 1) {
                node->left = node->right = array[startIndex];
            } else if (nodeCount == 2) {
                node->left = array[startIndex];
                node->right = array[startIndex + 1];
            } else {
                //构造子树，递归调用此函数

                //每一层都重新选择轴
                //const int axis = randomInt(0, 2);
                auto box = boundingBox->emptyBox();
                for (size_t i = startIndex; i < endIndex; i++) {
                    box = box->merge(array[i]->getBoundingBox());
                }
                const int axis = box->longestAxis();

                bool (*func) (const std::shared_ptr<AbstractHittable> & obj1, const std::shared_ptr<AbstractHittable> & obj2) = null;
                switch (axis) {
                    case 0: func = compare0; break;
                    case 1: func = compare1; break;
                    case 2: func = compare2; break;
                    default:func = compare0;
                }

                //排序当前区间内的物体
                std::sort(array.begin() + (long)startIndex, array.begin() + (long)endIndex, func);

                //分割物体列表
                const size_t middleIndex = startIndex + nodeCount / 2;

                //递归构造左右子节点
                const auto leftNode = buildNode(array, startIndex, middleIndex);
                const auto rightNode = buildNode(array, middleIndex, endIndex);
                node->left = leftNode;
                node->right = rightNode;
            }

            //构造包围盒
            node->setBoundingBox(node->left->getBoundingBox()->merge(node->right->getBoundingBox()));
            return node;
        }

    public:
        //使用物体列表构造BVH树
        explicit BVHTree(const HittableCollection & collection) {
            //将物体列表拷贝一份，递归构造时需要排序修改物体列表
            auto copyList = std::vector<std::shared_ptr<AbstractHittable>>(collection.getList());
            if (collection.size() == 0) {
                return;
            } else {
                //Tree类的boundingBox在构造完成前设置一个初始值，使得构造过程中可以使用emptyBox虚函数
                boundingBox = copyList[0]->getBoundingBox();
            }

            //调用递归构造
            root = buildNode(copyList, 0, collection.size());

            //树的包围盒就是根节点的包围盒，包含了列表中所有物体
            boundingBox = root->getBoundingBox();
        }
        ~BVHTree() override = default;

        //树的hit方法供外部调用，而node的hit方法为具体实现，在此方法中调用
        bool hit(const Ray & ray, const Range & range, HitRecord & record) const override {
            //检查树是否存在
            if (!root) {
                return false;
            }

            //调用node的hit进行递归碰撞检查
            return root->hit(ray, range, record);
        }

        bool equals(const AbstractObject &obj) const override { throw std::runtime_error("Not supported"); }
        std::string toString() const override { throw std::runtime_error("Not supported"); }
    };
}

#endif //RENDERERTEST_BVHTREE_HPP
