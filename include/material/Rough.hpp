#ifndef RENDERERTEST_ROUGH_HPP
#define RENDERERTEST_ROUGH_HPP

#include <texture/SolidColor.hpp>
#include <material/AbstractMaterial.hpp>
#include <util/CosinePDF.hpp>

namespace renderer {
    /*
     * 粗糙材质类，使用纹理
     */
    class Rough final : public AbstractMaterial {
    private:
        std::shared_ptr<AbstractTexture> texture;

    public:
        explicit Rough(const std::shared_ptr<AbstractTexture> & texture) : texture(texture) {}
        explicit Rough(const Color3 & albedo = Color3()) : texture(std::make_shared<SolidColor>(albedo)) {}
        ~Rough() override = default;

        //粗糙材质：漫反射
        bool scatter(const Ray & in, const HitRecord & record, ScatterRecord & scatterRecord) const override {
            /*//使用正交基，以record.normalVector作为z轴
            OrthonormalBase base(record.normalVector, 2);
            Vec3 randomVec = Vec3::randomCosineVector(2, true);

            //将randomVec从世界坐标系转换到局部坐标系，randomVec为随机选择的反射方向
            randomVec = base.transform(randomVec);

            *//*
            //随机选择一个反射方向
            Vec3 reflectDirection = (record.normalVector + Vec3::randomSpaceVector(1.0)).unitVector();

            //随机的反射方向可能和法向量相互抵消，此时取消随机反射
            if (floatValueEquals(reflectDirection.lengthSquare(), Vec3::VECTOR_LENGTH_SQUARE_ZERO_EPSILON)) {
                reflectDirection = record.normalVector;
            }
            *//*
            //从反射点出发构造反射光线
            out = Ray(record.hitPoint, randomVec.unitVector(), in.getTime());
            attenuation = texture->value(record.uvPair, record.hitPoint);

            //材质采样PDF：与全局无关，只关心入射光线散射到出射方向的概率密度
            pdfValue = scatterPDF(in, record, out);*/

            scatterRecord.attenuation = texture->value(record.uvPair, record.hitPoint);
            scatterRecord.pdf = std::make_shared<CosinePDF>(record.normalVector);
            scatterRecord.isSkipPDF = false;
            return true;
        }

        double scatterPDF(const Ray & in, const HitRecord & record, const Ray & out) const override {
            //此处必须对out.getDirection()取单位向量
            return std::max(0.0, Vec3::dot(record.normalVector, out.getDirection().unitVector()) / PI);
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * rough = dynamic_cast<const Rough *>(&obj);
            if (rough == null) return false;
            return texture == rough->texture;
        }

        std::string toString() const override {
            std::string ret("Rough: Albedo = ");
            return ret + texture->toString();
        }
    };
}

#endif //RENDERERTEST_ROUGH_HPP
