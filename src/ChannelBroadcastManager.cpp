#include "../include/managers/ChannelBroadcastManager.hpp"

ChannelBroadcastManager::ChannelBroadcastManager(ChannelRegistry& channelRegistry,
                                                 PendingIOManager& pendingIOManager)
    : channelRegistry(channelRegistry), pendingIOManager(pendingIOManager) {}

ChannelBroadcastManager::~ChannelBroadcastManager() {}

void ChannelBroadcastManager::broadcast(int senderFd, const std::string& channelName,
                                        const std::string& message) {

    std::set<int> members = channelRegistry.findChannel(channelName).getMembers();

    members.erase(senderFd);

    for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it) {
        pendingIOManager.pushOutput(*it, message);
    }
}
