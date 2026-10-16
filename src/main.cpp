#include <iomanip>
#include <iostream>
#include <chrono>
#include <cmath>

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

#include "app/AppUi.h"
#include "app/SceneBenchmark.h"
#include "core/AssetPath.h"
#include "graphics/OpenGL.h"
#include "graphics/Shader.h"
#include "graphics/Window.h"
#include "graphics/Camera.h"
#include "graphics/Chunk.h"
#include "graphics/Texture.h"
#include "graphics/Humanoid.h"
#include "graphics/NaturalProps.h"

#include <GLFW/glfw3.h>

namespace {

constexpr int kInitialWidth = 1280;
constexpr int kInitialHeight = 720;
constexpr const char* kStarterWorkload = "minecraft-voxel-chunk";

craft::graphics::Camera lookingAt(glm::vec3 eye, glm::vec3 target) {
  const auto direction = glm::normalize(target - eye);
  return craft::graphics::Camera(eye, glm::vec3(0,1,0),
      glm::degrees(std::atan2(direction.z, direction.x)), glm::degrees(std::asin(direction.y)));
}
craft::graphics::Camera defaultCamera() { return lookingAt({42,30,50}, {16,9,16}); }
craft::graphics::Camera gCamera = defaultCamera();
bool gBenchmarkRunning = false;

bool gCursorCaptured = false;
bool gFirstMouse = true;
double gLastMouseX = 0.0;
double gLastMouseY = 0.0;

void startBenchmark(craft::graphics::Window& window,
                    craft::app::SceneBenchmark& benchmark, craft::app::SceneSettings& settings) {
  gCamera = defaultCamera();
  settings = {};
  gBenchmarkRunning = true;
  window.setFramebufferSize(craft::app::kBenchmarkFramebufferWidth,
                            craft::app::kBenchmarkFramebufferHeight);
  window.setVSync(false);
  benchmark.start();
}

void printBenchmarkResult(const craft::app::BenchmarkSnapshot& result,
                          int framebufferWidth, int framebufferHeight) {
  std::cout << std::fixed << std::setprecision(4)
            << "benchmark_result sample_count=" << result.measuredFrames
            << " average_ms=" << result.averageMilliseconds
            << " average_fps=" << result.averageFramesPerSecond
            << " resolution=" << framebufferWidth << 'x' << framebufferHeight
            << " workload=" << kStarterWorkload << '\n'
            << "benchmark_settings=fixed-camera neutral-poses default-trees fog-off wireframe-off\n";
}

GLFWcursorposfun gPrevCursorPosCallback = nullptr;
GLFWkeyfun gPrevKeyCallback = nullptr;

}  // namespace

int main() {
  try {
    craft::graphics::Window window(kInitialWidth, kInitialHeight,
                                   "Craft - Voxel Tower");
    craft::graphics::Shader shader(
        craft::core::assetPath("shaders/basic.vert"),
        craft::core::assetPath("shaders/basic.frag"));

    craft::graphics::Chunk chunk;

    craft::app::AppUi appUi(window);
    craft::app::SceneBenchmark benchmark;

    craft::graphics::Texture atlasTexture(craft::core::assetPath("textures/texture.png"), true);
    craft::graphics::Texture characterTexture(craft::core::assetPath("textures/character.png"));
    craft::graphics::Humanoid character;
    craft::graphics::NaturalProps naturalProps;
    const glm::vec3 origins[] = {{16,static_cast<float>(craft::graphics::VoxelWorld::towerRoofHeight()+1),16}, {16, static_cast<float>(craft::graphics::VoxelWorld::terrainHeight(16,24)+1),24}};

    gPrevCursorPosCallback = glfwSetCursorPosCallback(
        window.nativeHandle(),
        [](GLFWwindow* win, double xpos, double ypos) {
            if (gPrevCursorPosCallback != nullptr) {
                gPrevCursorPosCallback(win, xpos, ypos);
            }

    if (!gCursorCaptured || gBenchmarkRunning) return;

    if (gFirstMouse) {
        gLastMouseX = xpos;
        gLastMouseY = ypos;
        gFirstMouse = false;
    }
    const float xoffset = static_cast<float>(xpos - gLastMouseX);
    const float yoffset = static_cast<float>(gLastMouseY - ypos);
    gLastMouseX = xpos;
    gLastMouseY = ypos;

    gCamera.processMouseMovement(xoffset, yoffset);
        });

    gPrevKeyCallback = glfwSetKeyCallback(
        window.nativeHandle(),
        [](GLFWwindow* win, int key, int scancode, int action, int mods) {
            if (gPrevKeyCallback != nullptr) {
                gPrevKeyCallback(win, key, scancode, action, mods);
            }

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS && !gBenchmarkRunning) {
        gCursorCaptured = !gCursorCaptured;
        glfwSetInputMode(win, GLFW_CURSOR,
            gCursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        gFirstMouse = true;
    }
        });

    const bool commandLineBenchmark =
        craft::app::benchmarkRequestedFromEnvironment();
    if (commandLineBenchmark) {
      startBenchmark(window, benchmark, appUi.settings());
    }

    auto& settings = appUi.settings();
    std::cout << "renderer=" << glGetString(GL_RENDERER) << "\nOpenGL=" << glGetString(GL_VERSION) << '\n';
    std::cout << "scene solid_blocks=" << chunk.solidBlockCount() << " visible_faces=" << chunk.triangleCount()/2
              << " triangles=" << chunk.triangleCount() << '\n';

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    auto lastFrameTime = std::chrono::steady_clock::now();

    while (!window.shouldClose()) {
      const auto now = std::chrono::steady_clock::now();
      const float deltaTime =
          std::chrono::duration<float>(now - lastFrameTime).count();
      lastFrameTime = now;

      window.pollEvents();

      bool cameraBlocked = false;
      if (gCursorCaptured && !gBenchmarkRunning) {
          GLFWwindow* nativeWin = window.nativeHandle();
          if (glfwGetKey(nativeWin, GLFW_KEY_W) == GLFW_PRESS) {
              const auto before = gCamera.position();
              gCamera.processKeyboard(GLFW_KEY_W, GLFW_PRESS, deltaTime, settings.collision ? &chunk.world() : nullptr);
              cameraBlocked = settings.collision && glm::length(gCamera.position() - before) < 0.00001F;
          }
          if (glfwGetKey(nativeWin, GLFW_KEY_S) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_S, GLFW_PRESS, deltaTime, settings.collision ? &chunk.world() : nullptr);
          if (glfwGetKey(nativeWin, GLFW_KEY_A) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_A, GLFW_PRESS, deltaTime, settings.collision ? &chunk.world() : nullptr);
          if (glfwGetKey(nativeWin, GLFW_KEY_D) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_D, GLFW_PRESS, deltaTime, settings.collision ? &chunk.world() : nullptr);
          if (glfwGetKey(nativeWin, GLFW_KEY_SPACE) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_SPACE, GLFW_PRESS, deltaTime, settings.collision ? &chunk.world() : nullptr);
          if (glfwGetKey(nativeWin, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS, deltaTime, settings.collision ? &chunk.world() : nullptr);
      }

      auto [framebufferWidth, framebufferHeight] = window.framebufferSize();
      if (framebufferWidth == 0 || framebufferHeight == 0) {
        window.swapBuffers();
        continue;
      }
      if (gBenchmarkRunning && (framebufferWidth != craft::app::kBenchmarkFramebufferWidth ||
                                framebufferHeight != craft::app::kBenchmarkFramebufferHeight)) {
        window.setFramebufferSize(craft::app::kBenchmarkFramebufferWidth, craft::app::kBenchmarkFramebufferHeight);
        const auto fixedSize = window.framebufferSize();
        framebufferWidth = fixedSize.first;
        framebufferHeight = fixedSize.second;
        benchmark.start(); // Discard mixed-size samples and repeat the warm-up.
      }

      craft::app::DiagnosticsData diag;
      diag.cameraBlocked = cameraBlocked;
      diag.cameraPosition = gCamera.position();
      diag.cameraYaw = gCamera.yaw();
      diag.cameraPitch = gCamera.pitch();
      diag.chunkWidth = craft::graphics::Chunk::kWidth;
      diag.chunkHeight = craft::graphics::Chunk::kHeight;
      diag.chunkDepth = craft::graphics::Chunk::kDepth;
      diag.triangleCount = chunk.triangleCount();
      diag.solidBlockCount = chunk.solidBlockCount();
      appUi.updateDiagnostics(diag);

      appUi.beginFrame();
      const auto benchmarkState = benchmark.snapshot();
      if (appUi.draw(benchmarkState) && !benchmarkState.running) {
          std::cout << "[Benchmark] Started 60 warmup + 240 measured frames...\n";
          startBenchmark(window, benchmark, appUi.settings());
          const auto resizedFramebuffer = window.framebufferSize();
          framebufferWidth = resizedFramebuffer.first;
          framebufferHeight = resizedFramebuffer.second;
      }

      if (!gBenchmarkRunning)
        for (auto& characterSettings : settings.characters) characterSettings.advanceWalk(deltaTime);

      benchmark.beginSceneFrame();

      glViewport(0, 0, framebufferWidth, framebufferHeight);
      glClearColor(0.45F, 0.68F, 1.0F, 1.0F);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (appUi.isWireframe()) {
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      else {
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }

      shader.use();

      const float aspect = static_cast<float>(framebufferWidth) /
          static_cast<float>(framebufferHeight);
      const glm::mat4 viewProj = gCamera.projectionMatrix(aspect) * gCamera.viewMatrix();
      shader.setMat4("uViewProj", viewProj);
      shader.setVec3("uTint", glm::vec3(1.0F, 1.0F, 1.0F));
      shader.setInt("uFogEnabled", settings.fog ? 1 : 0);
      shader.setVec3("uCameraPosition", gCamera.position());
      shader.setVec3("uFogColor", glm::vec3(0.45F, 0.68F, 1.0F));
      shader.setFloat("uFogStart", settings.fogStart);
      shader.setFloat("uFogEnd", settings.fogEnd);

      atlasTexture.bind(0);
      shader.setInt("uTexture", 0);
      shader.setMat4("uModel", glm::mat4(1.0F));
      chunk.draw();
      naturalProps.draw(shader,settings.trees);

      characterTexture.bind(0);
      for (std::size_t i = 0; i < settings.characters.size(); ++i) {
        const auto& control = settings.characters[i];
        auto model = glm::translate(glm::mat4(1), origins[i] + control.offset);
        model = glm::rotate(model, glm::radians(control.yaw), glm::vec3(0,1,0));
        character.draw(shader, model, control.effectivePose());
      }

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      const bool benchmarkCompleted = benchmark.endSceneFrame();

      if (benchmarkCompleted) {
        printBenchmarkResult(benchmark.snapshot(), framebufferWidth,
                             framebufferHeight);
      }
      
      // Dear ImGui and presentation remain outside the measured scene region.
      appUi.render();
      window.swapBuffers();

      if (benchmarkCompleted) {
        gBenchmarkRunning = false;
        window.setVSync(true);
        if (commandLineBenchmark) {
          window.requestClose();
        }
      }
    }
  } catch (const std::exception& error) {
    std::cerr << "Craft failed: " << error.what() << '\n';
    return 1;
  }

  return 0;
}
