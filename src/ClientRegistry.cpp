#include "../include/client/ClientRegistry.hpp"

ClientRegistry::ClientRegistry() {}

ClientRegistry::~ClientRegistry() {
    for (std::map<int, Client*>::iterator it = clientsByFd.begin(); it != clientsByFd.end();
         ++it) {
        delete it->second;
    }
    clientsByFd.clear();
}

void ClientRegistry::addClient(int fd) {
    if (clientsByFd.find(fd) != clientsByFd.end())
        throw ClientAlreadyExistsException(fd);

    Client* client = new Client(fd);
    try {
        clientsByFd.insert(std::make_pair(fd, client));
    } catch (...) {
        delete client;
        throw;
    }
}

void ClientRegistry::removeClient(int fd) {
    std::map<int, Client*>::iterator it = clientsByFd.find(fd);
    if (it == clientsByFd.end())
        throw ClientNotFoundException(fd);
    delete it->second;
    clientsByFd.erase(it);
}

Client& ClientRegistry::findClient(int fd) {
    std::map<int, Client*>::iterator it = clientsByFd.find(fd);
    if (it != clientsByFd.end())
        return *it->second;
    throw ClientNotFoundException(fd);
}

const Client& ClientRegistry::findClient(int fd) const {
    std::map<int, Client*>::const_iterator it = clientsByFd.find(fd);
    if (it != clientsByFd.end())
        return *it->second;
    throw ClientNotFoundException(fd);
}