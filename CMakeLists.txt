cmake_minimum_required(VERSION 3.28)
set(EXECUTABLE_NAME "RendererTest")
project(${EXECUTABLE_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

if (WIN32)
    link_directories("${CMAKE_SOURCE_DIR}/lib")
else ()
    #sudo apt update && sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-net-dev
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_net REQUIRED)
endif ()

#安装必要工具
#sudo apt update && sudo apt install -y gpg-agent wget

#下载并添加 Intel GPG 公钥，这个密钥用于验证从 Intel 仓库下载的软件包的完整性
#wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \ | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null

#添加 OneAPI 仓库到 APT 源列表，会创建一个新的文件，告诉系统从哪里获取 OneAPI 软件包
#echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list

#让包管理器获取新的软件包
#sudo apt update

#安装渲染工具包
#sudo apt install intel-renderkit（安装全部工具：sudo apt install intel-basekit）
#oidnDenoise --help

#配置运行时动态库查找路径
#echo "/opt/intel/oneapi/oidn/2.2/lib/" | sudo tee /etc/ld.so.conf.d/oneapi.conf && sudo ldconfig
#撤销：sudo rm /etc/ld.so.conf.d/oneapi.conf && sudo ldconfig

#set(OpenImageDenoise_DIR "/opt/intel/oneapi/oidn/2.2/lib/cmake/OpenImageDenoise/")
#传递编译参数：-DOpenImageDenoise_DIR=/opt/intel/oneapi/oidn/2.2/lib/cmake/OpenImageDenoise ..
#或在运行前使用环境配置脚本：source /opt/intel/oneapi/setvars.sh
#find_package(OpenImageDenoise REQUIRED)

#set(VTK_DIR "/home/zhang/Desktop/VTK/install/lib/cmake/vtk-9.5/")
#find_package(VTK REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/include")

add_executable(${EXECUTABLE_NAME}
        src/Main.cpp
        include/Global.hpp
        include/basic/AbstractTuple.hpp
        include/basic/Vec3.hpp
        include/basic/Point3.hpp
        include/basic/Color3.hpp
        include/util/Range.hpp
        include/basic/Ray.hpp
        include/Camera.hpp
        src/Camera.cpp
        include/AbstractObject.hpp
        include/hittable/AbstractHittable.hpp
        include/hittable/HittableCollection.hpp
        include/material/AbstractMaterial.hpp
        include/material/Metal.hpp
        include/hittable/Sphere.hpp
        include/hittable/AbstractHittable.hpp
        include/material/Rough.hpp
        include/material/Dielectric.hpp
        include/material/AbstractLight.hpp
        include/material/DiffuseLight.hpp
        include/material/Isotropic.hpp
        include/texture/AbstractTexture.hpp
        include/texture/CheckerBoard.hpp
        include/texture/SolidColor.hpp
        include/texture/Image.hpp
        include/texture/PerlinNoise.hpp
        include/util/PerlinGenerator.hpp
        src/util/PerlinGenerator.cpp
        include/hittable/Parallelogram.hpp
        include/box/AbstractBoundingBox.hpp
        include/box/AxisAlignedBoundingBox.hpp
        include/box/BVHTree.hpp
        include/hittable/Triangle.hpp
        include/hittable/Polyhedron.hpp
        include/Example.hpp
        src/Example.cpp
        include/test/Integration.hpp
        include/util/Matrix.hpp
        include/hittable/Transform.hpp
        include/hittable/ConstantMedium.hpp
        include/util/OrthonormalBase.hpp
        include/util/AbstractPDF.hpp
        include/util/UniformPDF.hpp
        include/util/CosinePDF.hpp
        include/util/HittablePDF.hpp
        include/util/MixturePDF.hpp
        src/util/Matrix.cpp
)

if (WIN32)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC mingw32 SDL2main)
endif ()

target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL2 SDL2_image SDL2_mixer SDL2_ttf SDL2_net)
#target_link_libraries(${EXECUTABLE_NAME} PUBLIC OpenImageDenoise)
#target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${VTK_LIBRARIES})