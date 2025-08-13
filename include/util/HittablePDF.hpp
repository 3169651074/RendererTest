#ifndef RENDERERTEST_HITTABLEPDF_HPP
#define RENDERERTEST_HITTABLEPDF_HPP

#include <util/AbstractPDF.hpp>
#include <hittable/AbstractHittable.hpp>

namespace renderer {
    /*
     * 直接向物体采样的PDF
     * origin为光线的起点（当前碰撞点）
     */
    class HittablePDF : public AbstractPDF {
    private:
        std::shared_ptr<AbstractHittable> object;
        Point3 origin;

    public:
        HittablePDF(const std::shared_ptr<AbstractHittable> &object, const Point3 &origin) :
            object(object), origin(origin) {}

        ~HittablePDF() override = default;

        Vec3 generate() const override {
            //从碰撞点指向物体上任意一点
            return object->randomVector(origin);
        }

        double value(const Vec3 &vec) const override {
            return object->pdfValue(origin, vec.unitVector());
        }

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * pdf = dynamic_cast<const HittablePDF *>(&obj);
            if (pdf == null) return false;
            return object == pdf->object && origin == pdf->origin;
        }

        std::string toString() const override {
            std::string ret("Hittable PDF: Object = ");
            return ret + object->toString() + std::string(", Origin = ") + origin.toString();
        }
    };
}

#endif //RENDERERTEST_HITTABLEPDF_HPP
