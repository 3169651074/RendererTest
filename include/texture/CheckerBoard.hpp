#ifndef RENDERERTEST_CHECKERBOARD_HPP
#define RENDERERTEST_CHECKERBOARD_HPP

#include <texture/SolidColor.hpp>

namespace renderer {
    /*
     * 棋盘纹理：物体表面被划分为规则方格，可以指定两种方格的纹理
     */
    class CheckerBoard final : public AbstractTexture {
    private:
        double scale; //表面方格的缩放比例
        std::shared_ptr<AbstractTexture> even;
        std::shared_ptr<AbstractTexture> odd;

    public:
        CheckerBoard(const std::shared_ptr<AbstractTexture> &even,
                     const std::shared_ptr<AbstractTexture> &odd, double scale) : scale(scale), even(even), odd(odd) {}

        explicit CheckerBoard(const Color3 & evenColor = Color3(1.0, 1.0, 1.0), const Color3 & oddColor = Color3(), double scale = 1.0) {
            this->even = std::make_shared<SolidColor>(evenColor);
            this->odd = std::make_shared<SolidColor>(oddColor);
            this->scale = scale;
        }
        ~CheckerBoard() override = default;

        Color3 value(const std::pair<double, double> &uvPair, const Point3 &point) const override {
            //根据传入的点坐标返回even纹理或odd纹理
            int sum = 0;
            for (Uint32 i = 0; i < 3; i++) {
                sum += static_cast<int>(point[i] / scale);
            }
            return sum % 2 == 0 ? even->value(uvPair, point) : odd->value(uvPair, point);
        }

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * checkBoard = dynamic_cast<const CheckerBoard *>(&obj);
            if (checkBoard == null) return false;
            return scale == checkBoard->scale && even == checkBoard->even && odd == checkBoard->odd;
        }

        std::string toString() const override {
            return "Checker Board Texture: Even: " + even->toString() + ", Odd: " + odd->toString() + ", Scale: " + std::to_string(scale);
        }

        double getScale() const { return scale; }
        const std::shared_ptr<AbstractTexture> &getEven() const { return even; }
        const std::shared_ptr<AbstractTexture> &getOdd() const { return odd; }
    };
}

#endif //RENDERERTEST_CHECKERBOARD_HPP
