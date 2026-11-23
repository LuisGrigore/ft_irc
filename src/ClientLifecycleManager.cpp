#include "../include/managers/ClientLifecycleManager.hpp"
#include <algorithm>

ClientLifecycleManager::ClientLifecycleManager(ClientRegistry& clientRegistry,
                                               AuthenticationManager& authManager,
                                               RegistrationManager& registrationManager,
                                               ChannelMembershipManager& channelMembershipManager,
                                               PendingIOManager& pendingIOManager)
    : clientRegistry(clientRegistry), authManager(authManager),
      registrationManager(registrationManager), channelMembershipManager(channelMembershipManager),
      pendingIOManager(pendingIOManager) {}

ClientLifecycleManager::~ClientLifecycleManager() {}

void ClientLifecycleManager::addClient(int fd, const std::string& hostname) {
    clientRegistry.addClient(fd);
    registrationManager.setHostname(fd, hostname);
}

void ClientLifecycleManager::removeClient(int fd) {
    authManager.unAuthenticate(fd);
    registrationManager.unRegister(fd);
    channelMembershipManager.removeClientFromAllChannels(fd);
    isQuitting.erase(fd);
    pendingIOManager.clearInput(fd);
    pendingIOManager.clearOutput(fd);
    clientRegistry.removeClient(fd);
}

void ClientLifecycleManager::setIsQuitting(int fd) {
    clientRegistry.findClient(fd).setQuitting(true);
    pendingIOManager.clearInput(fd);
    isQuitting.insert(fd);
}

std::set<int> ClientLifecycleManager::getClientsToClose() const {
    std::set<int> toClose;

    const std::set<int> pendingOutput = pendingIOManager.getHasPendingOutput();

    std::set_difference(isQuitting.begin(), isQuitting.end(), pendingOutput.begin(),
                        pendingOutput.end(), std::inserter(toClose, toClose.begin()));

    return toClose;
}
