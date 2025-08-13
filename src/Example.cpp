#include <Example.hpp>

using namespace my_library;
using namespace renderer;
using namespace std;

namespace renderer {
    void Example::test01() {
        //初始化SDL
        initSDLResources();

        const Camera cam(WINDOW_WIDTH, WINDOW_HEIGHT, Color3(0.7, 0.8, 1.0),
                         Point3(0.0, 2.0, 10.0), Point3(0.0, 2.0, 0.0),
                         80, 0.0, Range(0.0, 1.0), 10, 0.5, 10);
        SDL_Log("%s", cam.toString().c_str());

        //定义物体材质
        const auto groundMat = make_shared<Rough>(Color3(0.7, 0.6, 0.5));
        //向场景中添加物体
        HittableCollection list;
        const auto ground = make_shared<Sphere>(groundMat, Point3(0.0, -1000.0, 0.0), 1000);
        list.add(ground);

        //随机添加材质和物体
        const int range = 4;
        for (int a = -range; a <= range; a++) {
            for (int b = -range; b <= range; b++) {
                double chooseMat = randomDouble();
                Point3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());

                if (Point3::constructVector(Point3(4.0, 0.2, 0.0), center).length() > 0.9) {
                    shared_ptr<AbstractMaterial> material;
                    if (chooseMat < 0.8) {
                        auto albedo = Color3::randomColor() * Color3::randomColor();
                        material = make_shared<Rough>(albedo);
                        auto center2 = center + Vec3(0.0, randomDouble(0.0, 0.5), 0.0);
                        list.add(make_shared<Sphere>(material, center, center2, 0.2));
                    } else if (chooseMat < 0.95) {
                        auto albedo = Color3::randomColor(0.5, 1.0);
                        auto fuzz = randomDouble(0.0, 0.5);
                        material = make_shared<Metal>(albedo, fuzz);
                        list.add(make_shared<Sphere>(material, center, 0.2));
                    } else {
                        material = make_shared<Dielectric>(1.5);
                        list.add(make_shared<Sphere>(material, center, 0.2));
                    }
                }
            }
        }

        list.add(make_shared<Sphere>(make_shared<Dielectric>(1.5), Point3(0.0, 1.0, 0.0), 1.0));
        list.add(make_shared<Sphere>(make_shared<Rough>(Color3(0.4, 0.2, 0.1)), Point3(-4.0, 1.0, 0.0), 1.0));
        list.add(make_shared<Sphere>(make_shared<Metal>(Color3(0.7, 0.6, 0.5), 0.0), Point3(4.0, 1.0, 0.0), 1.0));

        //使用包含所有物体的 list 来构造 BVH 树
        auto bvhTree = make_shared<BVHTree>(list);

        //创建一个新的 collection，并将构造好的 BVH 树作为唯一的物体添加进去
        HittableCollection world;
        world.add(bvhTree);

        //渲染包含 BVH 树的 world
        const Uint32 start = SDL_GetTicks();
        cam.render(window, static_cast<Uint32 *>(surface->pixels), surface->format, world);
        const Uint32 end = SDL_GetTicks();
        SDL_Log("Render Time: %u ms", end - start);

        //显示图像
        SDL_Log("Render Complete");
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 * 1);
        releaseSDLResourcesImpl();
    }

    void Example::test02() {
        initSDLResources();

        const Camera cam(WINDOW_WIDTH, WINDOW_HEIGHT, Color3(0.7, 0.8, 1.0),
                         Point3(0.0, 2.0, 10.0), Point3(0.0, 2.0, 0.0),
                         100, 0.0, Range(0.0, 1.0), 10, 0.5, 10);
        SDL_Log("%s", cam.toString().c_str());

        const auto checker = make_shared<CheckerBoard>(Color3(), Color3(1.0, 1.0, 1.0), 0.32);
        const auto sphere1 = make_shared<Sphere>(make_shared<Rough>(checker), Point3(0.0, -10.0, 0.0), 10.0);
        const auto sphere2 = make_shared<Sphere>(make_shared<Rough>(checker), Point3(0.0, 10.0, 0.0), 10.0);

        HittableCollection list;
        list.add(sphere1);
        list.add(sphere2);

        auto bvhTree = make_shared<BVHTree>(list);
        HittableCollection world;
        world.add(bvhTree);
        const Uint32 start = SDL_GetTicks();
        cam.render(window, static_cast<Uint32 *>(surface->pixels), surface->format, world);
        const Uint32 end = SDL_GetTicks();
        SDL_Log("Render Time: %u ms", end - start);

        SDL_Log("Render Complete");
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 * 1);
        releaseSDLResourcesImpl();
    }

    void Example::test03() {
        initSDLResources();

        const Camera cam(WINDOW_WIDTH, WINDOW_HEIGHT, Color3(0.7, 0.8, 1.0),
                         Point3(2.0, 0.0, 10.0), Point3(2.0, 0.0, 0.0),
                         60, 0.0, Range(0.0, 1.0), 10, 0.5, 10);
        SDL_Log("%s", cam.toString().c_str());

        const auto texture = make_shared<Image>(imgSurface);
        const auto sphere = make_shared<Sphere>(make_shared<Rough>(texture), Point3(2.0, 0.0, 0.0), 2.0);
        HittableCollection list;
        list.add(sphere);

        auto bvhTree = make_shared<BVHTree>(list);
        HittableCollection world;
        world.add(bvhTree);
        const Uint32 start = SDL_GetTicks();
        cam.render(window, static_cast<Uint32 *>(surface->pixels), surface->format, world);
        const Uint32 end = SDL_GetTicks();
        SDL_Log("Render Time: %u ms", end - start);

        SDL_Log("Render Complete");
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 * 1);
        releaseSDLResourcesImpl();
    }

    void Example::test04() {
        initSDLResources();

        const Camera cam(WINDOW_WIDTH, WINDOW_HEIGHT, Color3(0.7, 0.8, 1.0),
                         Point3(0.0, 2.0, 4.0), Point3(0.0, 2.0, 0.0),
                         100, 0.0, Range(0.0, 1.0), 10, 0.5, 10);
        SDL_Log("%s", cam.toString().c_str());

        const auto texture = make_shared<PerlinNoise>(1.0, PerlinNoiseType::RANDOM_TURBULENCE_NET);
        const auto sphere1 = make_shared<Sphere>(make_shared<Rough>(texture), Point3(0.0, -1000.0, 0.0), 1000.0);
        const auto sphere2 = make_shared<Sphere>(make_shared<Rough>(texture), Point3(0.0, 2.0, 0.0), 2.0);

        HittableCollection list;
        list.add(sphere1);
        list.add(sphere2);

        auto bvhTree = make_shared<BVHTree>(list);
        HittableCollection world;
        world.add(bvhTree);
        const Uint32 start = SDL_GetTicks();
        cam.render(window, static_cast<Uint32 *>(surface->pixels), surface->format, world);
        const Uint32 end = SDL_GetTicks();
        SDL_Log("Render Time: %u ms", end - start);

        SDL_Log("Render Complete");
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 * 1);
        releaseSDLResourcesImpl();
    }

    void Example::test05() {
        initSDLResources();

        const Camera cam(WINDOW_WIDTH, WINDOW_HEIGHT, Color3(),
                         Point3(4.0, 2.0, 10.0), Point3(0.0, 2.0, 0.0),
                         80, 0.0, Range(0.0, 1.0), 10, 0.5, 10);
        SDL_Log("%s", cam.toString().c_str());

        const auto texture = make_shared<PerlinNoise>(3.0);
        const auto sphere1 = make_shared<Sphere>(make_shared<Rough>(texture), Point3(0.0, -1000.0, 0.0), 1000.0);
        const auto sphere2 = make_shared<Sphere>(make_shared<Rough>(texture), Point3(0.0, 2.0, 0.0), 2.0);

        //内部计算时使用无限制光强（HDR），仅在写入颜色时限制到LDR的范围
        const auto light = make_shared<DiffuseLight>(Color3(4.0, 4.0, 4.0));
        const auto rectangle = make_shared<Parallelogram>(light, Point3(3.0, 1.0, -2.0), Vec3(4.0, 0.0, 0.0), Vec3(0.0, 4.0, 0.0));

        HittableCollection list;
        list.add(sphere1);
        list.add(sphere2);
        list.add(rectangle);

        auto bvhTree = make_shared<BVHTree>(list);
        HittableCollection world;
        world.add(bvhTree);
        const Uint32 start = SDL_GetTicks();
        cam.render(window, static_cast<Uint32 *>(surface->pixels), surface->format, world);
        const Uint32 end = SDL_GetTicks();
        SDL_Log("Render Time: %u ms", end - start);

        SDL_Log("Render Complete");
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 * 1);
        releaseSDLResourcesImpl();
    }

    void Example::test06() {
        initSDLResources();

        const Camera cam(WINDOW_WIDTH, WINDOW_HEIGHT, Color3(0.7, 0.8, 1.0),
                         Point3(0.0, 0.0, 20.0), Point3(0.0, 0.0, 0.0),
                         80, 0.0, Range(0.0, 1.0), 10, 0.5, 10);
        SDL_Log("%s", cam.toString().c_str());

        const auto leftRedMat = make_shared<Rough>(Color3(1.0, 0.2, 0.2));
        const auto backGreenMat = make_shared<Rough>(Color3(0.2, 1.0, 0.2));
        const auto rightBlueMat = make_shared<Rough>(Color3(0.2, 0.2, 1.0));
        const auto upperOrangeMat = make_shared<Rough>(Color3(1.0, 0.5, 0.0));
        const auto lowerTealMat = make_shared<Rough>(Color3(0.2, 0.8, 0.8));

        const auto quad1 = make_shared<Parallelogram>(leftRedMat, Point3(-3.0, -2.0, 5.0), Vec3(0.0, 0.0, -4.0), Vec3(0.0, 4.0, 0.0));
        const auto quad2 = make_shared<Parallelogram>(backGreenMat, Point3(-2.0, -2.0, 0.0), Vec3(4.0, 0.0, 0.0), Vec3(0.0, 4.0, 0.0));
        const auto quad3 = make_shared<Parallelogram>(rightBlueMat, Point3(3.0, -2.0, 1.0), Vec3(0.0, 0.0, 4.0), Vec3(0.0, 4.0, 0.0));
        const auto quad4 = make_shared<Parallelogram>(upperOrangeMat, Point3(-2.0, 3.0, 1.0), Vec3(4.0, 0.0, 0.0), Vec3(0.0, 0.0, 4.0));
        const auto quad5 = make_shared<Parallelogram>(lowerTealMat, Point3(-2.0, -3.0, 5.0), Vec3(4.0, 0.0, 0.0), Vec3(0.0, 0.0, -4.0));

        HittableCollection list;
        list.add(quad1);
        list.add(quad2);
        list.add(quad3);
        list.add(quad4);
        list.add(quad5);

        auto bvhTree = make_shared<BVHTree>(list);
        HittableCollection world;
        world.add(bvhTree);
        const Uint32 start = SDL_GetTicks();
        cam.render(window, static_cast<Uint32 *>(surface->pixels), surface->format, world);
        const Uint32 end = SDL_GetTicks();
        SDL_Log("Render Time: %u ms", end - start);

        SDL_Log("Render Complete");
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 * 1);
        releaseSDLResourcesImpl();
    }

    void Example::test07() {
        initSDLResources();

        const Camera cam(WINDOW_WIDTH, WINDOW_HEIGHT, Color3(0.7, 0.8, 1.0),
                         Point3(10.0, 2.0, 10.0), Point3(0.0, 0.0, 0.0),
                         80, 0.0, Range(0.0, 1.0), 10, 0.5, 10);
        SDL_Log("%s", cam.toString().c_str());

        const auto red = make_shared<Rough>(Color3(.65, .05, .05));
        const auto white = make_shared<Rough>(Color3(.73, .73, .73));
        const auto green = make_shared<Rough>(Color3(.12, .45, .15));
        const auto light = make_shared<DiffuseLight>(Color3(15.0, 15.0, 15.0));

        HittableCollection list;

        const auto t1 = make_shared<Triangle>(red, Point3(-2.0, 0.0, 0.0), Point3(2.0, 0.0, 0.0), Point3(0.0, 2.0, 0.0));
        const auto t2 = make_shared<Triangle>(white, Point3(-2.0, 0.0, 0.0), Point3(2.0, 0.0, 0.0), Point3(0.0, -2.0, 0.0));
        const auto t3 = make_shared<Triangle>(green, Point3(0.0, 2.0, 0.0), Point3(0.0, -2.0, 0.0), Point3(2.0, 0.0, -2.0));

        list.add(t1);
        list.add(t2);
        list.add(t3);

        auto bvhTree = make_shared<BVHTree>(list);
        HittableCollection world;
        world.add(bvhTree);
        const Uint32 start = SDL_GetTicks();
        cam.render(window, static_cast<Uint32 *>(surface->pixels), surface->format, world);
        const Uint32 end = SDL_GetTicks();
        SDL_Log("Render Time: %u ms", end - start);

        SDL_Log("Render Complete");
        SDL_UpdateWindowSurface(window);
        SDL_Delay(1000 * 1);
        releaseSDLResourcesImpl();
    }

    void Example::test08() {
        initSDLResources();

        const Camera cam(WINDOW_WIDTH, WINDOW_HEIGHT, Color3(),
                         Point3(278, 278, -600), Point3(278, 278, 0),
                         80, 0.0, Range(0.0, 1.0), 10, 0.5, 10);
        SDL_Log("%s", cam.toString().c_str());

        HittableCollection list;

        //材质
        const auto red = make_shared<Rough>(Color3(.65, .05, .05));
        const auto white = make_shared<Rough>(Color3(.73, .73, .73));
        const auto green = make_shared<Rough>(Color3(.12, .45, .15));
        const auto light = make_shared<DiffuseLight>(Color3(15, 15, 15));
        const auto metal = make_shared<Metal>(Color3(0.8, 0.85, 0.88), 0.0);
        const auto glass = make_shared<Dielectric>(1.5);

        //墙壁
        const auto w1 = make_shared<Parallelogram>(green, Point3(555.0, 0.0, 0.0), Vec3(0.0, 0.0, 555.0), Vec3(0.0, 555.0, 0.0));
        const auto w2 = make_shared<Parallelogram>(red, Point3(0.0, 0.0, 555.0), Vec3(0.0, 0.0, -555.0), Vec3(0.0, 555.0, 0.0));
        const auto w3 = make_shared<Parallelogram>(white, Point3(0.0, 555.0, 0.0), Vec3(555.0, 0.0, 0.0), Vec3(0.0, 0.0, 555.0));
        const auto w4 = make_shared<Parallelogram>(white, Point3(0.0, 0.0, 555.0), Vec3(555.0, 0.0, 0.0), Vec3(0.0, 0.0, -555.0));
        const auto w5 = make_shared<Parallelogram>(white, Point3(555.0, 0.0, 555.0), Vec3(-555.0, 0.0, 0.0), Vec3(0.0, 555.0, 0.0));
        const auto w6 = make_shared<Parallelogram>(light, Point3(213.0, 554.0, 227.0), Vec3(130.0, 0.0, 0.0), Vec3(0.0, 0.0, 105.0));

        list.add(w1);
        list.add(w2);
        list.add(w3);
        list.add(w4);
        list.add(w5);
        list.add(w6);

        //初始化列表不能直接转换为std::array，需要显式调用array的构造
        const auto box1 = Parallelogram::constructBox(white, Point3(), Point3(165.0, 165.0, 165.0));
        const auto box2 = Parallelogram::constructBox(metal, Point3(), Point3(165.0, 330.0, 165.0));
        const auto trans1 = make_shared<Transform>(box1, array<double, 3>{0.0, -15.0, 0.0}, array<double, 3>{130, 0.0, 65.0}/*, array<double, 3>{1.5, 1.5, 1.5}*/);
        const auto trans2 = make_shared<Transform>(box2, array<double, 3>{0.0, 18.0, 0.0}, array<double, 3>{265.0, 0.0, 295.0}/*, array<double, 3>{1.5, 1.5, 1.5}*/);

        const auto iso1 = make_shared<Isotropic>(Color3());
        const auto iso2 = make_shared<Isotropic>(Color3(5.0, 5.0, 5.0));
        const auto volumeWhite = make_shared<ConstantMedium>(trans1, iso2, 0.01);
        const auto volumeBlack = make_shared<ConstantMedium>(trans2, iso1, 0.01);
        //list.add(trans1);
        list.add(trans2);

        const auto sphere = make_shared<Sphere>(glass, Point3(190.0, 90.0, 190.0), 90.0);
        list.add(sphere);

        vector<shared_ptr<AbstractHittable>> pdfList;
        pdfList.push_back(w6);
        pdfList.push_back(sphere);

        auto bvhTree = make_shared<BVHTree>(list);
        HittableCollection world;
        world.add(bvhTree);

        const Uint32 start = SDL_GetTicks();
        cam.render(window, static_cast<Uint32 *>(surface->pixels), surface->format, world, &pdfList);
        const Uint32 end = SDL_GetTicks();
        SDL_Log("Render Time: %u ms", end - start);
        SDL_Log("Render Complete");
        SDL_UpdateWindowSurface(window);

        sdlCheckErrorInt(IMG_SavePNG(surface, "../files/output.png"), "Save PNG", PRINT_MASSAGE);
        SDL_Delay(1000 * 1);
        releaseSDLResourcesImpl();
    }

    void Example::testAll() {
        test01();
        test02();
        test03();
        test04();
        test05();
        test06();
        test07();
    }
}

namespace {
    void initSDLResources() {
        registerReleaseSDLResources(releaseSDLResourcesImpl);
        int ret;

        //初始化SDL库
        ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
        sdlCheckErrorInt(ret, "Init", EXIT_PROGRAM);
        window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        sdlCheckErrorPtr(window, "Create Window", EXIT_PROGRAM);
        surface = SDL_GetWindowSurface(window);
        sdlCheckErrorPtr(surface, "Get Surface", EXIT_PROGRAM);
        pixels = static_cast<Uint32 *>(surface->pixels);
        sdlCheckErrorPtr(pixels, "Cast Pixel Pointer", EXIT_PROGRAM);

        //初始化字体库
        ret = TTF_Init();
        sdlCheckErrorInt(ret, "Init TTF", EXIT_PROGRAM);
        font = TTF_OpenFont("../files/consola.ttf", 30);
        sdlCheckErrorPtr(font, "Open Font", EXIT_PROGRAM);

        //初始化图像库
        ret = IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
        sdlCheckErrorInt(ret, "Init IMG", EXIT_PROGRAM);
        imgSurface = IMG_Load("../files/world_map.jpg");
        sdlCheckErrorPtr(imgSurface, "Get IMG Surface", EXIT_PROGRAM);

        //打印SDL信息
        SDL_Log("SDL Version: %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
        SDL_Log("SDL Color Format: %s", SDL_GetPixelFormatName(surface->format->format));
    }

    void releaseSDLResourcesImpl() {
        if (imgSurface != null) {
            releaseSDLResource(SDL_FreeSurface(imgSurface), "Free IMG Surface");
        }
        releaseSDLResource(IMG_Quit(), "Quit IMG");
        if (font != null) {
            releaseSDLResource(TTF_CloseFont(font), "Close Font");
        }
        releaseSDLResource(TTF_Quit(), "Quit TTF");
        if (surface != null) {
            releaseSDLResource(SDL_FreeSurface(surface), "Free Surface");
        }
        if (window != null) {
            releaseSDLResource(SDL_DestroyWindow(window), "Destroy Window");
        }
        releaseSDLResource(SDL_Quit(), "Quit");
    }
}