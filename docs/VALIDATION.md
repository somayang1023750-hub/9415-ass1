# Validation — 16 October 2026

Environment: Debian 13 x86-64 cloud machine; GCC 14.2.0; CMake 4.4.4; Mesa 25.0.7 / llvmpipe LLVM 19.1.7. Headless graphics use Xvfb. Build/dependency files remain outside the repository.

## Verified

- CMake Release build with Ninja: passed.
- Independent CMake Release build with **Unix Makefiles**: passed; existing pinned dependency sources reused, separate output directory. That executable also completed its 1920×1080 benchmark. BUILD_TESTING was OFF for this secondary build; tests ran in the Ninja build.
- CTest: **12/12 tests passed**. Includes actual atlas pixel selection, 64×64 skin mapping, repeatable world generation/final solid count, replacement/regeneration, camera wall collision/sliding/tunnelling, lower flight bounds/input/time/pitch, underside collision, seeded material constraints, transform hierarchy sibling isolation, paired walking limb angles/hip pivots, and independent walk play/pause/reset.
- Actual framebuffer captures: scene, wireframe, atlas close-up, neutral hierarchy close-up, distance fog, and collision contact. Temporary validation captures are kept outside the checkout; they are not submission materials.
- Native X11 mouse events verified the fog checkbox changes the scene and restores the baseline when toggled off; the hierarchy panel expands correctly. Native clicks also enable the walking pose and continuous limb animation, verified by framebuffer differences.
- Fog image check outside the UI: 200,241 scene pixels changed toward the sky colour; mean RGB distance to sky fell from 213.67 to 137.93; the background sky pixel stayed unchanged.
- Refinement captures verified the irregular soil/rock boundary and the two opposite walk phases; no rendering GL errors. The underside contains 854 stone / 170 dirt blocks. Camera tests descend below the island with collision enabled and confirm upward movement stops at its solid underside.
- Full scene: **10,171 final solid voxels**, **4,820 exposed quads**, **9,640 chunk triangles**. Two six-box humanoids add 144 triangles (72 each), so total scene triangles are 9,784. Diagnostics count chunk geometry and do not include characters or Dear ImGui.
- Benchmark: 60 warm-up + exactly **240 measured frames**; 1920×1080; default camera `(42,30,50)` looking at `(16,9,16)`; neutral characters; fog/wireframe off. Latest Ninja result: **8.2686 ms/frame**. The raw log is `docs/benchmark.txt`. The separate Make run has its own timing; neither run is a repeatability promise.
- Atlas provenance: course atlas is byte-identical to the upstream Craft atlas; SHA-256 and original source are recorded in `ATTRIBUTION.md`; upstream MIT notice retained.

## Scope and limits

Timings above describe a CPU software renderer on this machine. They do not describe hardware-GPU or VLAB performance. VLAB, Windows and macOS were not available for runtime validation. Report/video preparation and GitLab submission are outside the requested scope.

Tests do not assert marks or full rubric acceptance. Camera collision is eye-AABB voxel collision and outer bounds without gravity, entity collision or character collision. The optional normal mapping/multiple lights features are not selected. Skin rendering uses the provided classic-arm base layer, without optional clothing overlays.
