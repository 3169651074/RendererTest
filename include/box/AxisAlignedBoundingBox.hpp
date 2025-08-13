#ifndef RENDERERTEST_AXISALIGNEDBOUNDINGBOX_HPP
#define RENDERERTEST_AXISALIGNEDBOUNDINGBOX_HPP

#include <box/AbstractBoundingBox.hpp>

namespace renderer {
    /*
     * 轴对齐包围盒：包围盒为长方体，所有棱和坐标轴平行，为能够完全包住物体的最小长方体
     * 构造方式：两个对角点，或直接提供三个Range。使用merge方法进行合并
     */
    class AxisAlignedBoundingBox : public AbstractBoundingBox {
    private:
        Range range[3];

        //确保包围盒体积有效
        void ensureVolume() {
            constexpr double EPSILON = 0.0005;
            for (auto & i : range) {
                if (i.length() < EPSILON) {
                    i.expand(EPSILON);
                }
            }
        }

    public:
        //默认构造空包围盒
        explicit AxisAlignedBoundingBox(const Range & x = Range(), const Range & y = Range(), const Range & z = Range()) {
            range[0] = x; range[1] = y; range[2] = z;
            ensureVolume();
        }

        //使用两个对角点构造长方体
        AxisAlignedBoundingBox(const Point3 & p1, const Point3 & p2) {
            //取两个点每个分量的有效值
            for (size_t i = 0; i < 3; i++) {
                range[i] = p1[i] < p2[i] ? Range(p1[i], p2[i]) : Range(p2[i], p1[i]);
            }
            ensureVolume();
        }

        //构造两个包围盒的合并
        AxisAlignedBoundingBox(const AxisAlignedBoundingBox & b1, const AxisAlignedBoundingBox & b2) {
            for (size_t i = 0; i < 3; i++) {
                range[i] = Range(b1.range[i], b2.range[i]);
            }
            //合并不会减小包围盒的体积
        }

        ~AxisAlignedBoundingBox() override = default;

        bool hit(const Ray & ray, const Range & checkRange) const override {
            const Point3 & rayOrigin = ray.getOrigin();
            const Vec3 & rayDirection = ray.getDirection();

            Range currentRange(checkRange);
            for (Uint32 axis = 0; axis < 3; axis++) {
                const Range & axisRange = range[axis];
                const double q = rayOrigin[axis];
                const double d = rayDirection[axis];

                //计算光在当前轴和边界的两个交点
                const double t1 = (axisRange.getMin() - q) / d;
                const double t2 = (axisRange.getMax() - q) / d;

                //将currentRange限制到这两个交点的范围内
                if (t1 < t2) {
                    if (t1 > currentRange.getMin()) currentRange.setMin(t1);
                    if (t2 < currentRange.getMax()) currentRange.setMax(t2);
                } else {
                    if (t2 > currentRange.getMin()) currentRange.setMin(t2);
                    if (t1 < currentRange.getMax()) currentRange.setMax(t1);
                }

                if (!currentRange.isValid()) {
                    return false;
                }
            }
            return true;
        }

        std::shared_ptr<AbstractBoundingBox> merge(const std::shared_ptr<AbstractBoundingBox> &box) const override {
            const auto _box = std::dynamic_pointer_cast<AxisAlignedBoundingBox>(box);
            return std::make_shared<AxisAlignedBoundingBox>(*this, *_box);
        }

        Point3 centerPoint() const override {
            Point3 ret;
            for (size_t i = 0; i < 3; i++) {
                ret[i] = (range[i].getMin() + range[i].getMax()) / 2.0;
            }
            return ret;
        }

        int longestAxis() const override {
            if (range[0].length() > range[1].length()) {
                return range[0].length() > range[2].length() ? 0 : 2;
            } else {
                return range[1].length() > range[2].length() ? 1 : 2;
            }
        }

        std::shared_ptr<AbstractBoundingBox> emptyBox() const override {
            return std::make_shared<AxisAlignedBoundingBox>();
        }

        std::shared_ptr<AbstractBoundingBox> transformBoundingBox(const Matrix & matrix) override {
            //使用矩阵对包围盒的8个顶点进行变换
            Point3 min(INFINITY, INFINITY, INFINITY);
            Point3 max(-INFINITY, -INFINITY, -INFINITY);

            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        //取出每个顶点的坐标
                        const double x = i * range[0].getMax() + (1.0 - i) * range[0].getMin();
                        const double y = j * range[1].getMax() + (1.0 - j) * range[1].getMin();
                        const double z = k * range[2].getMax() + (1.0 - k) * range[2].getMin();

                        //计算变换后的坐标
                        Point3 point(x, y, z);
                        point = (matrix * Matrix::toMatrix(point.toVector(), 1.0)).toPoint();

                        //计算最值，保证包围盒和坐标轴对齐
                        for (int l = 0; l < 3; l++) {
                            min[l] = std::min(min[l], point[l]);
                            max[l] = std::max(max[l], point[l]);
                        }
                    }
                }
            }

            //使用min和max重构包围盒
            return std::make_shared<AxisAlignedBoundingBox>(min, max);
        }

        // ====== 类封装函数 ======

        //同三元组类，使用下标访问分量
        Range operator[](size_t index) const { return range[index]; }
        Range & operator[](size_t index) { return range[index]; }

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * box = dynamic_cast<const AxisAlignedBoundingBox *>(&obj);
            if (box == null) return false;
            return range[0] == box->range[0] && range[1] == box->range[1] && range[2] == box->range[2];
        }

        std::string toString() const override {
            std::string ret("AxisAlignedBoundingBox:");
            for (Uint8 i = 0; i < 3; i++) {
                ret += "\n\t";
                ret += static_cast<char>('x' + i);
                ret += " = " + range[i].toString();
            }
            return ret;
        }
    };
}

#endif //RENDERERTEST_AXISALIGNEDBOUNDINGBOX_HPP
