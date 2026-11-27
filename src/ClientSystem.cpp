#include "../include/managers/ClientSystem.hpp"


// ---------------------------------------------------------------------------
// ClientDetails
// ---------------------------------------------------------------------------

ClientSystem::ClientDetails::ClientDetails(int fd, std::string nick, std::string username,
                                           std::string realname, std::string hostname,
                                           bool authenticated, bool registered, bool quitting,
                                           std::set<std::string> channels)
    : fd(fd), nick(nick), username(username), realname(realname), hostname(hostname),
      authenticated(authenticated), registered(registered), quitting(quitting),
      channels(channels) {}

bool ClientSystem::ClientDetails::operator==(const ClientDetails& other) const {
    return fd == other.fd && nick == other.nick && username == other.username &&
           realname == other.realname && hostname == other.hostname &&
           authenticated == other.authenticated && registered == other.registered &&
           quitting == other.quitting && channels == other.channels;
}

bool ClientSystem::ClientDetails::operator!=(const ClientDetails& other) const {
    return !(*this == other);
}

// ---------------------------------------------------------------------------
// ClientSystem
// ---------------------------------------------------------------------------

ClientSystem::ClientSystem(ClientLifecycleManager& lifecycleManager,
                           PendingIOManager& pendingIOManager, AuthenticationManager& authManager,
                           RegistrationManager& registrationManager,
                           ClientRegistry& clientRegistry)
    : lifecycleManager(lifecycleManager), pendingIOManager(pendingIOManager),
      authManager(authManager), registrationManager(registrationManager),
      clientRegistry(clientRegistry) {}

ClientSystem::~ClientSystem() {}

// Queries on sets of clients

std::set<int> ClientSystem::getClientsToClose() const {
    return lifecycleManager.getClientsToClose();
}

std::set<int> ClientSystem::getHasPendingInput() const {
    return pendingIOManager.getHasPendingInput();
}

std::set<int> ClientSystem::getHasPendingOutput() const {
    return pendingIOManager.getHasPendingOutput();
}

// Lifecycle

void ClientSystem::addClient(int fd, const std::string& hostname) {
    lifecycleManager.addClient(fd, hostname);
}

void ClientSystem::removeClient(int fd) {
    lifecycleManager.removeClient(fd);
}

void ClientSystem::setIsQuitting(int fd) {
    lifecycleManager.setIsQuitting(fd);
}

// Pending I/O

void ClientSystem::pushInput(int fd, const std::string& data) {
    pendingIOManager.pushInput(fd, data);
}

bool ClientSystem::getNextInputLine(int fd, std::string& line) {
    return pendingIOManager.getNextInputLine(fd, line);
}

void ClientSystem::pushOutput(int fd, const std::string& data) {
    pendingIOManager.pushOutput(fd, data);
}

std::string ClientSystem::peekOutput(int fd, std::size_t length) const {
    return pendingIOManager.peekOutput(fd, length);
}

void ClientSystem::consumeOutput(int fd, std::size_t length) {
    pendingIOManager.consumeOutput(fd, length);
}

// Authentication

void ClientSystem::authenticate(int fd, const std::string& password) {
    authManager.authenticate(fd, password);
}

bool ClientSystem::isAuthenticated(int fd) const {
    return authManager.isAuthenticated(fd);
}

// Registration

void ClientSystem::setNick(int fd, const std::string& nick) {
    registrationManager.setNick(fd, nick);
}

void ClientSystem::setUsername(int fd, const std::string& username) {
    registrationManager.setUsername(fd, username);
}

void ClientSystem::setRealname(int fd, const std::string& realname) {
    registrationManager.setRealname(fd, realname);
}

bool ClientSystem::isRegistered(int fd) const {
    return registrationManager.isRegistered(fd);
}

int ClientSystem::getFdByNick(const std::string& nick) const {
    return registrationManager.getFdByNick(nick);
}

// Client details

ClientSystem::ClientDetails ClientSystem::getClientDetails(int fd) const {
    const Client& client = clientRegistry.findClient(fd);
    return ClientDetails(client.getFd(), client.getNick(), client.getUsername(),
                         client.getRealname(), client.getHostname(), client.isAuthenticated(),
                         registrationManager.isRegistered(fd), client.isQuitting(), client.getChannels());
}