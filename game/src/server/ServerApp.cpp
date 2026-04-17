#include "pch.h"

#include "ClientRegistry.h"
#include "ServerApp.h"

#include "core/Assert.h"
#include "core/NetworkProtocol.h"

using server::ClientConnection;
using server::ClientRegistry;
using server::InvalidClientId;

namespace
{
    constexpr float TickSeconds = 1.f / 60.f;
    sf::Vector2f clampPosition(const sf::Vector2f& position)
    {
        return {
            std::clamp(position.x, net::PlayerRadius, static_cast<float>(net::WindowWidth) - net::PlayerRadius),
            std::clamp(position.y, net::PlayerRadius, static_cast<float>(net::WindowHeight) - net::PlayerRadius)
        };
    }

    sf::Packet makeWelcomePacket(const std::uint32_t clientId, const sf::Vector2f& position)
    {
        sf::Packet packet;
        net::writeServerMessageType(packet, net::ServerMessageType::Welcome);
        packet << clientId;
        net::writeVector2f(packet, position);
        return packet;
    }

    sf::Packet makeSnapshotPacket(const std::vector<ClientConnection>& clients)
    {
        sf::Packet packet;
        net::writeServerMessageType(packet, net::ServerMessageType::WorldSnapshot);
        packet << static_cast<std::uint32_t>(clients.size());

        for (const ClientConnection& client : clients)
        {
            net::writePlayerSnapshot(packet, net::PlayerSnapshot{client.id, client.position});
        }

        return packet;
    }
}

struct ServerApp::Impl
{
    sf::TcpListener tcpListener;
    sf::UdpSocket udpSocket;
    sf::SocketSelector selector;
    ClientRegistry clients;
    std::atomic_bool running = false;
    bool networkingInitialized = false;
    std::string lastError;
};

ServerApp::ServerApp()
    : impl(std::make_unique<Impl>())
{
}

ServerApp::~ServerApp() = default;

bool ServerApp::start()
{
    if (!initializeNetworking())
    {
        return false;
    }

    impl->running.store(true);
    std::cout << "Server started successfully." << '\n';
    return true;
}

void ServerApp::run()
{
    if (!impl->running.load() && !start())
    {
        return;
    }

    std::cout << "Server loop running." << '\n';

    sf::Clock tickClock;

    while (isRunning())
    {
        const float deltaTime = tickClock.restart().asSeconds();

        processNetwork();
        updateGameLogic(deltaTime);

        sf::sleep(sf::seconds(TickSeconds));
    }
}

bool ServerApp::isRunning() const
{
    return impl && impl->running.load();
}

const char* ServerApp::getLastError() const
{
    if (!impl)
    {
        return "";
    }

    return impl->lastError.c_str();
}

void ServerApp::stop()
{
    if (!impl)
    {
        return;
    }

    impl->running.store(false);
}

bool ServerApp::initializeNetworking()
{
    if (impl->networkingInitialized)
    {
        return true;
    }

    impl->tcpListener.close();
    impl->udpSocket.unbind();
    impl->selector.clear();
    impl->clients.clear();
    impl->lastError.clear();

    if (impl->tcpListener.listen(net::TcpPort) != sf::Socket::Status::Done)
    {
        impl->lastError = "Failed to bind TCP listener.";
        return false;
    }

    if (impl->udpSocket.bind(net::UdpPort) != sf::Socket::Status::Done)
    {
        impl->tcpListener.close();
        impl->lastError = "Failed to bind UDP socket.";
        return false;
    }

    impl->tcpListener.setBlocking(false);
    impl->udpSocket.setBlocking(false);

    impl->selector.add(impl->tcpListener);
    impl->selector.add(impl->udpSocket);
    impl->networkingInitialized = true;

    std::cout << "TCP listener bound to port " << net::TcpPort << '\n';
    std::cout << "UDP socket bound to port " << net::UdpPort << '\n';
    return true;
}

void ServerApp::processNetwork()
{
    if (!impl->selector.wait(sf::microseconds(1)))
    {
        return;
    }

    if (impl->selector.isReady(impl->tcpListener))
    {
        handlePendingTcpConnections();
    }

    if (impl->selector.isReady(impl->udpSocket))
    {
        handlePendingUdpPackets();
    }

    handleConnectedTcpClients();
}

void ServerApp::handlePendingTcpConnections()
{
    while (true)
    {
        auto socket = std::make_unique<sf::TcpSocket>();
        AssertBreakUnless(socket, "Failed to allocate a TCP socket for a new client.");
        socket->setBlocking(false);

        const sf::Socket::Status acceptStatus = impl->tcpListener.accept(*socket);
        if (acceptStatus == sf::Socket::Status::NotReady)
        {
            break;
        }

        if (acceptStatus != sf::Socket::Status::Done)
        {
            break;
        }

        ClientConnection client;
        client.id = impl->clients.allocateClientId();
        client.label = "client_" + std::to_string(client.id);
        client.tcpSocket = std::move(socket);
        client.position = {
            static_cast<float>(net::WindowWidth) * 0.5f,
            static_cast<float>(net::WindowHeight) * 0.5f
        };
        client.position = clampPosition({
            static_cast<float>(net::WindowWidth) * 0.5f + ((client.id % 5) * 70.f) - 140.f,
            static_cast<float>(net::WindowHeight) * 0.5f
        });

        impl->selector.add(*client.tcpSocket);

        sf::TcpSocket& tcpSocket = *client.tcpSocket;
        const std::uint32_t welcomeClientId = client.id;
        const sf::Vector2f welcomePosition = client.position;
        const std::optional<sf::IpAddress> remoteAddress = tcpSocket.getRemoteAddress();
        const unsigned short remotePort = tcpSocket.getRemotePort();
        impl->clients.add(std::move(client));

        std::cout << "Accepted TCP client " << welcomeClientId;
        if (remoteAddress.has_value())
        {
            std::cout << " from " << *remoteAddress << ':' << remotePort;
        }
        std::cout << '\n';

        sf::Packet welcomePacket = makeWelcomePacket(welcomeClientId, welcomePosition);
        [[maybe_unused]] const sf::Socket::Status sendStatus = tcpSocket.send(welcomePacket);
    }
}

void ServerApp::handlePendingUdpPackets()
{
    while (true)
    {
        sf::Packet packet;
        std::optional<sf::IpAddress> remoteAddress;
        unsigned short remotePort = 0;

        const sf::Socket::Status status = impl->udpSocket.receive(packet, remoteAddress, remotePort);
        if (status == sf::Socket::Status::NotReady)
        {
            break;
        }

        if (status != sf::Socket::Status::Done)
        {
            break;
        }

        net::ClientMessageType messageType{};
        std::uint32_t clientId = InvalidClientId;
        sf::Vector2f movementInput;

        if (!net::readClientMessageType(packet, messageType))
        {
            continue;
        }

        packet >> clientId;
        if (!packet || !net::readVector2f(packet, movementInput))
        {
            continue;
        }

        if (messageType != net::ClientMessageType::MovementInput)
        {
            continue;
        }

        ClientConnection* client = impl->clients.find(clientId);
        if (!client)
        {
            continue;
        }

        client->udpAddress = remoteAddress;
        client->udpPort = remotePort;
        client->movementInput = movementInput;

        if (client->movementInput.lengthSquared() > 1.f)
        {
            client->movementInput = client->movementInput.normalized();
        }
    }
}

void ServerApp::handleConnectedTcpClients()
{
    for (std::size_t clientIndex = 0; clientIndex < impl->clients.values().size();)
    {
        ClientConnection& client = impl->clients.values()[clientIndex];
        sf::TcpSocket& tcpSocket = *client.tcpSocket;

        if (!impl->selector.isReady(tcpSocket))
        {
            ++clientIndex;
            continue;
        }

        sf::Packet packet;
        const sf::Socket::Status status = tcpSocket.receive(packet);

        if (status == sf::Socket::Status::Disconnected)
        {
            const std::uint32_t disconnectedClientId = client.id;
            impl->selector.remove(tcpSocket);
            impl->clients.removeAt(clientIndex);
            std::cout << "Client " << disconnectedClientId << " disconnected." << '\n';
            continue;
        }

        if (status == sf::Socket::Status::NotReady)
        {
            ++clientIndex;
            continue;
        }

        if (status != sf::Socket::Status::Done)
        {
            const std::uint32_t erroredClientId = client.id;
            impl->selector.remove(tcpSocket);
            impl->clients.removeAt(clientIndex);
            std::cout << "Client " << erroredClientId << " TCP socket errored and was removed." << '\n';
            continue;
        }

        ++clientIndex;
    }
}

void ServerApp::updateGameLogic(float deltaTime)
{
    for (ClientConnection& client : impl->clients.values())
    {
        sf::Vector2f movement = client.movementInput;
        if (movement.lengthSquared() > 1.f)
        {
            movement = movement.normalized();
        }

        client.position += movement * (net::PlayerSpeed * deltaTime);
        client.position = clampPosition(client.position);
    }

    sf::Packet snapshotPacket = makeSnapshotPacket(impl->clients.values());
    for (ClientConnection& client : impl->clients.values())
    {
        if (!client.udpAddress.has_value())
        {
            continue;
        }

        [[maybe_unused]] const sf::Socket::Status sendStatus =
            impl->udpSocket.send(snapshotPacket, *client.udpAddress, client.udpPort);
    }
}
