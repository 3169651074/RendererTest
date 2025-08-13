#ifndef RENDERERTEST_IMAGE_HPP
#define RENDERERTEST_IMAGE_HPP

#include <texture/AbstractTexture.hpp>

namespace renderer {
    /*
     * 图像纹理，使用SDL Surface指针管理图像
     */
    class Image final : public AbstractTexture {
    private:
        SDL_Surface * surface;

        //获取指定像素坐标的颜色（RGB）
        Color3 getPixelColor(Uint32 x, Uint32 y) const {
            //SDL_LockSurface(surface);
            //使用surface->format->BytesPerPixel确定每个像素的字节大小
            //***不同图片格式，surface的像素字节数不同，不能固定
            const Uint8 * pixelAddr = (Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
            const Uint32 pixel = *((Uint32 *)pixelAddr);

            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            //SDL_UnlockSurface(surface);
            return Color3(r, g, b);
        }

    public:
        explicit Image(SDL_Surface * surface) : surface(surface) {}
        ~Image() override = default;

        //返回图像对应位置的像素颜色
        Color3 value(const std::pair<double, double> & uvPair, const Point3 & point) const override {
            if (surface == null) {
                return Color3(0.0, 1.0, 1.0);
            }

            const double u = Range(0.0, 1.0).clamp(uvPair.first);
            const double v = 1.0 - Range(0.0, 1.0).clamp(uvPair.second);
            const auto x = static_cast<Uint32>(u * (surface->w - 1));
            const auto y = static_cast<Uint32>(v * (surface->h - 1));

            return Color3(getPixelColor(x, y) / 255.0); //将颜色值归一化到[0, 1]范围
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return false;
            const auto * image = dynamic_cast<const Image *>(&obj);
            return surface == image->surface;
        }

        std::string toString() const override {
            return "Image Texture: Pointer = " + std::to_string(reinterpret_cast<uintptr_t>(surface));
        }
    };
}

#endif //RENDERERTEST_IMAGE_HPP
