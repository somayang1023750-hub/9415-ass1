#include <iomanip>
#include <iostream>
#include <chrono>

#include <glm/ext/matrix_clip_space.hpp>
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

#include <GLFW/glfw3.h>


namespace {

constexpr int kInitialWidth = 1280;
constexpr int kInitialHeight = 720;
constexpr const char* kStarterWorkload = "minecraft-voxel-chunk";

craft::graphics::Camera gCamera(glm::vec3(16.0F, 22.0F, 48.0F), glm::vec3(0.0F, 1.0F, 0.0F), -90.0F, -20.0F);
bool gCursorCaptured = false;
bool gFirstMouse = true;
double gLastMouseX = 0.0;
double gLastMouseY = 0.0;

void startBenchmark(craft::graphics::Window& window,
                    craft::app::SceneBenchmark& benchmark) {
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
            << "benchmark_note=replace the tetrahedron with your voxel scene "
               "between the beginSceneFrame and endSceneFrame calls\n";
}

glm::mat4 fixedStarterViewProjection(int framebufferWidth,
                                     int framebufferHeight) {
  const float aspect = static_cast<float>(framebufferWidth) /
                       static_cast<float>(framebufferHeight);
  const glm::mat4 projection =
      glm::perspective(glm::radians(55.0F), aspect, 0.1F, 100.0F);
  const glm::mat4 view =
      glm::lookAt(glm::vec3(2.4F, 1.8F, 3.0F), glm::vec3(0.0F),
                  glm::vec3(0.0F, 1.0F, 0.0F));
  return projection * view;
}

GLFWcursorposfun gPrevCursorPosCallback = nullptr;
GLFWkeyfun gPrevKeyCallback = nullptr;

}  // namespace

int main() {
  try {
    craft::graphics::Window window(kInitialWidth, kInitialHeight,
                                   "Craft 3D starter");
    craft::graphics::Shader shader(
        craft::core::assetPath("shaders/basic.vert"),
        craft::core::assetPath("shaders/basic.frag"));

    craft::graphics::Chunk chunk;

    craft::app::AppUi appUi(window);
    craft::app::SceneBenchmark benchmark;

    craft::graphics::Texture atlasTexture(craft::core::assetPath("textures/texture.png"));
    craft::graphics::Texture characterTexture(craft::core::assetPath("textures/character.png"));
    craft::graphics::Humanoid character;

    gPrevCursorPosCallback = glfwSetCursorPosCallback(
        window.nativeHandle(),
        [](GLFWwindow* win, double xpos, double ypos) {
            if (gPrevCursorPosCallback != nullptr) {
                gPrevCursorPosCallback(win, xpos, ypos);
            }

    if (!gCursorCaptured) return;

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

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        gCursorCaptured = !gCursorCaptured;
        glfwSetInputMode(win, GLFW_CURSOR,
            gCursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        gFirstMouse = true;
    }
        });

    const bool commandLineBenchmark =
        craft::app::benchmarkRequestedFromEnvironment();
    if (commandLineBenchmark) {
      startBenchmark(window, benchmark);
    }

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

      if (gCursorCaptured) {
          GLFWwindow* nativeWin = window.nativeHandle();
          if (glfwGetKey(nativeWin, GLFW_KEY_W) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_W, GLFW_PRESS, deltaTime);
          if (glfwGetKey(nativeWin, GLFW_KEY_S) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_S, GLFW_PRESS, deltaTime);
          if (glfwGetKey(nativeWin, GLFW_KEY_A) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_A, GLFW_PRESS, deltaTime);
          if (glfwGetKey(nativeWin, GLFW_KEY_D) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_D, GLFW_PRESS, deltaTime);
          if (glfwGetKey(nativeWin, GLFW_KEY_SPACE) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_SPACE, GLFW_PRESS, deltaTime);
          if (glfwGetKey(nativeWin, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
              gCamera.processKeyboard(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS, deltaTime);
      }

      auto [framebufferWidth, framebufferHeight] = window.framebufferSize();
      if (framebufferWidth == 0 || framebufferHeight == 0) {
        window.swapBuffers();
        continue;
      }

      craft::app::DiagnosticsData diag;
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
          startBenchmark(window, benchmark);
          const auto resizedFramebuffer = window.framebufferSize();
          framebufferWidth = resizedFramebuffer.first;
          framebufferHeight = resizedFramebuffer.second;
      }

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
      atlasTexture.bind(0);
      shader.setInt("uTexture", 0);

      const float aspect = static_cast<float>(framebufferWidth) /
          static_cast<float>(framebufferHeight);
      const glm::mat4 viewProj = gCamera.projectionMatrix(aspect) * gCamera.viewMatrix();
      shader.setMat4("uViewProj", viewProj);
      shader.setVec3("uTint", glm::vec3(1.0F, 1.0F, 1.0F));

      atlasTexture.bind(0);
      shader.setInt("uTexture", 0);
      shader.setMat4("uModel", glm::mat4(1.0F));
      chunk.draw();

      characterTexture.bind(0);
      shader.setInt("uTexture", 0);

      glm::mat4 char1Model = glm::translate(glm::mat4(1.0F), glm::vec3(16.0F, 23.0F, 16.0F));
      character.draw(shader, char1Model, 0.0F, true);

      glm::mat4 char2Model = glm::translate(glm::mat4(1.0F), glm::vec3(16.0F, 11.0F, 24.0F));
      character.draw(shader, char2Model, 0.0F, true);

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
