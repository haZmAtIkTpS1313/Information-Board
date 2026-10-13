FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qtbase5-dev \
    qtmultimedia5-dev \
    libqt5sql5-sqlite \
    libqt5sql5-psql \
    libgl1-mesa-glx \
    libx11-6 \
    libxext6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .