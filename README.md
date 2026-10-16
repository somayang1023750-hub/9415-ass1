# Craft — Voxel Tower

C++20 / OpenGL 3.3 scene for COMP3421/COMP9415. One deterministic **32 × 32 × 32** voxel chunk, a hollow brick tower with a doorway, two independently controlled six-part humanoids, two hierarchical trees and 12 flower/grass decorations. The default scene contains **10,573 solid voxels**, **5,126 exposed faces**, and **10,252 terrain/tower triangles** (character geometry is additional).

## Build and run

Use the course starter's CMake toolchain. On Linux, install a C/C++ compiler, CMake ≥ 3.18, Python with Jinja2 (GLAD generator), and X11 development headers (`libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`). CMake fetches the upstream dependencies at the versions pinned in `CMakeLists.txt`; network access to GitHub is needed on first configuration.

```bash
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 4
ctest --test-dir build --output-on-failure
./build/Craft
```

Ninja is also supported (`-G Ninja` in a separate build directory). On Windows, use a suitable CMake/Visual Studio toolchain; the executable may be under `build/Release`. On macOS the starter uses the system OpenGL framework. Linux Make and Ninja builds have been tested in the cloud environment; an actual VLAB run remains a local submission check.

For a headless Linux run, install Xvfb/xauth and use `xvfb-run -a ./build/Craft`. Mesa software rendering works for functional verification.

## Controls

| Input | Action |
| --- | --- |
| Tab | Capture/release mouse; movement is active only while captured |
| WASD + mouse | Fly forward/back/strafe and look |
| Space / Left Shift | Fly up / down |
| Esc | Exit |
| Wireframe Mode | Inspect the indexed triangle mesh |
| Camera collision | Toggle a 0.4-unit eye AABB against occupied voxels and the outer flight bounds |
| Distance fog | Toggle distance fog; adjust near/far distances |
| Character hierarchy | Select Tower/Ground; adjust root offset/yaw, head yaw and all four limb angles; enable a coordinated walking pose |
| Walking pose / Animate walk | Preview a stride or play the walk cycle; adjust phase, stride angle and cycles/second |
| Tree hierarchy | Select Tree 1/2; move/rotate the root and bend left/right branches with their child crowns |
| Reset selected tree | Restore the selected tree to its stable default pose |
| Reset selected to neutral | Restore the selected object's default root and stable neutral pose |
| Run benchmark | Reset to the fixed camera/default settings and measure the scene |

The default view uses camera `(42, 30, 50)` looking at `(16, 9, 16)`. The two neutral roots are `(16, 25, 16)` and `(16, 7, 24)`; both use the bundled 64×64 classic-arm skin. Each arm rotates at its shoulder and each leg at its hip. Individual part edits leave siblings unchanged. Walking pairs each arm with the opposite leg and supports independent phase/speed per character; it is a walk-in-place pose without root locomotion. Root transformations affect the entire hierarchy.

## Implemented requirements

See [requirement mapping](docs/REQUIREMENTS.md), [asset attribution](docs/ATTRIBUTION.md).

- Core: indexed exposed-face chunk mesh, deterministic terrain and tower, perspective fly camera, diagnostics, sharp atlas tiles and top > side > bottom GLSL brightness.
- Challenge 1: two separately controlled textured humanoids plus two textured trees. Each tree has a trunk, fork joints/branches, a top crown, and two crowns parented to the side branches. Bending one branch moves only that branch and its child crown; moving the root moves the whole tree.
- Challenge 2: **three selected features** — seeded Perlin fBm terrain height variation, distance fog, and basic camera collision/bounds. No extra runtime library is needed for these features.

Height comes from **FastNoise2 Perlin + four-octave fBm**, using seed **941526**, base frequency **0.075**, gain **0.5**, lacunarity **2.0**, and octave amplitude normalisation **1.875**. The rule is `clamp(round(9 + 10 * noise), 4, 15)`; the default 32×32 heightmap spans 4…14. This noise changes the actual number of solid voxels in each column, not only texture/material selection. No generation runs inside the measured render loop.

Top voxels are grass and the next two layers always dirt. Separate seeded coherent value noise (`0x941526`) varies soil depth and adds soil pockets inside the lower rock volume. The current underside contains 853 stone and 171 dirt blocks; no stone reaches the protected surface layers. The tower occupies x/z 13…19; its level floor is the maximum terrain height under that footprint (currently y=11), and its roof is 13 voxels above (currently y=24). A solid foundation fills gaps down to the terrain; the interior stays hollow, with a south doorway. The tower character stands on the roof and the other character uses the generated ground height. Fixed seeds make each run reproducible. Collision is a fly-camera safety boundary, with no gravity or player physics. The outer eye-position range is x −16…48, y −16…48, z −16…64; disable collision for unrestricted inspection. If you enable collision while already inside geometry, disable it and fly clear first.

The terrain atlas uses top-left image coordinates without an upload flip. Grass top = tile (0,13), side/fringe = (0,14), bottom/dirt = (6,15). Face brightness is 1.0 top, 0.82 north/south, 0.68 east/west, 0.50 bottom. Nearest filtering and inset tile UVs prevent interpolation/atlas bleeding.

## Natural props and cutout textures

Tree roots stand at terrain cells `(9,22)` and `(26,10)` and are anchored to their generated surface heights. `NaturalProps` uses shared indexed cube meshes for wood/leaves and double-sided crossed quads for 12 decorative plants. The trees are hierarchical objects; the small flowers/grass are static decorations. Existing atlas tiles are: wood end grain `(4,13)`, bark `(4,14)`, leaves `(14,15)`, grass `(0,12)`, red flower `(2,12)`, yellow flower `(4,12)`, blue flower `(6,12)`.

The terrain texture loader converts the atlas's magenta key to alpha in memory; the GLSL shader discards alpha below 0.5 before writing colour/depth. No texture asset is replaced. Cutouts use nearest sampling, need no alpha blending/sorting, and remain compatible with fog and wireframe. Tree/plant placement and mesh construction happen before frame measurement. These decorative models are separate from the logical voxel chunk and do not alter its solid/face count.

## Benchmark

```bash
CRAFT_BENCHMARK=1 ./build/Craft
# Headless Linux:
xvfb-run -a -s '-screen 0 1920x1080x24' env CRAFT_BENCHMARK=1 ./build/Craft
```

The benchmark resets the fixed 32³ scene view to neutral characters with walk animation disabled, default tree poses, fog off and wireframe off, locks camera/scene controls, renders 60 unmeasured warm-up frames, then measures exactly 240 frames at **1920×1080** and prints arithmetic mean ms/frame. The measured region includes clear/uniforms/scene draws and GPU completion; it excludes asset/mesh creation, Dear ImGui and buffer swaps. Automatic benchmark mode exits on completion. Renderer/version and counts are printed for reproducibility. See [validation evidence](docs/VALIDATION.md); cloud llvmpipe timings are software-renderer results, not GPU/VLAB performance claims.
