#ifndef RENDERERTEST_ABSTRACTTEXTURE_HPP
#define RENDERERTEST_ABSTRACTTEXTURE_HPP

#include <basic/Point3.hpp>
#include <basic/Color3.hpp>

namespace renderer {
    /*
     * 纹理类，部分材质拥有纹理，提供随空间变化的表面效果
     */
    class AbstractTexture : public AbstractObject {
    public:
        ~AbstractTexture() override = default;

        //纹理映射函数，通过二维UV坐标获取对应位置颜色
        virtual Color3 value(const std::pair<double, double> & uvPair, const Point3 & point) const = 0;
    };
}

#endif //RENDERERTEST_ABSTRACTTEXTURE_HPP
