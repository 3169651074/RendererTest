#include <Camera.hpp>
#include <util/HittablePDF.hpp>
#include <util/MixturePDF.hpp>

using namespace std;

namespace renderer {
    //递归获取指定光线的最终颜色
    Color3 rayColor(const Camera & cam, const HittableCollection & collection, const Ray & ray, Uint32 currentIterateDepth, const vector<shared_ptr<AbstractHittable>> * pdfObjectList) {
        if (currentIterateDepth >= cam.getRayTraceDepth()) {
            return Color3(); //达到最大递归深度，当前递归层次的颜色不再做出贡献
        }
        HitRecord record;
        ScatterRecord scatterRecord;

        //range最小值略微大于0，避免由于浮点数误差使得光线和物体碰撞点未精确落在物体表面上导致的错误
        if (collection.hit(ray, Range(0.001, INFINITY), record)) {
            Ray out;
            double pdfValue;

            /*
             * 尝试对record的材质属性进行向下转型，判断是否为发光材质
             * 不能直接通过material.scatter的返回值判断：Metal等非发光材质不一定返回true
             */
            const auto lightMaterial = std::dynamic_pointer_cast<AbstractLight>(record.material);
            if (lightMaterial) {
                //emitted实现光源背面剔除
                return lightMaterial->emitted(ray, record);
            } else {
                //如果非发光材质的scatter函数返回false，说明由于计算问题，当前光线无效
                if (!record.material->scatter(ray, record, scatterRecord)) {
                    return Color3();
                } else {
                    //TODO 多条阴影光线

                    /*CosinePDF pdf(record.normalVector);
                    out = Ray(record.hitPoint, pdf.generate(), ray.getTime());
                    pdfValue = pdf.value(out.getDirection());*/

                    /*HittablePDF pdf(light, record.hitPoint);
                    out = Ray(record.hitPoint, pdf.generate(), ray.getTime());
                    pdfValue = pdf.value(out.getDirection());*/

                    /*const auto p1 = make_shared<CosinePDF>(record.normalVector);
                    const auto p2 = make_shared<HittablePDF>(light, record.hitPoint);
                    vector<shared_ptr<AbstractPDF>> list = {p1, p2};
                    MixturePDF pdf(list);
                    out = Ray(record.hitPoint, pdf.generate(), ray.getTime());
                    pdfValue = pdf.value(out.getDirection());*/

                    /*//在光源上随机选点
                    const Point3 onLight(randomDouble(213.0, 343.0), 554.0, randomDouble(227.0, 332.0));

                    //构造从碰撞点到光源上随机点的向量
                    const Vec3 toLight = Point3::constructVector(record.hitPoint, onLight);
                    const double distanceSquare = toLight.lengthSquare();
                    const Vec3 toLightUnitize = toLight.unitVector();

                    //保证参数有效
                    //第一个条件：保证光源和被照亮的点在同一个半球方向内，从而保证光线不会被物体遮挡而无效
                    if (Vec3::dot(toLightUnitize, record.normalVector) < 0.0 || floatValueNearZero(toLightUnitize[1])) {
                        return Color3();
                    }

                    const double lightArea = (343.0 - 213.0) * (332.0 - 227.0);
                    //theta为光源平面法向量和点到光源上点向量的夹角，要取绝对值保证面积不为负
                    pdfValue = distanceSquare / (abs(Vec3::dot(toLightUnitize, Vec3(0, -1, 0))) * lightArea);
                    out = Ray(record.hitPoint, toLightUnitize, ray.getTime());*/

                    if (scatterRecord.isSkipPDF) {
                        //不计算PDF
                        return scatterRecord.attenuation * rayColor(cam, collection, scatterRecord.skipPDFRay, currentIterateDepth + 1, pdfObjectList);
                    }

                    //将要采样的物体和当前材质的PDF添加到列表
                    vector<shared_ptr<AbstractPDF>> pdfList;
                    if (pdfObjectList != null) {
                        pdfList.reserve(pdfObjectList->size());
                        for (const auto & item : *pdfObjectList) {
                            const auto ptr = make_shared<HittablePDF>(item, record.hitPoint);
                            pdfList.push_back(ptr);
                        }
                    }
                    pdfList.push_back(scatterRecord.pdf);

                    //构造混合PDF
                    MixturePDF pdf(pdfList);
                    out = Ray(record.hitPoint, pdf.generate(), ray.getTime());
                    pdfValue = pdf.value(out.getDirection());

                    //pdfValue有效性检查
                    if (isnan(pdfValue) || isinf(pdfValue) || floatValueNearZero(pdfValue)) {
                        return Color3();
                    }

                    const double scatterPDF = record.material->scatterPDF(ray, record, out);
                    const Color3 nextColor = rayColor(cam, collection, out, currentIterateDepth + 1, pdfObjectList);
                    return scatterPDF * scatterRecord.attenuation * nextColor / pdfValue;
                }
            }
            /*//更加符合渲染方程的写法
            const auto lightMaterial = std::dynamic_pointer_cast<AbstractLight>(record.material);
            Color3 emittedColor;
            if (lightMaterial) {
                //如果转型成功，说明是发光材质，返回发光颜色
                emittedColor = lightMaterial->emitted(ray, record);
            }

            if (!record.material->scatter(ray, record, attenuation, out, pdfValue)) {
                //如果不发生散射，则直接返回发光颜色
                return emittedColor;
            } else {
                //如果发生散射，则返回自发光颜色与递归计算的散射光线颜色的总和
                return emittedColor + attenuation *
                    rayColor(cam, collection, out, currentIterateDepth + 1, light);
            }*/
        } else {
            //光线没有和物体发生碰撞，返回背景颜色
            return cam.getBackgroundColor();
        }
    }

    void Camera::render(SDL_Window * window, Uint32 * pixels, const SDL_PixelFormat * format,
                        const HittableCollection & collection, const std::vector<std::shared_ptr<AbstractHittable>> * pdfObjectList) const
    {
        Uint32 lastRate = 0;
        SDL_Log("Render Start...");

        //逐个获取像素颜色，写入到屏幕的对应位置
        for (Uint32 i = 0; i < windowHeight; i++) {
            for (Uint32 j = 0; j < windowWidth; j++) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) { exit(1); }
                }

                //当前像素最终颜色
                Color3 color;

                //亚像素采样抗锯齿
                /*for (Uint32 k = 0; k < sampleCount; k++) {
                    //当前像素对应位置
                    const Point3 samplePoint =
                            pixelOrigin + (i + randomDouble(-sampleRange, sampleRange)) * viewPortPixelDy + (j + randomDouble(-sampleRange, sampleRange)) * viewPortPixelDx;

                    //单次离焦采样：在离焦半径内随机选取一个点，以这个点发射光线
                    Point3 rayOrigin = cameraCenter;
                    if (focusDiskRadius > 0.0) {
                        const Vec3 defocusVector = Vec3::randomPlaneVector(focusDiskRadius);
                        //使用视口方向向量定位采样点
                        rayOrigin = cameraCenter + defocusVector[0] * cameraU + defocusVector[1] * cameraV;
                    }

                    //在快门开启时段内随机找一个时刻发射光线并追踪
                    const Vec3 rayDirection = Point3::constructVector(rayOrigin, samplePoint).unitVector();
                    const Ray ray(rayOrigin, rayDirection, randomDouble(shutterRange.getMin(), shutterRange.getMax()));
                    color += rayColor(*this, collection, ray, 0);
                }*/

                /*
                 * 亚像素采样抗锯齿：改进版采样方法
                 * 将亚像素采样区域划分为网格，在每个小网格中随机选点发射光线，双重循环次数均为采样数的开方
                 *     则每一个小区域都有且仅有一个采样点
                 * 分层采样通过让采样点更加均匀地分散在采样区域内，降低了采样的方差
                 *
                 * 在相同的采样总数下，分层采样得到的图像噪点更少，图像收敛到最终清晰状态的速度更快
                 * 随机采样噪点连续大块，分层采样的噪点均匀细小，更加不明显
                 */
                const auto sqrtSampleCount = static_cast<size_t>(sqrt(sampleCount));
                //预计算倒数，加速除法
                const double reciprocalSqrtSampleCount = 1.0 / static_cast<double>(sqrtSampleCount);

                for (size_t sampleI = 0; sampleI < sqrtSampleCount; sampleI++) {
                    for (size_t sampleJ = 0; sampleJ < sqrtSampleCount; sampleJ++) {
                        const double offsetX = ((sampleJ + randomDouble()) * reciprocalSqrtSampleCount) - 0.5;
                        const double offsetY = ((sampleI + randomDouble()) * reciprocalSqrtSampleCount) - 0.5;
                        const Point3 samplePoint =
                                pixelOrigin + ((j + offsetX) * viewPortPixelDx) + ((i + offsetY) * viewPortPixelDy);

                        //单次离焦采样
                        Point3 rayOrigin = cameraCenter;
                        if (focusDiskRadius > 0.0) {
                            const Vec3 defocusVector = Vec3::randomPlaneVector(focusDiskRadius);
                            rayOrigin = cameraCenter + defocusVector[0] * cameraU + defocusVector[1] * cameraV;
                        }

                        //发射光线
                        const Vec3 rayDirection = Point3::constructVector(rayOrigin, samplePoint).unitVector();
                        const Ray ray(rayOrigin, rayDirection, randomDouble(shutterRange.getMin(), shutterRange.getMax()));
                        color += rayColor(*this, collection, ray, 0, pdfObjectList);
                    }
                }

                //取颜色平均值
                color /= sampleCount;

                //写入颜色
                color.writeColor(pixels + (i * windowWidth + j), format);
            }

            //每渲染1个百分比打印一次进度
            const auto rate = static_cast<Uint32>(i * 100 / windowHeight);
            if (rate / 1 != lastRate) {
                lastRate = rate / 1;
                SDL_Log("Rendered %u%%", rate);
                SDL_UpdateWindowSurface(window);
            }
        }
    }

    Camera::Camera(Uint32 windowWidth, Uint32 windowHeight, const Color3 & backgroundColor, const Point3 &center, const Point3 &target, double fov, double focusDiskRadius,
                   const Range &shutterRange, Uint32 sampleCount, double sampleRange, Uint32 rayTraceDepth)
                   :
            windowWidth(windowWidth), windowHeight(windowHeight), backgroundColor(backgroundColor),
            cameraCenter(center), cameraTarget(target), horizontalFOV(fov), focusDiskRadius(focusDiskRadius),
            shutterRange(shutterRange), sampleCount(sampleCount), sampleRange(sampleRange), rayTraceDepth(rayTraceDepth),
            focusDistance(Point3::distance(cameraCenter, cameraTarget))
    {
        const double thetaFOV = degreeToRadian(horizontalFOV);
        const double vWidth = 2.0 * tan(thetaFOV / 2.0) * focusDistance;
        const double vHeight = vWidth / (windowWidth * 1.0 / windowHeight);

        this->viewPortWidth = vWidth;
        this->viewPortHeight = vHeight;

        //相机W向量由center和target直接决定，V向量由指定向量（设定为指向Y轴正半轴）投影到过canter，垂直于U的平面上，U向量和二者垂直
        this->cameraW = Point3::constructVector(cameraCenter, cameraTarget).unitVector();
        const Vec3 straightUp(0.0, 1.0, 0.0);
        //W作为视口平面的法向量，利用叉乘构造垂直
        this->cameraU = Vec3::cross(cameraW, straightUp).unitVector();
        this->cameraV = Vec3::cross(cameraU, cameraW).unitVector();

        this->viewPortX = vWidth * cameraU;
        this->viewPortY = vHeight * -cameraV; //屏幕Y轴和空间坐标系的Y轴反向

        this->viewPortPixelDx = viewPortX / windowWidth;
        this->viewPortPixelDy = viewPortY / windowHeight;

        this->viewPortOrigin = cameraCenter + focusDistance * cameraW - viewPortX * 0.5 - viewPortY * 0.5;
        this->pixelOrigin = viewPortOrigin + viewPortPixelDx * 0.5 + viewPortPixelDy * 0.5;
    }

    string Camera::toString() const {
        string ret("Renderer Camera:\n");
        char buffer[4 * TOSTRING_BUFFER_SIZE] = { 0 };
        snprintf(buffer, 4 * TOSTRING_BUFFER_SIZE,
                 "\tWindow Size: %u x %u\n\tBackground Color: %s\n\t"
                 "Camera Direction: %s --> %s, FOV: %.4lf\n\t"
                 "Viewport Size: %.4lf x %.4lf\n\t"
                 "Viewport Base Vector: U = %s, V = %s, W = %s\n\t"
                 "Viewport Delta Vector: dx = %s, dy = %s\n\t"
                 "Viewport Origin: %s, Pixel Origin: %s\n\t"
                 "Sample Disk Radius: %.4lf, Focus Distance: %.4lf\n\t"
                 "Shutter %s\n\tSSAA Sample Count: %u, Range: %.2lf\n\t"
                 "Raytrace Depth: %u",
                 windowWidth, windowHeight, backgroundColor.toString().c_str(),
                 cameraCenter.toString().c_str(), cameraTarget.toString().c_str(),
                 horizontalFOV, viewPortWidth, viewPortHeight,
                 cameraU.toString().c_str(), cameraV.toString().c_str(), cameraW.toString().c_str(),
                 viewPortPixelDx.toString().c_str(), viewPortPixelDy.toString().c_str(),
                 viewPortOrigin.toString().c_str(), pixelOrigin.toString().c_str(),
                 focusDiskRadius, focusDistance, shutterRange.toString().c_str(), sampleCount, sampleRange, rayTraceDepth
        );
        return ret + buffer;
    }

    bool Camera::equals(const AbstractObject &obj) const {
        if (this == &obj)  return true;
        const auto * camera = dynamic_cast<const Camera *>(&obj);
        if (camera == null) return false;
        return windowWidth == camera->windowWidth &&
               windowHeight == camera->windowHeight &&
               backgroundColor == camera->backgroundColor &&
               cameraCenter == camera->cameraCenter &&
               cameraTarget == camera->cameraTarget &&
               horizontalFOV == camera->horizontalFOV &&
               viewPortWidth == camera->viewPortWidth &&
               viewPortHeight == camera->viewPortHeight &&
               cameraU == camera->cameraU &&
               cameraV == camera->cameraV &&
               cameraW == camera->cameraW &&
               viewPortX == camera->viewPortX &&
               viewPortY == camera->viewPortY &&
               viewPortPixelDx == camera->viewPortPixelDx &&
               viewPortPixelDy == camera->viewPortPixelDy &&
               viewPortOrigin == camera->viewPortOrigin &&
               pixelOrigin == camera->pixelOrigin &&
               focusDiskRadius == camera->focusDiskRadius &&
               focusDistance == camera->focusDistance &&
               shutterRange == camera->shutterRange &&
               sampleCount == camera->sampleCount &&
               sampleRange == camera->sampleRange &&
               rayTraceDepth == camera->rayTraceDepth;
    }
}