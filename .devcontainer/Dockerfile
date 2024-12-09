FROM mcr.microsoft.com/devcontainers/base:ubuntu-20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    xauth \
    libx11-dev \
    libxext-dev \
    libxft-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxrandr-dev \
    libglfw3-dev \
    libglew-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    mesa-utils \
    && apt-get clean && rm -rf /var/lib/apt/lists/*


ENV LIBGL_ALWAYS_INDIRECT=1
ENV NO_AT_BRIDGE=1

