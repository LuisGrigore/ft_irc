#ifndef CHANNEL_MEMBERSHIP_MANAGER_HPP
#define CHANNEL_MEMBERSHIP_MANAGER_HPP

#include <cstddef>
#include <sstream>
#include <string>

#include "../channel/ChannelRegistry.hpp"
#include "../client/ClientRegistry.hpp"
#include "../exceptions_common/BaseException.hpp"

/**
 * @brief Manages the relationship between clients and channels: membership, operator status and
 * invitations.
 *
 * It keeps the Channel (set of fds) and the Client (set of channel names) sides consistent: every
 * membership change is applied to both, so a client is a member of a channel if and only if the
 * channel lists the client and the client lists the channel.
 *
 * It also applies the channel lifecycle: a channel is created by createChannel() and removed
 * automatically when its last member leaves.
 *
 * It does not own the ChannelRegistry or the ClientRegistry, and it does not create or remove
 * clients. Channel names are case-insensitive, as in ChannelRegistry.
 *
 * The manager is not copyable.
 */
class ChannelMembershipManager {
  private:
    ChannelRegistry& channelRegistry;
    ClientRegistry& clientRegistry;

    ChannelMembershipManager(const ChannelMembershipManager&);
    ChannelMembershipManager& operator=(const ChannelMembershipManager&);

  public:
    /**
     * @brief Thrown when a client tries to join a channel it already belongs to.
     */
    class AlreadyMemberException : public BaseException {
      private:
        static std::string buildMessage(int fd, const std::string& channelName) {
            std::ostringstream oss;
            oss << "Client with fd: \"" << fd << "\" is already a member of channel: \""
                << channelName << "\"";
            return oss.str();
        }

      public:
        AlreadyMemberException(int fd, const std::string& channelName,
                               const std::exception* cause = NULL,
                               const std::exception* overrides = NULL)
            : BaseException("AlreadyMemberException", buildMessage(fd, channelName), -1, cause,
                            overrides) {}
    };

    /**
     * @brief Thrown when an operation requires membership and the client is not a member of the
     * channel.
     */
    class NotMemberException : public BaseException {
      private:
        static std::string buildMessage(int fd, const std::string& channelName) {
            std::ostringstream oss;
            oss << "Client with fd: \"" << fd << "\" is not a member of channel: \"" << channelName
                << "\"";
            return oss.str();
        }

      public:
        NotMemberException(int fd, const std::string& channelName,
                           const std::exception* cause = NULL,
                           const std::exception* overrides = NULL)
            : BaseException("NotMemberException", buildMessage(fd, channelName), -1, cause,
                            overrides) {}
    };

    /**
     * @brief Constructs a ChannelMembershipManager.
     * @param channelRegistry Registry containing the existing channels.
     * @param clientRegistry Registry containing the existing clients.
     */
    ChannelMembershipManager(ChannelRegistry& channelRegistry, ClientRegistry& clientRegistry);
    ~ChannelMembershipManager();

    /**
     * @brief Creates a channel and makes the client its first member and operator.
     * @param fd The file descriptor of the creator.
     * @param channelName The name of the new channel.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws ChannelRegistry::InvalidChannelNameException if the name is invalid.
     * @throws ChannelRegistry::ChannelAlreadyExistsException if the channel already exists.
     */
    void createChannel(int fd, const std::string& channelName);

    /**
     * @brief Adds a client to an existing channel as an ordinary member.
     *
     * If the client had an invitation to the channel, it is consumed.
     *
     * @param fd The file descriptor of the client.
     * @param channelName The name of the channel.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     * @throws AlreadyMemberException if the client is already a member.
     */
    void addMemberToChannel(int fd, const std::string& channelName);

    /**
     * @brief Removes a client from a channel, including its operator status.
     *
     * If the client was the last member, the channel is removed from the registry.
     *
     * @param fd The file descriptor of the client.
     * @param channelName The name of the channel.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     * @throws NotMemberException if the client is not a member.
     */
    void removeClientFromChannel(int fd, const std::string& channelName);

    /**
     * @brief Removes a client from every channel it belongs to, deleting the channels left
     * empty. Intended for QUIT and disconnections. Does nothing if it has no channels.
     * @param fd The file descriptor of the client.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void removeClientFromAllChannels(int fd);

    /**
     * @brief Checks whether a client is a member of a channel.
     * @param fd The file descriptor of the client.
     * @param channelName The name of the channel.
     * @return true if the client is a member.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    bool isMember(int fd, const std::string& channelName) const;

    /**
     * @brief Grants or revokes channel operator status.
     * @param fd The file descriptor of the client.
     * @param channelName The name of the channel.
     * @param isOperator true to grant, false to revoke.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     * @throws NotMemberException if the client is not a member.
     */
    void setOperator(int fd, const std::string& channelName, bool isOperator);

    /**
     * @brief Checks whether a client is an operator of a channel.
     * @param fd The file descriptor of the client.
     * @param channelName The name of the channel.
     * @return true if the client is an operator.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    bool isOperator(int fd, const std::string& channelName) const;

    /**
     * @brief Invites a client to a channel. Inviting twice has no extra effect.
     * @param fd The file descriptor of the invited client.
     * @param channelName The name of the channel.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    void addInviteToChannel(int fd, const std::string& channelName);

    /**
     * @brief Consumes the invitation of a client. Without invitation it has no effect.
     * @param fd The file descriptor of the client.
     * @param channelName The name of the channel.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    void consumeInviteToChannel(int fd, const std::string& channelName);

    /**
     * @brief Checks whether a client has a pending invitation to a channel.
     * @param fd The file descriptor of the client.
     * @param channelName The name of the channel.
     * @return true if the client is invited.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws ChannelRegistry::ChannelNotFoundException if the channel does not exist.
     */
    bool isInvited(int fd, const std::string& channelName) const;
};

#endif // CHANNEL_MEMBERSHIP_MANAGER_HPP