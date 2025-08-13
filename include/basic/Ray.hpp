#ifndef RAY_HPP
#define RAY_HPP

#include <basic/Point3.hpp>

namespace renderer {
    /*
     * 光线类。P(t) = A + tB，A为光线的起点，t为实数，B为光线的方向向量，一般为单位向量
     *
     * at(t)：获取光线在参数为t的空间位置（一个点）
     */
    class Ray final : public AbstractObject {
    private:
        Point3 origin;
        Vec3 direction;
        double time; //光线被发射出的时间

    public:
        explicit Ray(const Point3 & origin = Point3(), const Vec3 & direction = Vec3(1.0, 0.0, 0.0), double time = 0.0) :
            origin(origin), direction(direction), time(time) {}
        ~Ray() override = default;

        // ====== 对象操作函数 ======

        Point3 at(double t) const {
            return origin + t * direction;
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject& obj) const override {
            if (this == &obj) return true;
            const auto * ray = dynamic_cast<const Ray *>(&obj);
            if (ray == null) return false;
            return origin == ray->origin && direction == ray->direction && time == ray->time;
        }

        std::string toString() const override {
            char buffer[TOSTRING_BUFFER_SIZE] = { 0 };
            snprintf(buffer, TOSTRING_BUFFER_SIZE, "Ray: Time = %.4lf, Origin = (%.4lf, %.4lf, %.4lf), Direction = (%.4lf, %.4lf, %.4lf)",
                time, origin[0], origin[1], origin[2], direction[0], direction[1], direction[2]);
            return {buffer};
        }

        Point3 getOrigin() const { return origin; }
        void setOrigin(const Point3& _origin) { this->origin = _origin; }
        Vec3 getDirection() const { return direction; }
        void setDirection(const Vec3& _direction) { this->direction = _direction; }
        double getTime() const { return time; }
        void setTime(double _time) {this->time = _time; }
    };
}

#endif //RAY_HPP
