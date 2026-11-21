#ifndef CHANNEL_SETTINGS_MANAGER_HPP
#define CHANNEL_SETTINGS_MANAGER_HPP

#include <string>

#include "../channel/ChannelRegistry.hpp"

/**
 * @brief Updates the configuration of existing channels: invite-only, topic-protected, user
 * limit, key and topic.
 *
 * It only changes those settings. It never alters the members, operators or invitations of a
 * channel, and it does not check who is allowed to change a setting (that is the caller's
 * responsibility, e.g. the MODE and TOPIC commands).
 *
 * It does not own the ChannelRegistry and it never creates or removes channels. Channel names
 * are case-insensitive, as in ChannelRegistry.
 *
 * The manager is not copyable.
 */
class ChannelSettingsManager {
  private:
    ChannelRegistry& channelRegistry;

    ChannelSettingsManager(const ChannelSettingsManager&);
    ChannelSettingsManager& operator=(const ChannelSettingsManager&);

  public:
    /**
     * @brief Constructs a ChannelSettingsManager.
     * @param channelRegistry Registry containing the channels to configure.
     */
    ChannelSettingsManager(ChannelRegistry& channelRegistry);
    ~ChannelSettingsManager();

    /**
     * @brief Enables or disables the invite-only mode of a channel.
     * @param channelName The name of the channel.
     * @param inviteOnly true to enable, false to disable.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    void setInviteOnly(const std::string& channelName, bool inviteOnly);

    /**
     * @brief Enables or disables the topic-protected mode of a channel.
     * @param channelName The name of the channel.
     * @param topicProtected true to enable, false to disable.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    void setTopicProtected(const std::string& channelName, bool topicProtected);

    /**
     * @brief Sets the maximum number of members of a channel.
     * @param channelName The name of the channel.
     * @param limit The new limit. 0 removes the limit.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    void setLimit(const std::string& channelName, int limit);

    /**
     * @brief Sets the key (password) required to join a channel.
     * @param channelName The name of the channel.
     * @param key The new key. An empty key removes it.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    void setKey(const std::string& channelName, const std::string& key);

    /**
     * @brief Sets the topic of a channel.
     * @param channelName The name of the channel.
     * @param topic The new topic. An empty topic clears it.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    void setTopic(const std::string& channelName, const std::string& topic);
};

#endif // CHANNEL_SETTINGS_MANAGER_HPP