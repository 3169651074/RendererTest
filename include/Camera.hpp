#ifndef RENDERERTEST_CAMERA_HPP
#define RENDERERTEST_CAMERA_HPP

#include <basic/Ray.hpp>
#include <basic/Color3.hpp>
#include <hittable/HittableCollection.hpp>
#include <material/AbstractLight.hpp>

namespace renderer {
    /*
     * 相机类
     * 相机需要向场景中发射光线，并根据光线碰撞情况渲染图像，完成主要渲染任务
     */
    class Camera final : public AbstractObject {
    private:
        // ====== 窗口属性 ======
        Uint32 windowWidth;
        Uint32 windowHeight;

        //默认背景颜色，填充没有物体的区域
        Color3 backgroundColor;

        // ====== 相机属性 ======
        Point3 cameraCenter;
        Point3 cameraTarget;                    //相机放置在cameraCenter，看向cameraTarget，此两点间距为焦距
        double horizontalFOV;                   //水平方向的视角，构造时单位为角度，决定视口宽度

        //视口属性
        double viewPortWidth;
        double viewPortHeight;

        /*
         * 视口平面的基向量，用于定位视口
         * U指向相机的右侧（视口的右边界，V指向相机的上方（视口的上边界），W从center指向target
         * 当center和target确定时，仍然可以绕两点连线旋转
         * 三个基向量为单位向量
         */
        Vec3 cameraU, cameraV, cameraW;

        Vec3 viewPortX, viewPortY;              //视口平面方向向量
        Vec3 viewPortPixelDx, viewPortPixelDy;  //视口平面方向变化向量，用于定位每一个像素的空间位置
        Point3 viewPortOrigin;                  //视口原点（左上角）空间坐标
        Point3 pixelOrigin;                     //第一个像素的空间坐标

        //采样属性
        double focusDiskRadius;                 //光线虚化强度，采样平面的圆盘半径
        double focusDistance;                   //焦距

        Range shutterRange;                     //相机快门的开启时间段

        Uint32 sampleCount;                     //SSAA：每像素采样数
        double sampleRange;                     //SSAA：采样偏移半径

        Uint32 rayTraceDepth;                   //光线追踪深度

    public:
        Camera(Uint32 windowWidth, Uint32 windowHeight, const Color3 & backgroundColor,
               const Point3 & center, const Point3 & target, double fov, double focusDiskRadius,
               const Range & shutterRange, Uint32 sampleCount, double sampleRange, Uint32 rayTraceDepth);
        ~Camera() override = default;

        // ====== 对象操作函数 ======

        //渲染图像并写入到参数指定的窗口
        void render(SDL_Window * window, Uint32 * pixels, const SDL_PixelFormat * format,
                    const HittableCollection & collection, const std::vector<std::shared_ptr<AbstractHittable>> * pdfObjectList = null) const;

        // ====== 类封装函数 ======

        bool equals(const AbstractObject &obj) const override;
        std::string toString() const override;

        Uint32 getWindowWidth() const { return windowWidth; }
        Uint32 getWindowHeight() const { return windowHeight; }
        const Color3 & getBackgroundColor() const { return backgroundColor; }
        const Point3 & getCameraCenter() const { return cameraCenter; }
        const Point3 & getCameraTarget() const { return cameraTarget; }
        double getHorizontalFov() const { return horizontalFOV; }
        double getViewPortWidth() const { return viewPortWidth; }
        double getViewPortHeight() const { return viewPortHeight; }
        const Vec3 & getCameraU() const { return cameraU; }
        const Vec3 & getCameraV() const { return cameraV; }
        const Vec3 & getCameraW() const { return cameraW; }
        const Vec3 & getViewPortX() const { return viewPortX; }
        const Vec3 & getViewPortY() const { return viewPortY; }
        const Vec3 & getViewPortPixelDx() const { return viewPortPixelDx; }
        const Vec3 & getViewPortPixelDy() const { return viewPortPixelDy; }
        const Point3 & getViewPortOrigin() const { return viewPortOrigin; }
        const Point3 & getPixelOrigin() const { return pixelOrigin; }
        double getFocusDiskRadius() const { return focusDiskRadius; }
        double getFocusDistance() const { return focusDistance; }
        const Range & getShutterRange() const { return shutterRange; }
        Uint32 getSampleCount() const { return sampleCount; }
        double getSampleRange() const { return sampleRange; }
        Uint32 getRayTraceDepth() const { return rayTraceDepth; }
    };
}

#endif //RENDERERTEST_CAMERA_HPP
