#pragma once

#include "Player.h"

class Game
{
public:
    Game(sf::RenderWindow& window, sf::IpAddress serverAddress);

    bool start();
    void stop();

    void setWindowFocused(bool focused);
    void pollInputs();
    void update(float deltaTime);
    void render();

private:
    bool connectToServer();
    void processTcpMessages();
    void processUdpMessages();
    void dispatchInputChanges();
    void reconcileLocalPlayer(const sf::Vector2f& authoritativePosition);
    void sendMovementUpdate();
    void clampPlayerToScreen(Player& player) const;

private:
    sf::RenderWindow& window;
    sf::IpAddress serverAddress;
    sf::TcpSocket tcpSocket;
    sf::UdpSocket udpSocket;

    Player localPlayer;
    std::unordered_map<std::uint32_t, Player> remotePlayers;
    std::vector<InputListener*> inputListeners;

    std::array<sf::Keyboard::Scancode, static_cast<std::size_t>(InputAction::Count)> actionBindings{};
    std::array<bool, static_cast<std::size_t>(InputAction::Count)> currentActionStates{};
    std::array<bool, static_cast<std::size_t>(InputAction::Count)> previousActionStates{};

    std::uint32_t localClientId = 0;
    bool running = false;
    bool connectedToServer = false;
    bool windowFocused = false;
};
