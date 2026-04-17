#pragma once

#include "SFML/Network.hpp"
#include "SFML/System.hpp"

namespace net
{
    constexpr unsigned short TcpPort = 54000;
    constexpr unsigned short UdpPort = 54001;

    constexpr unsigned int WindowWidth = 1280;
    constexpr unsigned int WindowHeight = 720;

    constexpr float PlayerRadius = 24.f;
    constexpr float PlayerSpeed = 260.f;

    enum class ClientMessageType : std::uint8_t
    {
        MovementInput = 1
    };

    enum class ServerMessageType : std::uint8_t
    {
        Welcome = 1,
        WorldSnapshot = 2
    };

    struct PlayerSnapshot
    {
        std::uint32_t clientId = 0;
        sf::Vector2f position;
    };

    inline void writeClientMessageType(sf::Packet& packet, const ClientMessageType type)
    {
        packet << static_cast<std::uint8_t>(type);
    }

    inline bool readClientMessageType(sf::Packet& packet, ClientMessageType& type)
    {
        std::uint8_t rawValue = 0;
        packet >> rawValue;
        type = static_cast<ClientMessageType>(rawValue);
        return static_cast<bool>(packet);
    }

    inline void writeServerMessageType(sf::Packet& packet, const ServerMessageType type)
    {
        packet << static_cast<std::uint8_t>(type);
    }

    inline bool readServerMessageType(sf::Packet& packet, ServerMessageType& type)
    {
        std::uint8_t rawValue = 0;
        packet >> rawValue;
        type = static_cast<ServerMessageType>(rawValue);
        return static_cast<bool>(packet);
    }

    inline void writeVector2f(sf::Packet& packet, const sf::Vector2f& value)
    {
        packet << value.x << value.y;
    }

    inline bool readVector2f(sf::Packet& packet, sf::Vector2f& value)
    {
        packet >> value.x >> value.y;
        return static_cast<bool>(packet);
    }

    inline void writePlayerSnapshot(sf::Packet& packet, const PlayerSnapshot& snapshot)
    {
        packet << snapshot.clientId;
        writeVector2f(packet, snapshot.position);
    }

    inline bool readPlayerSnapshot(sf::Packet& packet, PlayerSnapshot& snapshot)
    {
        packet >> snapshot.clientId;
        if (!packet)
        {
            return false;
        }

        return readVector2f(packet, snapshot.position);
    }
}
