#pragma once

#include "app/CustomUi.h"
#include "app/SceneBenchmark.h"

namespace craft::graphics {
    class Window;
}

namespace craft::app {

    class AppUi {
    public:
        explicit AppUi(graphics::Window& window);
        ~AppUi();

        AppUi(const AppUi&) = delete;
        AppUi& operator=(const AppUi&) = delete;

        void beginFrame();
        [[nodiscard]] bool draw(const BenchmarkSnapshot& benchmark);
        void render();

        void updateDiagnostics(const DiagnosticsData& data) {
            customUi_.updateDiagnostics(data);
        }

        [[nodiscard]] bool isWireframe() const noexcept {
            return customUi_.isWireframe();
        }

    private:
        [[nodiscard]] bool drawBenchmarkSection(const BenchmarkSnapshot& benchmark) const;

        CustomUi customUi_;
    };

}  // namespace craft::app