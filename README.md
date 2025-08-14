# RendererTest
一个简单的CPU端光线追踪器实现  
请查看`files/Ray-Tracing.pdf`以获取技术细节

## Build
此仓库中包含的源代码可直接在Windows和Linux上编译  
如果在Windows上编译，请确保依赖库（SDL2）所需文件已经被正确放置：bin目录中包含.dll动态库文件，lib目录中包含静态库文件。  
如果在Linux上编译，请先通过系统包管理器安装SDL2  
### Ubuntu/Debian
`sudo apt update && sudo apt install libsdl2-dev libsdl2_image-dev libsdl2_ttf-dev  libsdl2_net-dev `
