# RendererTest
一个简单的CPU端光线追踪器实现  
查看`files/Ray-Tracing.pdf`以获取技术细节

## Build
此仓库中包含的源代码可直接在Windows和Linux上编译  
如果在Windows上编译，请确保依赖库（SDL2）所需文件已经被正确放置：bin目录中包含.dll动态库文件，lib目录中包含静态库文件。  
如果在Linux上编译，请先通过系统包管理器安装SDL2  
--> 推荐使用CLion IDE一键编译

### Windows
克隆仓库后直接使用CMake编译即可

### Ubuntu/Debian
1. 使用系统包管理器安装SDL2依赖库和GCC
```
sudo apt update && sudo apt install libsdl2-dev libsdl2-image-dev gcc g++
```
2. 使用oneapi安装Intel OIDN降噪器  
安装必要工具
```
sudo apt update && sudo apt install -y gpg-agent wget
```

下载并添加 Intel GPG 公钥，这个密钥用于验证从 Intel 仓库下载的软件包的完整性
```
wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \ | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
```

添加 OneAPI 仓库到 APT 源列表，会创建一个新的文件，告诉系统从哪里获取 OneAPI 软件包
```
echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list
```

让包管理器获取新的软件包并安装OIDN（安装全部工具：sudo apt install intel-basekit）
```
sudo apt update && sudo apt install intel-renderkit
```

配置运行时动态库查找路径（撤销修改：sudo rm /etc/ld.so.conf.d/oneapi.conf && sudo ldconfig）
```
echo "/opt/intel/oneapi/oidn/2.2/lib/" | sudo tee /etc/ld.so.conf.d/oneapi.conf && sudo ldconfig
```

然后使用CMake编译，同时传入CMake参数（需要替换为实际的安装路径，由于版本不同安装路径可能不同）
```
-DOpenImageDenoise_DIR=/opt/intel/oneapi/oidn/2.2/lib/cmake/OpenImageDenoise/
```

3. 设置环境变量（运行时需要）  
1.使用CLion运行
```
在右上角项目名 -> 下拉菜单 -> Edit Configuration -> Environment Variables中添加环境变量
1.在新终端中执行source /opt/intel/oneapi/setvars.sh
2.echo $LD_LIBRARY_PATH
3.将输出的字符串完整复制作为环境变量值
4.保存修改
```

2.在终端中运行（新开一个终端），第二条命令无需严格执行，在终端中运行编译好的可执行文件即可
```
1.source /opt/intel/oneapi/setvars.sh
2.cd bin && ./RendererTest
```
