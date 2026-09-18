FROM ubuntu:24.04 AS builder
LABEL authors="gio"

# Dependencies
RUN apt-get update && apt-get install -y cmake g++ make git libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl-dev mesa-common-dev && rm -rf /var/lib/apt/lists/*
# Work Directory
WORKDIR /app

# Copy files
COPY CMakeLists.txt .
COPY src/ ./src/
COPY vendor/ ./vendor/
COPY assets/ ./assets/

# Build the project
ARG CMAKE_BUILD_TYPE=Release
RUN cmake -B build -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} && cmake --build build

# Runtime stage
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y libx11-6 libxrandr2 libxinerama1 libxcursor1 libxi6 libgl1 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy compiled executable from builder
COPY --from=builder /app/build/Apogee .
COPY assets/ ./assets/

ENTRYPOINT ["./Apogee"]