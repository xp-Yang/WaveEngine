#include "EngineAPI.hpp"

int WinMain()
{
    auto& app = GetApp();
    app.init();
    app.run();
    app.shutdown();
    return 0;
}