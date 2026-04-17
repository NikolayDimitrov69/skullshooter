#pragma once

enum class InputAction
{
    MoveUp = 0,
    MoveDown,
    MoveLeft,
    MoveRight,
    Count
};

class InputListener
{
public:
    virtual ~InputListener() = default;
    virtual void onInputAction(InputAction action, bool active) = 0;
};

class Player : public InputListener
{
public:
    Player();

    void onInputAction(InputAction action, bool active) override;

    void update(float deltaTime);
    void render(sf::RenderWindow& window) const;

    void setColor(const sf::Color& color);
    void setPosition(const sf::Vector2f& position);
    sf::Vector2f getPosition() const;
    sf::Vector2f getMovementIntent() const;

private:
    sf::CircleShape shape;
    std::array<bool, static_cast<std::size_t>(InputAction::Count)> actionStates{};
};
