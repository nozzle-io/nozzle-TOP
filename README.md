# nozzle-TOP

> This codebase is currently in its AI-slob prototyping phase: the code runs on momentum, vibes, and plausible intent.
> Proper debugging will be introduced once demand graduates from hypothetical to measurable.

TouchDesigner Custom TOP plugins for [nozzle](https://github.com/nozzle-io/nozzle) — local inter-process GPU texture sharing.

## Plugins

### Nozzle Send

Filter TOP (1 input). Publishes the input texture via nozzle and passes it through as output.

### Nozzle Receive

Generator TOP (0 inputs). Receives a nozzle frame and outputs it as a texture. Cooks every frame to poll for new frames.

Both plugins use **CPUMem mode** — texture data is transferred through CPU memory. This works on all platforms but is slower than direct GPU sharing. Performance may be improved in a future CUDA mode release (Windows/NVIDIA only).

## Parameters

Both plugins share the same parameters:

| Parameter | Default | Description |
|-----------|---------|-------------|
| Sender Name | `nozzle` | Nozzle sender name to publish or subscribe to |
| Application Name | `TouchDesigner` | Application name metadata |

## Build

### Prerequisites

- CMake 3.20+
- C++17 compiler
- macOS 12+ or Windows 10+

### macOS

```bash
git clone --recurse-submodules https://github.com/nozzle-io/nozzle-TOP.git
cd nozzle-TOP
cmake -B build
cmake --build build
```

Output: `plugin/NozzleSend.plugin` and `plugin/NozzleReceive.plugin` (universal binary: arm64 + x86_64).

### Windows

```bash
git clone --recurse-submodules https://github.com/nozzle-io/nozzle-TOP.git
cd nozzle-TOP
cmake -B build
cmake --build build --config Release
```

Output: `plugin/NozzleSend.dll` and `plugin/NozzleReceive.dll`.

### Custom Architecture

To override the default universal binary on macOS:

```bash
cmake -B build -DCMAKE_OSX_ARCHITECTURES=arm64
```

## Install

Copy the `.plugin` (macOS) or `.dll` (Windows) files to your TouchDesigner Custom OPs folder. See [Custom Operators — Plugin Folder Locations](https://docs.derivative.ca/Custom_Operators#Using_Custom_OPs_-_Plugin_Folder_Locations).

## Pixel Format Support

| TouchDesigner | Nozzle |
|---------------|--------|
| BGRA8 Fixed | BGRA8 UNORM |
| RGBA8 Fixed | RGBA8 UNORM |
| RGBA16 Fixed | RGBA16 UNORM |
| RGBA16 Float | RGBA16 Float |
| RGBA32 Float | RGBA32 Float |
| Mono 8/16/16F/32F Fixed/Float | R8/R16/R16F/R32F |
| RG 8/16/16F/32F Fixed/Float | RG8/RG16/RG16F/RG32F |

## License

MIT

Third-party dependencies:

- [nozzle](https://github.com/nozzle-io/nozzle) — MIT
