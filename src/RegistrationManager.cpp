#include "../include/managers/RegistrationManager.hpp"

RegistrationManager::RegistrationManager(ClientRegistry& clientRegistry, NickManager& nickManager)
    : clientRegistry(clientRegistry), nickManager(nickManager) {}

RegistrationManager::~RegistrationManager() {}

void RegistrationManager::setNick(int fd, const std::string& nick) {
    Client& client = clientRegistry.findClient(fd);

    if (!nickManager.isValid(nick))
        throw InvalidNicknameException(nick);

    const std::string oldNick = client.getNick();

    if (oldNick == nick)
        return;

    // The new nick is only equivalent to the client's own nick (e.g. "Alice" -> "alice").
    // Reserving it first would fail, so the old reservation is replaced instead. The reserve
    // cannot fail here: the nick is valid and has just been released by its own owner.
    if (!oldNick.empty() && !nickManager.isAvailable(nick) && nickManager.getFd(nick) == fd) {
        nickManager.release(oldNick);
        nickManager.reserve(nick, fd);
        client.setNick(nick);
        return;
    }

    // Reserve the new nick before releasing the old one, so a failure never leaves the client
    // without a nick.
    if (!nickManager.reserve(nick, fd))
        throw NicknameAlreadyInUseException(nick);

    client.setNick(nick);

    if (!oldNick.empty())
        nickManager.release(oldNick);
}

void RegistrationManager::setUsername(int fd, const std::string& username) {
    clientRegistry.findClient(fd).setUsername(username);
}

void RegistrationManager::setRealname(int fd, const std::string& realname) {
    clientRegistry.findClient(fd).setRealname(realname);
}

void RegistrationManager::setHostname(int fd, const std::string& hostname) {
    clientRegistry.findClient(fd).setHostname(hostname);
}

bool RegistrationManager::isRegistered(int fd) const {
    const Client& client = clientRegistry.findClient(fd);

    return !client.getNick().empty() && !client.getUsername().empty() &&
           !client.getRealname().empty();
}

void RegistrationManager::unRegister(int fd) {
    Client& client = clientRegistry.findClient(fd);

    if (!client.getNick().empty())
        nickManager.release(client.getNick());

    const std::string empty;
    client.setNick(empty);
    client.setUsername(empty);
    client.setRealname(empty);
    client.setHostname(empty);
}

int RegistrationManager::getFdByNick(const std::string& nick) const {
    return nickManager.getFd(nick);
}