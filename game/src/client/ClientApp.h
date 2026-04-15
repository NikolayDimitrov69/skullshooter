#pragma once

class ClientApp
{
public:
    void run();
	void stop();

private:
	void update(float deltaTime);
	void load();
	void unload();

private:
	std::unique_ptr<sf::RenderWindow> window;
};