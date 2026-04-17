#include "pch.h"

#include "ClientApp.h"

#include "core/NetworkProtocol.h"

ClientApp::ClientApp() = default;

void ClientApp::run()
{
    if (!load())
    {
        return;
    }

    sf::Clock frameClock;

	while (running && window && window->isOpen())
	{
        const float deltaTime = frameClock.restart().asSeconds();

        processEvents();
        update(deltaTime);
        render();
	}

    unload();
}

void ClientApp::stop()
{
    running = false;

    if (window)
    {
        window->close();
    }
}

void ClientApp::processEvents()
{
    while (const std::optional event = window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            stop();
        }
        else if (event->is<sf::Event::FocusLost>())
        {
            game->setWindowFocused(false);
        }
        else if (event->is<sf::Event::FocusGained>())
        {
            game->setWindowFocused(true);
        }
    }
}

void ClientApp::update(float deltaTime)
{
    game->pollInputs();
    game->update(deltaTime);
}

void ClientApp::render()
{
    game->render();
}

bool ClientApp::load()
{
    std::cout << "Enter server IP (leave empty for localhost): ";

    std::string serverIpInput;
    std::getline(std::cin, serverIpInput);

    sf::IpAddress serverAddress = sf::IpAddress::LocalHost;
    if (!serverIpInput.empty())
    {
        const std::optional<sf::IpAddress> resolvedAddress = sf::IpAddress::resolve(serverIpInput);
        if (!resolvedAddress.has_value())
        {
            std::cerr << "Client failed to resolve server IP: " << serverIpInput << '\n';
            return false;
        }

        serverAddress = *resolvedAddress;
    }

    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({net::WindowWidth, net::WindowHeight}),
        "skullshooter client"
    );
    window->setFramerateLimit(60);

    game = std::make_unique<Game>(*window, serverAddress);
    running = game->start();
    return running;
}

void ClientApp::unload()
{
    if (game)
    {
        game->stop();
        game.reset();
    }

    if (window)
    {
        window.reset();
    }
}
