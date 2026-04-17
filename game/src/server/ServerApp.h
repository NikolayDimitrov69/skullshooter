#pragma once

class ServerApp
{
public:
    ServerApp();
    ~ServerApp();

    bool start();
    void run();
    void stop();
    bool isRunning() const;
    const char* getLastError() const;

private:
    bool initializeNetworking();
    void processNetwork();
    void handlePendingTcpConnections();
    void handlePendingUdpPackets();
    void handleConnectedTcpClients();
    void updateGameLogic(float deltaTime);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
