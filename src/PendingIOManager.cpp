#include "../include/managers/PendingIOManager.hpp"

PendingIOManager::PendingIOManager(ClientRegistry& clientRegistry)
    : clientRegistry(clientRegistry) {}

PendingIOManager::~PendingIOManager() {}

void PendingIOManager::pushInput(int fd, const std::string& data) {
    Client& client = clientRegistry.findClient(fd);

    if (client.isQuitting())
        return;

    client.pushInput(data);

    if (client.isInputBufferEmpty())
        hasPendingInput.erase(fd);
    else
        hasPendingInput.insert(fd);
}

void PendingIOManager::pushOutput(int fd, const std::string& data) {
    Client& client = clientRegistry.findClient(fd);
    client.pushOutput(data);

    if (client.isOutputBufferEmpty())
        hasPendingOutput.erase(fd);
    else
        hasPendingOutput.insert(fd);
}

bool PendingIOManager::getNextInputLine(int fd, std::string& line) {
    Client& client = clientRegistry.findClient(fd);
    const bool hasLine = client.getNextInputLine(line);

    if (client.isInputBufferEmpty())
        hasPendingInput.erase(fd);
    else
        hasPendingInput.insert(fd);

    return hasLine;
}

std::string PendingIOManager::peekOutput(int fd, size_t length) const {
    const Client& client = clientRegistry.findClient(fd);
    return client.peekOutput(length);
}

void PendingIOManager::consumeOutput(int fd, size_t length) {
    Client& client = clientRegistry.findClient(fd);
    const std::string available = client.peekOutput(length);

    client.consumeOutput(available.size());

    if (client.isOutputBufferEmpty())
        hasPendingOutput.erase(fd);
    else
        hasPendingOutput.insert(fd);
}

std::set<int> PendingIOManager::getHasPendingOutput() const { return hasPendingOutput; }

std::set<int> PendingIOManager::getHasPendingInput() const { return hasPendingInput; }

void PendingIOManager::clearInput(int fd) {
    clientRegistry.findClient(fd).clearInput();
    hasPendingInput.erase(fd);
}
void PendingIOManager::clearOutput(int fd) {
    clientRegistry.findClient(fd).clearOutput();
    hasPendingOutput.erase(fd);
}
