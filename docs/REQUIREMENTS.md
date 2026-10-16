# Requirement mapping

Reference: the COMP3421/COMP9415 26T3 MiniCraft Core specification supplied by the author. This records implementation evidence, not a grading prediction.

| Requirement | Implementation / evidence |
| --- | --- |
| Core 1: starter pipeline, indexed geometry, diagnostics | `Chunk`, `BoxMesh`, `AppUi`, `CustomUi`; VAO/VBO/EBO draw calls; visible faces/triangles/camera/chunk counts |
| Core 2: logical 32³ chunk, terrain, structure, ≥100 solids, repeatability, hidden-face removal | `VoxelWorld`, `Chunk`; 10,171 final solids, deterministic sin/cos height and seeded soil/stone noise, hollow tower with door; 4,820 exposed quads |
| Core 3: separate transforms, perspective fly camera, dt movement, vertical controls, pitch clamp, cursor toggle, depth | `Camera`, `main.cpp`; keys documented in README/UI; camera regression tests |
| Core 4: atlas and top/side/bottom tiles, sharp filtering, GLSL shading | `Voxel.h`, `Texture`, `basic.vert/.frag`; grass top/side/dirt UVs, nearest filtering and padding; actual atlas regression test |
| Challenge 1: two textured multipart objects, parent-child transforms, neutral poses, attribution | Tower/Ground humanoids; root → hips/torso → legs/shoulders → head/arms; independent UI controls; `HumanoidPose.h` plus coordinated static/animated walk; `docs/ATTRIBUTION.md` |
| Challenge 2, selected feature 1 | Deterministic procedural terrain height variation in `VoxelWorld::terrainHeight` |
| Challenge 2, selected feature 2 | Adjustable distance fog in GLSL with shared world-space distance for chunk and characters |
| Challenge 2, selected feature 3 | Small-step eye AABB voxel collision, axis sliding and outer bounds in `Camera` / `VoxelWorld` |
| Benchmark | Course harness, 60 warm-up / exactly 240 measured frames, 1920×1080, fixed settings and camera, GPU finish; renderer/version/count logs |

## Validation scope

- Actual VLAB compatibility/performance must be checked on a VLAB machine. Debian Linux Make/Ninja builds are verified here; VLAB access was not available.

Assignment report/video preparation is outside this task, per the author's request.

No extra credit is claimed for optional unimplemented features (multiple lights, normal mapping, block editing, infinite terrain, physics).
