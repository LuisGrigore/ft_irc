#include "../include/managers/ChannelMembershipManager.hpp"

ChannelMembershipManager::ChannelMembershipManager(ChannelRegistry& channelRegistry,
                                                   ClientRegistry& clientRegistry)
    : channelRegistry(channelRegistry), clientRegistry(clientRegistry) {}

ChannelMembershipManager::~ChannelMembershipManager() {}

void ChannelMembershipManager::createChannel(int fd, const std::string& channelName) {
    Client& client = clientRegistry.findClient(fd);

    // Throws if the name is invalid or the channel already exists, before touching any state.
    channelRegistry.addChannel(channelName);

    Channel& channel = channelRegistry.findChannel(channelName);
    channel.addMember(fd);
    channel.addOperator(fd);
    client.addChannel(channel.getName());
}

void ChannelMembershipManager::addMemberToChannel(int fd, const std::string& channelName) {
    Client& client = clientRegistry.findClient(fd);
    Channel& channel = channelRegistry.findChannel(channelName);

    if (channel.isMember(fd))
        throw AlreadyMemberException(fd, channelName);

    channel.addMember(fd);
    channel.consumeInvited(fd);
    client.addChannel(channel.getName());
}

void ChannelMembershipManager::removeClientFromChannel(int fd, const std::string& channelName) {
    Client& client = clientRegistry.findClient(fd);
    Channel& channel = channelRegistry.findChannel(channelName);

    if (!channel.isMember(fd))
        throw NotMemberException(fd, channelName);

    const std::string canonicalName = channel.getName();

    channel.removeMember(fd); // also drops operator status
    channel.consumeInvited(fd);
    client.removeChannel(canonicalName);

    if (channel.getMembers().empty())
        channelRegistry.removeChannel(canonicalName);
}

void ChannelMembershipManager::removeClientFromAllChannels(int fd) {
    Client& client = clientRegistry.findClient(fd);

    // Iterate over a copy: the client's own set is modified inside the loop.
    const std::set<std::string> joined = client.getChannel();

    for (std::set<std::string>::const_iterator it = joined.begin(); it != joined.end(); ++it) {
        try {
            Channel& channel = channelRegistry.findChannel(*it);
            channel.removeMember(fd);
            channel.consumeInvited(fd);
            if (channel.getMembers().empty())
                channelRegistry.removeChannel(*it);
        } catch (const ChannelRegistry::ChannelNotFoundException&) {
            // Stale entry in the client's set: nothing to clean on the channel side.
        }
        client.removeChannel(*it);
    }
}

bool ChannelMembershipManager::isMember(int fd, const std::string& channelName) const {
    clientRegistry.findClient(fd);
    return channelRegistry.findChannel(channelName).isMember(fd);
}

void ChannelMembershipManager::setOperator(int fd, const std::string& channelName,
                                           bool isOperator) {
    clientRegistry.findClient(fd);
    Channel& channel = channelRegistry.findChannel(channelName);

    if (!channel.isMember(fd))
        throw NotMemberException(fd, channelName);

    if (isOperator)
        channel.addOperator(fd);
    else
        channel.removeOperator(fd);
}

bool ChannelMembershipManager::isOperator(int fd, const std::string& channelName) const {
    clientRegistry.findClient(fd);
    return channelRegistry.findChannel(channelName).isOperator(fd);
}

void ChannelMembershipManager::addInviteToChannel(int fd, const std::string& channelName) {
    clientRegistry.findClient(fd);
    channelRegistry.findChannel(channelName).addInvited(fd);
}

void ChannelMembershipManager::consumeInviteToChannel(int fd, const std::string& channelName) {
    clientRegistry.findClient(fd);
    channelRegistry.findChannel(channelName).consumeInvited(fd);
}

bool ChannelMembershipManager::isInvited(int fd, const std::string& channelName) const {
    clientRegistry.findClient(fd);
    return channelRegistry.findChannel(channelName).isInvited(fd);
}