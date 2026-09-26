# STELLAR PROGRAM
<!--
[![build](https://github.com/DreGi0/apogee/actions/workflows/build.yml/badge.svg)](https://github.com/DreGi0/apogee/actions/workflows/build.yml) ![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
-->
![OpenGL 4.6](https://img.shields.io/badge/OpenGL-4.6%20core-5586A4)
![License](https://img.shields.io/badge/license-PolyForm%20Noncommercial-lightgrey)

This is a [Kerbal Space Program](https://www.kerbalspaceprogram.com/) like space flight game, built in C++20. 
It is a Personal project to explore physics simulation and low-level graphics/physics programming, built as 
a portfolio/hobby project.

It's being build only by me while learning graphics programming on the go. On the commit history can be found all the learning
curve with mistakes included.

> This project is being assisted with Claude Code.

## Status

Early development. Core windowing and OpenGL up (GLFW + GLAD). Not playable yet, but trust me it will :)

## Stack

[`C++20`](https://cppreference.com/cpp/20) | 
[`OpenGL 4.6 core`](https://www.opengl.org/) | 
[`GLFW`](https://www.glfw.org/) | 
[`GLAD`](https://glad.dav1d.de/) | 
[`CMake`](https://cmake.org/) | 
[`Docker`](https://www.docker.com/)

## Installation
Requirements:
- [CMake](https://cmake.org/) at least 3.28
- [Ninja](https://ninja-build.org/)
- [vcpkg](https://vcpkg.io/), with `VCPKG_ROOT` set in your environment
  
> Note: this is still an unstable rolling version so there is a high chance errors will occur

```bash
git clone https://github.com/DreGi0/StellarProgram.git
cd StellarProgram

cmake --preset debug
cmake --build build/debug

./build/debug/StellarProgram
```

Opens a window rendering the current OpenGL scene. `ESC` closes it.

## Contributions

Not accepting pull requests at this stage. As mentioned at the beginning, it is a solo hobby/portfolio project. Bug 
reports and suggestions are welcome via Issues tho.

## License

[PolyForm Noncommercial License 1.0.0](https://polyformproject.org/licenses/noncommercial/1.0.0)

## Gallery

I'll gather more media to display later on, but I promise there will be a rich gallery section in here.
