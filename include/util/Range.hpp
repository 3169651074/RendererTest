#ifndef RENDERERTEST_RANGE_HPP
#define RENDERERTEST_RANGE_HPP

#include <AbstractObject.hpp>

namespace renderer {
    /*
     * 范围工具类
     *
     * 操作函数：
     * 将当前范围端点偏移指定值
     * 判断值是否在当前范围内
     * 判断区间是否有效
     * 将值限定在当前范围内
     * 将当前区间左右端点各扩展指定长度
     *
     * EMPTY：空区间
     * UNIVERSE：全实数域
     */
    class Range final : public AbstractObject {
    private:
        double min;
        double max;

    public:
        //默认构造空区间
        explicit Range(double min = 0.0, double max = 0.0) : min(min), max(max) {}

        //构造两个区间的并集
        Range(const Range & r1, const Range & r2) :
            min(r1.min < r2.min ? r1.min : r2.min), max(r1.max > r2.max ? r1.max : r2.max) {}

        ~Range() override = default;

        // ====== 对象操作函数 ======

        bool inRange(double value, bool isLeftClose = true, bool isRightClose = true) const {
            const bool equalsToMin = floatValueEquals(value, min);
            const bool equalsToMax = floatValueEquals(value, max);

            if ((value < min && !equalsToMin) || (value > max && !equalsToMax)) return false;
            if (equalsToMin && !isLeftClose) return false;
            if (equalsToMax && !isRightClose) return false;
            return true;
        }

        Range & offset(double offsetValue) {
            min += offsetValue;
            max += offsetValue;
            return *this;
        }

        bool isValid() const {
            return min < max || floatValueEquals(min, max);
        }

        double length() const {
            return max - min;
        }

        double clamp(double value) const {
            if (value > max) {
                return max;
            } else if (value < min) {
                return min;
            } else {
                return value;
            }
        }

        //将当前区间左右端点各扩展length长度
        Range & expand(double length) {
            if (length > 0) { //负长度不扩展
                min -= length;
                max += length;
            }
            return *this;
        }

        // ====== 类封装函数 =======

        bool equals(const AbstractObject & obj) const override {
            if (this == &obj) return true;
            const auto * range = dynamic_cast<const Range *>(&obj);
            if (range == null) return false;
            return min == range->min && max == range->max;
        }

        std::string toString() const override {
            char buffer[TOSTRING_BUFFER_SIZE] = { 0 };
            snprintf(buffer, TOSTRING_BUFFER_SIZE, "Range: [%.4lf, %.4lf]", min, max);
            return {buffer};
        }

        double getMin() const { return min; }
        void setMin(const double _min) { this->min = _min; }
        double getMax() const { return max; }
        void setMax(const double _max) { this->max = _max; }
    };
}

#endif //RENDERERTEST_RANGE_HPP
