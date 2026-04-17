#include "pch.h"

#include "Game.h"

#include "core/NetworkProtocol.h"

namespace
{
    constexpr sf::Time ConnectTimeout = sf::seconds(2.f);
    constexpr float IgnoreCorrectionDistance = 2.f;
    constexpr float SnapCorrectionDistance = 40.f;
    constexpr float ReconciliationBlendFactor = 0.35f;

    constexpr std::size_t toIndex(const InputAction action)
    {
        return static_cast<std::size_t>(action);
    }
}

Game::Game(sf::RenderWindow& window, sf::IpAddress serverAddress)
    : window(window)
    , serverAddress(serverAddress)
{
    localPlayer.setColor(sf::Color::Green);

    actionBindings[toIndex(InputAction::MoveUp)] = sf::Keyboard::Scancode::W;
    actionBindings[toIndex(InputAction::MoveDown)] = sf::Keyboard::Scancode::S;
    actionBindings[toIndex(InputAction::MoveLeft)] = sf::Keyboard::Scancode::A;
    actionBindings[toIndex(InputAction::MoveRight)] = sf::Keyboard::Scancode::D;

    inputListeners.push_back(&localPlayer);
}

bool Game::start()
{
    running = true;
    windowFocused = window.hasFocus();
    connectedToServer = connectToServer();
    return true;
}

void Game::stop()
{
    running = false;
    tcpSocket.disconnect();
    udpSocket.unbind();
}

void Game::setWindowFocused(const bool focused)
{
    windowFocused = focused;

    if (windowFocused)
    {
        return;
    }

    currentActionStates.fill(false);
    dispatchInputChanges();
}

void Game::pollInputs()
{
    if (!running)
    {
        return;
    }

    for (std::size_t index = 0; index < actionBindings.size(); ++index)
    {
        currentActionStates[index] = windowFocused && sf::Keyboard::isKeyPressed(actionBindings[index]);
    }

    dispatchInputChanges();
}

void Game::update(const float deltaTime)
{
    if (!running)
    {
        return;
    }

    processTcpMessages();
    localPlayer.update(deltaTime);
    clampPlayerToScreen(localPlayer);
    sendMovementUpdate();
    processUdpMessages();
}

void Game::render()
{
    if (!running)
    {
        return;
    }

    window.clear(sf::Color(24, 28, 36));

    for (const auto& [clientId, remotePlayer] : remotePlayers)
    {
        static_cast<void>(clientId);
        remotePlayer.render(window);
    }

    localPlayer.render(window);
    window.display();
}

bool Game::connectToServer()
{
    if (udpSocket.bind(sf::Socket::AnyPort) != sf::Socket::Status::Done)
    {
        std::cerr << "Client failed to bind UDP socket." << '\n';
        return false;
    }

    udpSocket.setBlocking(false);

    const sf::Socket::Status connectStatus = tcpSocket.connect(serverAddress, net::TcpPort, ConnectTimeout);
    if (connectStatus != sf::Socket::Status::Done)
    {
        std::cerr << "Client failed to connect to " << serverAddress << ':' << net::TcpPort << '\n';
        tcpSocket.disconnect();
        udpSocket.unbind();
        return false;
    }

    tcpSocket.setBlocking(false);

    std::cout << "Client connected to " << serverAddress << " on TCP " << net::TcpPort
              << " and UDP " << net::UdpPort << '\n';
    return true;
}

void Game::processTcpMessages()
{
    if (!connectedToServer)
    {
        return;
    }

    while (true)
    {
        sf::Packet packet;
        const sf::Socket::Status status = tcpSocket.receive(packet);
        if (status == sf::Socket::Status::NotReady)
        {
            break;
        }

        if (status == sf::Socket::Status::Disconnected)
        {
            connectedToServer = false;
            std::cout << "Client disconnected from server TCP socket." << '\n';
            break;
        }

        if (status != sf::Socket::Status::Done)
        {
            break;
        }

        net::ServerMessageType messageType{};
        if (!net::readServerMessageType(packet, messageType))
        {
            continue;
        }

        if (messageType == net::ServerMessageType::Welcome)
        {
            sf::Vector2f spawnPosition;
            packet >> localClientId;
            if (!packet || !net::readVector2f(packet, spawnPosition))
            {
                continue;
            }
            localPlayer.setPosition(spawnPosition);

            std::cout << "Client received id " << localClientId << '\n';
        }
    }
}

void Game::processUdpMessages()
{
    if (!connectedToServer)
    {
        return;
    }

    while (true)
    {
        sf::Packet packet;
        std::optional<sf::IpAddress> remoteAddress;
        unsigned short remotePort = 0;

        const sf::Socket::Status status = udpSocket.receive(packet, remoteAddress, remotePort);
        if (status == sf::Socket::Status::NotReady)
        {
            break;
        }

        if (status != sf::Socket::Status::Done)
        {
            break;
        }

        net::ServerMessageType messageType{};
        if (!net::readServerMessageType(packet, messageType))
        {
            continue;
        }

        if (messageType != net::ServerMessageType::WorldSnapshot)
        {
            continue;
        }

        std::uint32_t snapshotCount = 0;
        packet >> snapshotCount;

        std::unordered_map<std::uint32_t, Player> nextRemotePlayers;

        for (std::uint32_t index = 0; index < snapshotCount; ++index)
        {
            net::PlayerSnapshot snapshot;
            if (!net::readPlayerSnapshot(packet, snapshot))
            {
                break;
            }

            if (snapshot.clientId == localClientId)
            {
                reconcileLocalPlayer(snapshot.position);
                continue;
            }

            Player remotePlayer;
            const auto existingIt = remotePlayers.find(snapshot.clientId);
            if (existingIt != remotePlayers.end())
            {
                remotePlayer = existingIt->second;
            }

            remotePlayer.setColor(sf::Color(90, 170, 255));
            remotePlayer.setPosition(snapshot.position);
            nextRemotePlayers.emplace(snapshot.clientId, remotePlayer);
        }

        remotePlayers = std::move(nextRemotePlayers);
    }
}

void Game::reconcileLocalPlayer(const sf::Vector2f& authoritativePosition)
{
    const sf::Vector2f currentPosition = localPlayer.getPosition();
    const sf::Vector2f error = authoritativePosition - currentPosition;
    const float errorDistanceSquared = error.lengthSquared();

    if (errorDistanceSquared <= IgnoreCorrectionDistance * IgnoreCorrectionDistance)
    {
        return;
    }

    if (errorDistanceSquared >= SnapCorrectionDistance * SnapCorrectionDistance)
    {
        localPlayer.setPosition(authoritativePosition);
        return;
    }

    localPlayer.setPosition(currentPosition + (error * ReconciliationBlendFactor));
}

void Game::dispatchInputChanges()
{
    for (std::size_t index = 0; index < currentActionStates.size(); ++index)
    {
        if (currentActionStates[index] == previousActionStates[index])
        {
            continue;
        }

        const auto action = static_cast<InputAction>(index);
        for (InputListener* listener : inputListeners)
        {
            listener->onInputAction(action, currentActionStates[index]);
        }

        previousActionStates[index] = currentActionStates[index];
    }
}

void Game::sendMovementUpdate()
{
    if (!connectedToServer || localClientId == 0)
    {
        return;
    }

    sf::Vector2f movement = localPlayer.getMovementIntent();
    if (movement.lengthSquared() > 0.f)
    {
        movement = movement.normalized();
    }

    sf::Packet packet;
    net::writeClientMessageType(packet, net::ClientMessageType::MovementInput);
    packet << localClientId;
    net::writeVector2f(packet, movement);

    [[maybe_unused]] const sf::Socket::Status sendStatus =
        udpSocket.send(packet, serverAddress, net::UdpPort);
}

void Game::clampPlayerToScreen(Player& player) const
{
    sf::Vector2f position = player.getPosition();

    position.x = std::clamp(
        position.x,
        net::PlayerRadius,
        static_cast<float>(window.getSize().x) - net::PlayerRadius
    );
    position.y = std::clamp(
        position.y,
        net::PlayerRadius,
        static_cast<float>(window.getSize().y) - net::PlayerRadius
    );

    player.setPosition(position);
}
