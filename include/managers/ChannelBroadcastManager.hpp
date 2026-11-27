#ifndef CHANNEL_BROADCAST_MANAGER_HPP
#define CHANNEL_BROADCAST_MANAGER_HPP

#include "../channel/ChannelRegistry.hpp"
#include "./PendingIOManager.hpp"

class ChannelBroadcastManager {
  private:
    ChannelRegistry& channelRegistry;
    PendingIOManager& pendingIOManager;

  public:
    ChannelBroadcastManager(ChannelRegistry& channelRegistry, PendingIOManager& pendingIOManager);
    ~ChannelBroadcastManager();

    /**
     * @brief Sends a message to every member of a channel except the sender.
     *
     * Looks up the channel by name, collects its current members, excludes the
     * sender, and queues the message in the pending output of each remaining
     * member. The message is not written to the sockets directly; actual
     * delivery happens later, when the I/O loop flushes the pending output.
     *
     * @param senderFd     File descriptor of the client that originated the
     *                     message. This client will not receive the broadcast.
     * @param channelName  Name of the target channel.
     * @param message      Fully formatted message to deliver to the members.
     *
     * @throws Any exception thrown by ChannelRegistry::findChannel() if the
     *         channel does not exist.
     */
    void broadcast(int senderFd, const std::string& channelName, const std::string& message);
};

#endif // CHANNEL_BROADCAST_MANAGER_HPP