#include "../include/managers/ChannelSettingsManager.hpp"

ChannelSettingsManager::ChannelSettingsManager(ChannelRegistry& channelRegistry)
    : channelRegistry(channelRegistry) {}

ChannelSettingsManager::~ChannelSettingsManager() {}

void ChannelSettingsManager::setInviteOnly(const std::string& channelName, bool inviteOnly) {
    channelRegistry.findChannel(channelName).setInviteOnly(inviteOnly);
}

void ChannelSettingsManager::setTopicProtected(const std::string& channelName,
                                               bool topicProtected) {
    channelRegistry.findChannel(channelName).setTopicProtected(topicProtected);
}

void ChannelSettingsManager::setLimit(const std::string& channelName, int limit) {
    channelRegistry.findChannel(channelName).setLimit(limit);
}

void ChannelSettingsManager::setKey(const std::string& channelName, const std::string& key) {
    channelRegistry.findChannel(channelName).setKey(key);
}

void ChannelSettingsManager::setTopic(const std::string& channelName, const std::string& topic) {
    channelRegistry.findChannel(channelName).setTopic(topic);
}