#include "Engine.hpp"
#include "demo.hpp"

int main()
{
    auto& engine = Engine::get();
    engine.init();
    Demo* cubetest = Demo::createDemo("cubetest");
    cubetest->init();
    engine.run();
    engine.shutdown();
    return 0;
}

int WinMain()
{
    main();
}