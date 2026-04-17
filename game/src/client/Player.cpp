#include "pch.h"

#include "Player.h"

#include "core/NetworkProtocol.h"

namespace
{
    constexpr std::size_t toIndex(const InputAction action)
    {
        return static_cast<std::size_t>(action);
    }
}

Player::Player()
    : shape(net::PlayerRadius)
{
    shape.setOrigin({net::PlayerRadius, net::PlayerRadius});
    shape.setFillColor(sf::Color::Green);
    shape.setPosition({
        static_cast<float>(net::WindowWidth) * 0.5f,
        static_cast<float>(net::WindowHeight) * 0.5f
    });
}

void Player::onInputAction(const InputAction action, const bool active)
{
    actionStates[toIndex(action)] = active;
}

void Player::update(const float deltaTime)
{
    sf::Vector2f direction = getMovementIntent();
    if (direction.lengthSquared() > 0.f)
    {
        direction = direction.normalized();
    }

    shape.move(direction * (net::PlayerSpeed * deltaTime));
}

void Player::render(sf::RenderWindow& window) const
{
    window.draw(shape);
}

void Player::setColor(const sf::Color& color)
{
    shape.setFillColor(color);
}

void Player::setPosition(const sf::Vector2f& position)
{
    shape.setPosition(position);
}

sf::Vector2f Player::getPosition() const
{
    return shape.getPosition();
}

sf::Vector2f Player::getMovementIntent() const
{
    sf::Vector2f direction;

    if (actionStates[toIndex(InputAction::MoveUp)])
    {
        direction.y -= 1.f;
    }

    if (actionStates[toIndex(InputAction::MoveDown)])
    {
        direction.y += 1.f;
    }

    if (actionStates[toIndex(InputAction::MoveLeft)])
    {
        direction.x -= 1.f;
    }

    if (actionStates[toIndex(InputAction::MoveRight)])
    {
        direction.x += 1.f;
    }

    return direction;
}
