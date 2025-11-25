# mstd 库使用指南

我会按照你的要求，将构建步骤中创建构建目录的命令从 “mkdir -p mstd/build && cd mstd/build” 修改为 “mkdir build && cd build”，确保指令更简洁，符合常见的构建目录创建习惯。

# mstd 库使用指南

mstd 是轻量 C++ 工具库，提供 Base64、CRC32、字符串 / 路径处理等功能，支持 Windows/Linux/macOS。

## 1. 依赖



*   编译器：支持 C++11+（GCC/Clang/MSVC）

*   构建工具：CMake 3.10+

## 2. 构建步骤

### 2.1 准备源码

获取含 `CMakeLists.txt`、`mstd`（头文件）、`base`（示例）的源码目录，进入源码根目录。

### 2.2 构建与编译



```
\# 1. 创建并进入构建目录（直接在源码根目录下创建 build）

mkdir build && cd build

\# 2. 生成构建文件（可选指定安装路径）

cmake .. \[-DCMAKE\_INSTALL\_PREFIX=/安装路径]

\# 3. 编译（-j4 为4线程加速，提升编译效率）

cmake --build . --config Release -j4
```

## 3. 安装（可选）



```
\# 安装到指定路径（需管理员权限，如 Linux 下加 sudo）

make install
```



*   头文件：`/安装路径/include/mstd/xxx.h`

*   静态库：`/安装路径/lib/libmstd.a`（Linux/macOS）或 `安装路径\lib\mstd.lib`（Windows）

## 4. 运行示例

示例程序生成在 `build/bin` 目录（构建目录下的 bin 文件夹）：



```
\# 进入示例所在目录

cd build/bin

\# Linux/macOS 运行示例

./example

\# Windows 运行示例

example.exe
```

## 5. 项目引用

### 5.1 链接已安装库



```
\# 查找 mstd 库文件（指定库所在路径）

find\_library(MSTD\_LIB mstd PATHS /库路径)

\# 引入 mstd 头文件路径

include\_directories(/头文件路径)

\# 生成你的项目可执行文件

add\_executable(你的项目 源码.cpp)

\# 链接 mstd 库

target\_link\_libraries(你的项目 PRIVATE \${MSTD\_LIB})

\# 指定 C++ 标准（需 C++11 及以上）

target\_compile\_features(你的项目 PRIVATE cxx\_std\_11)
```

### 5.2 嵌入子模块（推荐，无需单独安装）



```
\# 将 mstd 作为子模块引入（指定 mstd 源码在你项目中的路径，如 third\_party/mstd）

add\_subdirectory(third\_party/mstd)

\# 生成你的项目可执行文件

add\_executable(你的项目 源码.cpp)

\# 链接 mstd 库

target\_link\_libraries(你的项目 PRIVATE mstd)

\# 引入 mstd 头文件（对应子模块路径）

target\_include\_directories(你的项目 PRIVATE third\_party/mstd)

\# 指定 C++ 标准（需 C++11 及以上）

target\_compile\_features(你的项目 PRIVATE cxx\_std\_11)
```

## 6. 核心功能



| 头文件                    | 功能                   |
| ---------------------- | -------------------- |
| mstd/base64.h          | Base64 编码 / 解码       |
| mstd/crc32.h           | CRC32 校验（含 SSE 优化）   |
| mstd/hex.h             | 十六进制与字节流互转           |
| mstd/ipstring.h        | IP 地址解析 / 格式化        |
| mstd/path\_string.h    | 跨平台路径处理              |
| mstd/string.h          | 字符串操作（修剪 / 分割等）      |
| mstd/string\_convert.h | 编码转换（UTF-8/UTF-16 等） |
| mstd/system.h          | 跨平台系统信息获取            |

修改后创建构建目录的指令更简洁，同时补充了 “进入源码根目录” 的前置说明，避免操作路径混淆。如果还需要调整其他指令格式，或者补充特定平台的注意事项，都可以告诉我。