#include "pch.h"

#include "ClientRegistry.h"

namespace server
{
    void ClientRegistry::clear()
    {
        clients.clear();
        clientIndices.clear();
        nextClientId = 1;
    }

    std::uint32_t ClientRegistry::allocateClientId()
    {
        return nextClientId++;
    }

    void ClientRegistry::add(ClientConnection client)
    {
        clientIndices.emplace(client.id, clients.size());
        clients.push_back(std::move(client));
    }

    ClientConnection* ClientRegistry::find(const std::uint32_t clientId)
    {
        const auto clientIt = clientIndices.find(clientId);
        if (clientIt == clientIndices.end())
        {
            return nullptr;
        }

        return &clients[clientIt->second];
    }

    void ClientRegistry::removeAt(const std::size_t index)
    {
        const std::size_t lastIndex = clients.size() - 1;
        const std::uint32_t removedClientId = clients[index].id;

        if (index != lastIndex)
        {
            clients[index] = std::move(clients[lastIndex]);
            clientIndices[clients[index].id] = index;
        }

        clients.pop_back();
        clientIndices.erase(removedClientId);
    }

    std::vector<ClientConnection>& ClientRegistry::values()
    {
        return clients;
    }

    const std::vector<ClientConnection>& ClientRegistry::values() const
    {
        return clients;
    }
}
