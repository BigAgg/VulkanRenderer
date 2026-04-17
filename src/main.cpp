#include "core/App.h"
#include "utils/logging.h"
#include <stdexcept>

int main(int argc, char* argv[]) {
#ifdef NDEBUG
  logging::startlogging("", "run.log");
#endif

  int exitcode = EXIT_SUCCESS;

  App app;
  try {
    app.init(1980, 1080, "Vulkan Renderer");
    while (!app.shouldClose()) {
      app.beginDrawing();
      app.endDrawing();
    }
  } catch (const std::exception &e) {
    logging::logerror("MAIN %s", e.what());
    exitcode = EXIT_FAILURE;
  }

  logging::stoplogging();
  return exitcode;
}

#ifdef _MSC_VER
#ifdef NDEBUG
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  return main(__argc, __argv);
}
#endif
#endif
