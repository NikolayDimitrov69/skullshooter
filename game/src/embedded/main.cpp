#include "client/pch.h"

#include "server/ServerApp.h"
#include "client/ClientApp.h"

int main()
{
    ServerApp serverApp;
    std::thread serverThread(
        [&serverApp]()
        {
            if (!serverApp.start())
            {
                std::cerr << "Server thread failed to start: " << serverApp.getLastError() << '\n';
                return;
            }

            serverApp.run();
        }
    );

    ClientApp clientApp;
    clientApp.run();

    serverApp.stop();
    if (serverThread.joinable())
    {
        serverThread.join();
    }

    return 0;
}
