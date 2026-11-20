
#include "../include/managers/AuthenticationManager.hpp"

AuthenticationManager::AuthenticationManager(ClientRegistry& clientRegistry,
                                             const std::string& password

                                             )
    : clientRegistry(clientRegistry), password(password)

{}

AuthenticationManager::~AuthenticationManager() {}

void AuthenticationManager::authenticate(int fd, const std::string& password) {
    Client& client = clientRegistry.findClient(fd);

    if (password != this->password)
        throw AuthenticationFailedException();

    client.setAuthenticated(true);
}

void AuthenticationManager::unAuthenticate(int fd) {
    Client& client = clientRegistry.findClient(fd);
    client.setAuthenticated(false);
}

bool AuthenticationManager::isAuthenticated(int fd) const {
    const Client& client = clientRegistry.findClient(fd);
    return client.isAuthenticated();
}
