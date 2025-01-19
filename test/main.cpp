#include "EngineAPI.hpp"

int main()
{
    auto& app = Application::get();
    app.init();
    app.run();
    app.shutdown();
    return 0;
}

int WinMain()
{
    auto& app = Application::get();
    app.init();
    app.run();
    app.shutdown();
    return 0;
}