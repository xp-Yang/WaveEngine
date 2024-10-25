#include "EngineAPI.hpp"

int main()
{
    auto& app = GetApp();
    app.init();
    app.run();
    app.shutdown();
    return 0;
}

int WinMain()
{
    auto& app = GetApp();
    app.init();
    app.run();
    app.shutdown();
    return 0;
}