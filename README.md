# MiniRasterizer: A CPU-Based Software Rasterizer

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20(x64)-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A lightweight software rasterizer built from scratch in C++. It simulates the modern graphics pipeline entirely on the CPU, featuring programmable shaders, a state-machine-based API, and a real-time material previewer using SFML.

<br>
![MiniRasterizer Demo](Docs/Demo_BlinnPhong01.gif)  
<br>
<br>
![MiniRasterizer Demo](Docs/Demo_SimpleToonLit01.gif)  

*(Above: Real-time UI controls for Blinn-Phong and Toon shading)*
<br>

## Features

* **5-Stage Render Pipeline**: Implements the classic 5-stage pipeline:
    1.  Vertex Processing
    2.  Triangle Processing
    3.  Rasterization
    4.  Fragment Processing
    5.  Framebuffer Operations
* **Programmable Shaders**: An `IShader` interface allows for custom logic in `RunVertexShader` and `RunFragmentShader`, mimicking HLSL/GLSL.
* **Clean Bind/Draw API**: The core `RenderPipeline` class acts as a state machine. You `BindMaterial()` and `Draw()` geometry, separating state from execution.
* **Built-in Shaders**: Includes implementations for:
    * **Blinn-Phong** (Specular Highlights)
    * **Simple Toon Shader** (Cel shading + Rim Lighting)
* **Perspective-Correct Interpolation**: Correctly interpolates `Varyings` (like normals and view-space positions) across 3D space using the `1/w` method, avoiding 2D-screen-space artifacts.
* **Real-time UI**: A simple material previewer built with SFML allows for live tweaking of all shader properties (colors, smoothness, rim width, etc.) and light settings.

## The Render Pipeline

This project's goal is to demonstrate the *architecture* of a modern graphics pipeline.

### General GPU-Based Pipeline

In engines like **Unity**, **Unreal**, or **Godot**, the GPU executes a highly optimized hardware pipeline. The (simplified) conceptual stages are:

`[IMAGE PLACEHOLDER: A diagram of the 5 stages]`

1.  **Vertex Processing**: The Vertex Shader runs on each vertex, transforming it from 3D model space into 2D clip space (`Vec4`).
2.  **Triangle Processing**: Vertices are assembled into triangles. Triangles outside the view (frustum culling) are discarded.
3.  **Rasterization**: This fixed-function stage determines which pixels on the screen are *inside* each triangle, generating "fragments" (potential pixels).
4.  **Fragment Processing**: The Fragment Shader (or Pixel Shader) runs on each fragment to calculate its final color (e.g., applying lighting and textures).
5.  **Framebuffer Operations**: The final fragments pass a Z-Test (depth test) and are written into the final image (the Color Buffer).

### MiniRasterizer's CPU Design

MiniRasterizer simulates this entire process on the CPU. The `RenderPipeline` class manages the 5 stages, calling the "programmable" `IShader` stages (1 and 4) and executing the "fixed-function" stages (2, 3, and 5) itself.

`[IMAGE PLACEHOLDER: Your system architecture diagram]`

#### A Note on `Vec3` Simulation

To focus on the *pipeline architecture* and *data flow* rather than complex matrix math, this project uses a simplified math model for transformations. A full GPU pipeline uses `Vec4` and `Mat4x4` (4x4 matrices) for all transformations.

* `ShaderUtils::TransformModelToWorld` & `TransformWorldToView`:
    * **What it does:** Simulates a Model and View matrix (translation only) using simple `Vec3` addition and subtraction.
    * **Why:** This avoids the need to implement a full `Mat4x4` class just to move the object and camera.
    * **Full Implementation:** A full implementation would use matrix multiplication: `worldPos = ModelMatrix * Vec4(modelPos, 1.0)`.

* `ShaderUtils::TransformViewToClip`:
    * **What it does:** This is the one place where a `Vec4` is **critically** generated. It performs the perspective projection manually.
    * **Why:** The `w` component (`w = -viewPos.z`) is **essential** for the pipeline to function. The Rasterizer (Stage 3) **must** have this `w` value to perform:
        1.  **Perspective Divide** (`ndc.x = clip.x / clip.w`)
        2.  **Perspective-Correct Interpolation** (using `1/w`)
    * This function correctly simulates the work of a Projection Matrix.

## Prerequisites

* Windows 10/11
* Visual Studio 2022 (with the "Desktop development with C++" workload)
* [vcpkg (C++ Package Manager)](https://vcpkg.io/en/getting-started.html)
* [Git](https://git-scm.com/downloads)

## How to Build and Run

This project uses `vcpkg` in **Manifest Mode**, so no global dependencies are required. Visual Studio will automatically download and install `SFML` for this project.

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/](https://github.com/)[YourUsername]/[YourRepoName].git
    cd MiniRasterizer
    ```

2.  **Open in Visual Studio:**
    * Double-click `MiniRasterizer.sln` to open the solution.
    * Ensure your build configuration is set to `x64 Debug` or `x64 Release`.

3.  **Build & Run:**
    * Press **F5** (or `Build > Build Solution`).
    * **Wait:** The first build will take a few minutes. Visual Studio will automatically detect `vcpkg.json`, download `vcpkg`, and then use it to install `SFML` into a local `vcpkg_installed` folder.
    * The application will compile and run.

> **Note:** If you get a C1083 error (`Cannot open include file: 'SFML/Graphics.hpp'`), ensure that `Use Vcpkg Manifest` is set to `Yes` in the project properties (`Project > Properties > vcpkg > Use Vcpkg Manifest`).

## Usage

* The scene displays a sphere rendered with the default **Blinn-Phong** shader.
* Use the **sliders** on the left and right to control shader properties and light settings in real-time.
* Press **'C'** to cycle between the available shaders (Blinn-Phong and Toon).

## Project Notes
* **This project serves as a personal learning endeavor** to deepen my understanding of the graphics pipeline by recreating its core architecture. As this is an ongoing learning exercise, any feedback or corrections on conceptual misunderstandings are greatly appreciated. Future enhancements, such as texture mapping or additional shader models, may be explored as time allows.
* **High CPU Usage is Expected:** This is a CPU-based rasterizer. All 5 pipeline stages, including per-pixel fragment shading, are running on your CPU cores. This is intended to demonstrate the fundamental computational work that a GPU normally handles in parallel hardware.
* **Allman Style:** The codebase adheres to the Allman bracing style, as used in Unreal Engine development.

## References

* [GAMES101: Introduction to Computer Graphics](https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html)

## License

This project is licensed under the **MIT License**. See the `LICENSE` file for details.
