#ifndef POINT3_HPP
#define POINT3_HPP

#include <basic/Vec3.hpp>

namespace renderer {
    /*
     * 空间点类
     *
     * 点操作函数：
     * 计算两点距离(对象+静态)
     * 由两点构造空间向量(静态)
     * 点加减向量(点偏移) (对象)
     * 点转向量(对象)，向量转点(构造方法)
     */
    class Point3 final : public AbstractTuple<double> {
    public:
        explicit Point3(double x = 0.0, double y = 0.0, double z = 0.0)
                    : AbstractTuple(x, y, z) {}

        explicit Point3(const Vec3 & obj) : AbstractTuple(obj[0], obj[1], obj[2]) {}

        ~Point3() override = default;

        // ====== 对象操作函数 ======

        double distanceSquare(const Point3 & anotherPoint) const {
            double sum = 0.0;
            for (size_t i = 0; i < 3; i++) {
                sum += (elements[i] - anotherPoint.elements[i]) * (elements[i] - anotherPoint.elements[i]);
            }
            return sum;
        }

        double distance(const Point3 & anotherPoint) const {
            return std::sqrt(distanceSquare(anotherPoint));
        }

        Point3 & operator+=(const Vec3 & offset) {
            for (size_t i = 0; i < 3; i++) {
                elements[i] += offset[i];
            }
            return *this;
        }

        Point3 operator+(const Vec3 & offset) const {
            Point3 ret(*this); ret += offset; return ret;
        }

        Point3 & operator-=(const Vec3 & offset) {
            for (size_t i = 0; i < 3; i++) {
                elements[i] -= offset[i];
            }
            return *this;
        }

        Point3 operator-(const Vec3 & offset) const {
            Point3 ret(*this); ret -= offset; return ret;
        }

        //点转向量
        Vec3 toVector() const  {
            return Vec3(elements[0], elements[1], elements[2]);
        }

        // ====== 静态操作函数 ======

        static inline double distanceSquare(const Point3 & p1, const Point3 & p2) {
            return p1.distanceSquare(p2);
        }

        static inline double distance(const Point3 & p1, const Point3 & p2) {
            return std::sqrt(p1.distanceSquare(p2));
        }

        static inline Vec3 constructVector(const Point3 & from, const Point3 & to) {
            Vec3 ret;
            for (size_t i = 0; i < 3; i++) {
                ret[i] = to.elements[i] - from.elements[i];
            }
            return ret;
        }

        // ====== 类封装函数 ======

        std::string toString() const override {
            char buffer[TOSTRING_BUFFER_SIZE] = { 0 };
            snprintf(buffer, TOSTRING_BUFFER_SIZE, "Point3: (%.4lf, %.4lf, %.4lf)", elements[0], elements[1], elements[2]);
            return {buffer};
        }
    };
}

#endif //POINT3_HPP
