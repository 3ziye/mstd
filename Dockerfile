# 基础镜像：Ubuntu 24.04（稳定版）
FROM ubuntu:24.04

# 非交互模式：避免 apt 安装时弹窗
ENV DEBIAN_FRONTEND=noninteractive

# 1. 安装开发依赖（C++工具链 + CMake + 调试工具）
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    gdb \
    cmake \
    make \
    ninja-build \
    git \
    vim \
    sudo \
    rsync \
    zip \
    unzip \
    clang-format \
    cppcheck \
    && rm -rf /var/lib/apt/lists/*

# 3. 配置 GDB 调试支持
RUN echo "set auto-load safe-path /" >> /etc/gdb/gdbinit

# 4. 设置工作目录（与 devcontainer.json 挂载路径一致）
WORKDIR /workspace

# 6. 配置编译器环境变量
ENV CC=gcc
ENV CXX=g++
ENV PATH="/usr/local/bin:${PATH}"

# 容器启动时进入 bash
CMD ["/bin/bash"]