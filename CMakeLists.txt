cmake_minimum_required(VERSION 3.10)

# 项目名称
project(mstd LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 根据平台设置宏定义
if(WIN32)
    add_definitions(-DMSTD_WINDOWS)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DMSTD_LINUX)
elseif(APPLE)
    add_definitions(-DMSTD_MACOS)
endif()

# 源文件列表
set(SOURCES
    mstd/base64.cpp
    mstd/crc32.cpp
    mstd/crc32_sse.cpp
    mstd/hex.cpp
    mstd/ipstring.cpp
    mstd/path_string.cpp
    mstd/string.cpp
    mstd/string_convert.cpp
    mstd/system.cpp
)

# 头文件列表
set(HEADERS
    mstd/base64.h
    mstd/crc32.h
    mstd/hex.h
    mstd/ipstring.h
    mstd/path_string.h
    mstd/platform.h
    mstd/string.h
    mstd/string_convert.h
    mstd/system.h
)

# 创建库目标
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

install(FILES ${HEADERS} DESTINATION include)


add_executable(example
    base/base.cpp
)

# 为example添加mstd的头文件引用
target_include_directories(example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}  # 引用mstd的头文件
)

target_link_libraries(example PRIVATE
    ${PROJECT_NAME}  # 链接mstd库
)


# 安装规则
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)