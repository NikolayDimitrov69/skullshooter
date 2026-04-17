#pragma once

#include "Game.h"

class ClientApp
{
public:
    ClientApp();

    void run();
	void stop();

private:
    void processEvents();
	void update(float deltaTime);
	void render();
	bool load();
	void unload();

private:
	std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<Game> game;
    bool running = false;
};
