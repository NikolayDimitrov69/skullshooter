#pragma once

namespace server
{
    constexpr std::uint32_t InvalidClientId = 0;

    struct ClientConnection
    {
        std::uint32_t id = InvalidClientId;
        std::unique_ptr<sf::TcpSocket> tcpSocket;
        std::string label;
        std::optional<sf::IpAddress> udpAddress;
        unsigned short udpPort = 0;
        sf::Vector2f position;
        sf::Vector2f movementInput;
    };

    class ClientRegistry
    {
    public:
        void clear();
        [[nodiscard]] std::uint32_t allocateClientId();
        void add(ClientConnection client);
        [[nodiscard]] ClientConnection* find(std::uint32_t clientId);
        void removeAt(std::size_t index);

        [[nodiscard]] std::vector<ClientConnection>& values();
        [[nodiscard]] const std::vector<ClientConnection>& values() const;

    private:
        std::vector<ClientConnection> clients;
        std::unordered_map<std::uint32_t, std::size_t> clientIndices;
        std::uint32_t nextClientId = 1;
    };
}
