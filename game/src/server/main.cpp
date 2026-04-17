#include "ServerApp.h"

int main()
{
    ServerApp app;
    if (!app.start())
    {
        std::cerr << "Server failed to start: " << app.getLastError() << '\n';
        return 1;
    }

    app.run();
    return 0;
}
