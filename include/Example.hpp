#ifndef RENDERERTEST_EXAMPLE_HPP
#define RENDERERTEST_EXAMPLE_HPP

#include <Camera.hpp>
#include <material/Rough.hpp>
#include <material/Metal.hpp>
#include <material/Dielectric.hpp>
#include <material/DiffuseLight.hpp>
#include <material/Isotropic.hpp>
#include <hittable/Sphere.hpp>
#include <hittable/Parallelogram.hpp>
#include <hittable/Triangle.hpp>
#include <hittable/Polyhedron.hpp>
#include <hittable/Transform.hpp>
#include <hittable/ConstantMedium.hpp>
#include <texture/CheckerBoard.hpp>
#include <texture/Image.hpp>
#include <texture/PerlinNoise.hpp>
#include <box/BVHTree.hpp>

namespace {
    //窗口比例
    constexpr double ASPECT_RATIO = 16.0 / 9.0;
    //窗口尺寸
    constexpr Uint32 WINDOW_WIDTH = 800;
    constexpr Uint32 WINDOW_HEIGHT = static_cast<int>(WINDOW_WIDTH / ASPECT_RATIO);

    //SDL变量
    SDL_Window * window = null;
    SDL_Surface * surface = null;
    SDL_Surface * imgSurface = null;
    TTF_Font * font = null;
    Uint32 * pixels = null;

    void initSDLResources();
    void releaseSDLResourcesImpl();
}

namespace renderer {
    /**
     * Example class for testing the renderer.
     * This class contains various test cases to demonstrate the rendering capabilities.
     */
    class Example {
    public:
        /**
         * Test all examples in the Example class.
         */
         static void testAll();

        /**
         * Test case 1: Basic rendering with spheres.
         */
        static void test01();

        /**
         * Test case 2: Rendering with checkerboard texture on spheres.
         */
        static void test02();

        /**
         * Test case 3: Rendering a textured sphere.
         */
        static void test03();

        /**
         * Test case 4: Rendering with a ground plane and a sphere.
         */
        static void test04();

        /**
         * Test case 5: Rendering with a light source and multiple spheres.
         */
        static void test05();

        /**
         * Test case 6: Rendering a scene with multiple parallelograms.
         */
        static void test06();

        /**
         * Test case 7: Rendering a triangle scene.
         */
        static void test07();

        /**
         * Test case 8: Rendering a polyhedron.
         */
         static void test08();
    };
}

#endif //RENDERERTEST_EXAMPLE_HPP
