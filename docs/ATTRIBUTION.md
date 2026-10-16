# Assets, libraries and development assistance

- `assets/textures/character.png`: bundled 64×64 classic-arm skin; original source supplied by the project author: https://namemc.com/skin/80e5c3c708c61fd9 . Both humanoids use this same skin. The implementation renders its base layer; slim arms and optional second-layer clothing are not implemented.
- `assets/textures/texture.png`: unmodified course starter atlas, byte-identical to https://github.com/fogleman/Craft/blob/master/textures/texture.png (SHA-256: `85fa47c085586a3766e6758fd5eefa89384da10f5497877bf03b862c5e1e80f2`). Upstream project: https://github.com/fogleman/Craft (MIT licence). No new external terrain textures were added.
- Other bundled textures: retained from the course starter, unused by the current scene.
- Course starter: OpenGL window/context, shader utility, asset lookup, Dear ImGui integration, CMake dependency wiring and the 60/240-frame benchmark harness.
- GLFW 3.3.10: https://github.com/glfw/glfw (window, input, context).
- GLAD 2.0.8: https://github.com/Dav1dde/glad (OpenGL function loader on Windows/Linux).
- GLM 1.0.3: https://github.com/g-truc/glm (vectors, matrices and transforms).
- Dear ImGui 1.92.9b: https://github.com/ocornut/imgui (diagnostics and controls).
- stb, commit pinned in root CMakeLists.txt: https://github.com/nothings/stb (`stb_image` texture decoding).
- FastNoise2 1.1.1 and FastSIMD: retained starter dependency, https://github.com/Auburn/FastNoise2 ; current analytic terrain does not call it.
- Catch2 3.15.3: https://github.com/catchorg/Catch2 (optional tests only).
- OpenAI Codex assisted with the texture mapping fixes, fog, collision, hierarchy controls, regression tests and documentation in this branch. This attribution describes assistance and does not imply independent student authorship or instructor approval.

Upstream dependency repositories contain their respective licences. No new runtime library was added for the extension features.
