#include "pch.h"
#include "ClientApp.h"
#include "core/GameState.h"

void ClientApp::run()
{
    sf::RenderWindow window( sf::VideoMode( { 200, 200 } ), "skibidi67!" );
	sf::CircleShape shape( 100.f );
	shape.setFillColor( sf::Color::Green );

	while ( window.isOpen() )
	{
		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

		window.clear();
		window.draw( shape );
		window.display();
	}
}